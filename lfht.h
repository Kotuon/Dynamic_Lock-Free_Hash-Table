
#ifndef LFHT_H
#define LFHT_H

#include <array>
#include <atomic>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#define BANK_SIZE 6000
#define BUCKET_AMOUNT 100

template < typename Key, typename Value >
class LFHT {
private:
    struct Node;

    struct MarkPtrType {
        Node *ptr;
        int tag;
        bool is_deleted;

        MarkPtrType() : ptr( nullptr ), tag( 0 ), is_deleted( false ) {}
        MarkPtrType( Node *ptr_, int tag_, bool is_deleted_ ) : ptr( ptr_ ), tag( tag_ ), is_deleted( is_deleted_ ) {}

        constexpr bool operator!=( const MarkPtrType &rhs ) {
            return !( ptr == rhs.ptr && tag == rhs.tag && is_deleted == rhs.is_deleted );
        }
    };

    struct Node {
        Key key;
        Value value;

        std::atomic< MarkPtrType > info;

        Node() : key(), value(), info( MarkPtrType() ) {}
        Node( Key key_, Value value_ ) : key( key_ ), value( value_ ), info( MarkPtrType() ) {}
        Node( const Node &other ) : key( other.key ), value( other.value ), info( MarkPtrType() ) {}

        constexpr bool operator!=( const Node &rhs ) {
            return !( key == rhs.key && value == rhs.value && !( info != rhs.info ) );
        }
    };

    class MemoryBank {
    public:
        /**
         * @brief Construct a new Memory Bank object with BANK_SIZE spaces
         *
         */
        MemoryBank() : bank( BANK_SIZE ) {
            for ( int i = 0; i < BANK_SIZE; ++i ) {
                bank[i] = reinterpret_cast< Node * >( new char[sizeof( Node )] );
            }
        }

        /**
         * @brief Adds more space to bank
         *
         */
        void Allocate() {
            for ( int i = 0; i < BANK_SIZE; ++i ) {
                bank.push_back( reinterpret_cast< Node * >( new char[sizeof( Node )] ) );
            }
        }

        /**
         * @brief Gives out front of bank and removes it from bank
         *
         * @return std::vector<int>* front of bank
         */
        Node *Get() {
            std::lock_guard< std::mutex > lock( m_bank );

            if ( bank.empty() )
                Allocate();

            Node *front = bank.front();
            bank.pop_front();
            if ( counter >= BANK_SIZE )
                front->~Node();
            else
                ++counter;
            return front;
        }

        /**
         * @brief Store given std::vector<int>* into back of bank
         *
         * @param to_add
         */
        void Store( Node *to_add ) {
            std::lock_guard< std::mutex > lock( m_bank );
            bank.push_back( to_add );
        }

        /**
         * @brief Destroy the Memory Bank object
         *
         */
        ~MemoryBank() {
            int c = 0;
            for ( Node *to_delete : bank ) {
                if ( ( BANK_SIZE - counter ) <= c )
                    to_delete->~Node();
                ++c;
                delete[] reinterpret_cast< char * >( to_delete );
            }
        }

    private:
        std::deque< Node * > bank; //!< Bank of Node* data
        std::mutex m_bank;         //!< For locking bank
        int counter = 0;
    };

    std::array< std::atomic< MarkPtrType >, BUCKET_AMOUNT > buckets = {};
    MemoryBank mb;

private:
    std::size_t Hash( Key key ) {
        return std::hash< Key >{}( key ) % BUCKET_AMOUNT;
    }

    bool Insert( std::atomic< MarkPtrType > *head, Node *new_node ) {
        Key key = new_node->key;
        MarkPtrType curr{ MarkPtrType() };
        MarkPtrType next{ MarkPtrType() };
        std::atomic< MarkPtrType > *prev{ nullptr };

        while ( true ) {
            if ( Find( head, key, &prev, curr, next ) )
                return false;

            new_node->info.store( MarkPtrType( curr.ptr, 0, false ) );

            MarkPtrType expected( curr.ptr, curr.tag, false );
            MarkPtrType desired( new_node, curr.tag + 1, false );

            if ( prev->compare_exchange_weak( expected, desired ) ) {
                return true;
            }
        }
    }

    bool Delete( std::atomic< MarkPtrType > *head, Key key ) {
        MarkPtrType curr{ MarkPtrType() };
        MarkPtrType next{ MarkPtrType() };
        std::atomic< MarkPtrType > *prev{ nullptr };
        while ( true ) {
            if ( !Find( head, key, &prev, curr, next ) )
                return false;

            MarkPtrType expected( next.ptr, next.tag, false );
            MarkPtrType desired( next.ptr, next.tag + 1, true );

            if ( !curr.ptr->info.compare_exchange_weak( expected, desired ) )
                continue;

            expected = MarkPtrType( curr.ptr, curr.tag, false );
            desired = MarkPtrType( next.ptr, curr.tag + 1, false );

            if ( prev->compare_exchange_weak( expected, desired ) ) {
                mb.Store( curr.ptr );
                curr.is_deleted = true;
            } else {
                Find( head, key, &prev, curr, next );
            }

            return true;
        }
    }

    bool Search( std::atomic< MarkPtrType > *head, Key key ) {
        MarkPtrType curr{ MarkPtrType() };
        MarkPtrType next{ MarkPtrType() };
        std::atomic< MarkPtrType > *prev{ nullptr };
        return Find( head, key, &prev, curr, next );
    }

    bool Find( std::atomic< MarkPtrType > *head, Key key, std::atomic< MarkPtrType > **prev, MarkPtrType &curr, MarkPtrType &next ) {
    try_again:
        *prev = head;
        curr = ( *prev )->load();

        while ( true ) {
            if ( !( curr.ptr ) )
                return false;

            Key c_key = curr.ptr->key;
            next = curr.ptr->info.load();

            if ( ( ( *prev )->load() ) != MarkPtrType( curr.ptr, curr.tag, false ) )
                goto try_again;

            if ( !next.is_deleted ) {
                if ( ( c_key ) >= ( key ) )
                    return c_key == key;

                *prev = &( curr.ptr->info );
            } else {

                MarkPtrType expected( curr.ptr, curr.tag, false );
                MarkPtrType desired( next.ptr, curr.tag + 1, false );

                if ( ( *prev )->compare_exchange_weak( expected, desired ) ) {
                    mb.Store( curr.ptr );
                    curr.is_deleted = true;
                    next.tag = curr.tag + 1;
                } else {
                    goto try_again;
                }
            }
            curr = next;
        }
    }

public:
    LFHT() : mb() {}

    ~LFHT() {
        for ( int i = 0; i < BUCKET_AMOUNT; ++i ) {
            Node *curr_node = buckets[i].load().ptr;
            while ( curr_node ) {
                Node *next_node = curr_node->info.load().ptr;
                mb.Store( curr_node );
                curr_node = next_node;
            }
        }
    }

    bool HashInsert( Key key, Value value ) {
        Node *new_node = mb.Get();
        new_node->key = key;
        new_node->value = value;

        if ( Insert( &buckets[Hash( key )], new_node ) ) {
            return true;
        }

        mb.Store( new_node );
        return false;
    }

    bool HashDelete( Key key ) {
        return Delete( &buckets[Hash( key )], key );
    }

    bool HashSearch( Key key ) {
        return Search( &buckets[Hash( key )], key );
    }

    void print() {
        for ( int i = 0; i < BUCKET_AMOUNT; ++i ) {
            Node *curr_node = buckets[i].load().ptr;
            printf( "%d: ", i );
            while ( curr_node ) {
                printf( "%d, %d | ", curr_node->key, curr_node->value );
                curr_node = curr_node->info.load().ptr;
            }
            printf( "\n" );
        }
    }
};

#endif
