
#include "lfht.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define INSERT_AMOUNT 11

static bool print_success = false;

void test_int() {
    LFHT< int, int > lfht;

    // Insert
    for ( int i = 0; i < INSERT_AMOUNT; ++i ) {
        if ( lfht.HashInsert( i, i ) ) {
            if ( print_success ) printf( "Inserted %d\n", i );
        } else {
            printf( "Failed to insert\n" );
            return;
        }
    }

    // Find
    for ( int i = 0; i < INSERT_AMOUNT; ++i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashSearch( i ) ) {
            if ( print_success ) printf( "Found %d\n", i );
        } else {
            std::string error = "Failed to find " + key + "\n";
            printf( "%s", error.c_str() );
            return;
        }
    }

    // Delete
    for ( int i = 0; i < INSERT_AMOUNT; ++i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashDelete( i ) ) {
            if ( print_success ) printf( "Deleted %d\n", i );
        } else {
            std::string error = "Failed to delete " + key + "\n";
            printf( "%s", error.c_str() );
            return;
        }
    }
    std::cout << "All good" << std::endl;
}

void test_int_rev() {
    LFHT< int, int > lfht;

    // Insert
    for ( int i = INSERT_AMOUNT - 1; i >= 0; --i ) {
        if ( lfht.HashInsert( i, i ) ) {
            if ( print_success ) printf( "Inserted %d\n", i );
        } else {
            printf( "Failed to insert\n" );
            return;
        }
    }

    // Find
    for ( int i = INSERT_AMOUNT - 1; i >= 0; --i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashSearch( i ) ) {
            if ( print_success ) printf( "Found %d\n", i );
        } else {
            std::string error = "Failed to find " + key + "\n";
            printf( "%s", error.c_str() );
            return;
        }
    }

    // Delete
    for ( int i = INSERT_AMOUNT - 1; i >= 0; --i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashDelete( i ) ) {
            if ( print_success ) printf( "Deleted %d\n", i );
        } else {
            std::string error = "Failed to delete " + key + "\n";
            printf( "%s", error.c_str() );
            return;
        }
    }
    std::cout << "All good" << std::endl;
}

void test_string() {
    LFHT< std::string, int > lfht;

    // Insert
    for ( int i = 0; i < INSERT_AMOUNT; ++i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashInsert( key, i ) ) {
            if ( print_success ) printf( "Inserted %d\n", i );
        } else {
            printf( "Failed to insert\n" );
            return;
        }
    }

    // Find
    for ( int i = 0; i < INSERT_AMOUNT; ++i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashSearch( key ) ) {
            if ( print_success ) printf( "Found %d\n", i );

        } else {
            std::string error = "Failed to find " + key + "\n";
            printf( "%s", error.c_str() );
            return;
        }
    }

    // Delete
    for ( int i = 0; i < INSERT_AMOUNT; ++i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashDelete( key ) ) {
            if ( print_success ) printf( "Deleted %d\n", i );
        } else {
            std::string error = "Failed to delete " + key + "\n";
            printf( "%s", error.c_str() );
            return;
        }
    }
    std::cout << "All good" << std::endl;
}

void test_string_rev() {
    LFHT< std::string, int > lfht;

    // Insert
    for ( int i = INSERT_AMOUNT - 1; i >= 0; --i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashInsert( key, i ) ) {
            if ( print_success ) printf( "Inserted %d\n", i );
        } else {
            printf( "Failed to insert\n" );
            return;
        }
    }

    // Find
    for ( int i = INSERT_AMOUNT - 1; i >= 0; --i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashSearch( key ) ) {
            if ( print_success ) printf( "Found %d\n", i );

        } else {
            std::string error = "Failed to find " + key + "\n";
            printf( "%s", error.c_str() );
            return;
        }
    }

    // Delete
    for ( int i = INSERT_AMOUNT - 1; i >= 0; --i ) {
        std::string key( std::to_string( i ) );
        if ( lfht.HashDelete( key ) ) {
            if ( print_success ) printf( "Deleted %d\n", i );
        } else {
            std::string error = "Failed to delete " + key + "\n";
            printf( "%s", error.c_str() );
            return;
        }
    }
    std::cout << "All good" << std::endl;
}

LFHT< int, int > lfht_int_thd;

void insert_range( int b, int e ) {
    int *range = new int[e - b];
    for ( int i = b; i < e; ++i ) {
        range[i - b] = i;
    }

    for ( int i = 0; i < e - b; ++i ) {
        lfht_int_thd.HashInsert( range[i], range[i] );
    }

    delete[] range;
}

void delete_range( int b, int e ) {
    int *range = new int[e - b];
    for ( int i = b; i < e; ++i ) {
        range[i - b] = i;
    }

    for ( int i = 0; i < e - b; ++i ) {
        lfht_int_thd.HashDelete( range[i] );
    }

    delete[] range;
}

void test_int_thd( int num_threads, int num_per_thread, bool print ) {
    std::vector< std::thread > add_threads;

    for ( int i = 0; i < num_threads; ++i ) {
        add_threads.push_back( std::thread( insert_range, i * num_per_thread, ( i + 1 ) * num_per_thread ) );
    }

    for ( std::thread &thd : add_threads ) {
        thd.join();
    }

    printf( "Inserted\n" );

    if ( print )
        lfht_int_thd.print();

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        if ( !lfht_int_thd.HashSearch( i ) ) {
            std::cout << "Error: " << i << std::endl;
            return;
        }
    }

    printf( "Found all\n" );

    std::vector< std::thread > del_threads;

    for ( int i = 0; i < num_threads; ++i ) {
        del_threads.push_back( std::thread( delete_range, i * num_per_thread, ( i + 1 ) * num_per_thread ) );
    }

    for ( std::thread &thd : del_threads ) {
        thd.join();
    }

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        if ( lfht_int_thd.HashSearch( i ) ) {
            std::cout << "Error: " << i << std::endl;
            return;
        }
    }

    printf( "Deleted all\n" );
}

void test_int_cmp_map( int num_threads, int num_per_thread ) {
    std::vector< std::thread > threads;

    auto start = std::chrono::steady_clock::now();

    for ( int i = 0; i < num_threads; ++i ) {
        threads.push_back( std::thread( insert_range, i * num_per_thread, ( i + 1 ) * num_per_thread ) );
    }

    for ( std::thread &thd : threads ) {
        thd.join();
    }

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        if ( !lfht_int_thd.HashSearch( i ) ) {
            std::cout << "Error: " << i << std::endl;
            return;
        }
    }

    auto end = std::chrono::steady_clock::now();

    std::cout << "All good - lfht: " << std::chrono::duration_cast< std::chrono::microseconds >( end - start ).count() << std::endl;

    std::unordered_map< int, int > hash;

    start = std::chrono::steady_clock::now();

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        hash.insert( { i, i } );
    }

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        if ( hash.find( i ) == hash.end() ) {
            std::cout << "Error: " << i << std::endl;
            return;
        }
    }

    end = std::chrono::steady_clock::now();
    std::cout << "All good - unoredered_map: " << std::chrono::duration_cast< std::chrono::microseconds >( end - start ).count() << std::endl;
}

void test_int_cmp_set( int num_threads, int num_per_thread ) {
    auto start = std::chrono::steady_clock::now();

    std::vector< std::thread > threads;
    for ( int i = 0; i < num_threads; ++i ) {
        threads.push_back( std::thread( insert_range, i * num_per_thread, ( i + 1 ) * num_per_thread ) );
    }

    for ( std::thread &thd : threads ) {
        thd.join();
    }

    auto end = std::chrono::steady_clock::now();

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        if ( !lfht_int_thd.HashSearch( i ) ) {
            std::cout << "Error: " << i << std::endl;
            return;
        }
    }

    std::cout << "All good - lfht: " << std::chrono::duration_cast< std::chrono::microseconds >( end - start ).count() << std::endl;

    std::unordered_set< int > hash;

    start = std::chrono::steady_clock::now();

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        hash.insert( i );
    }

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        if ( hash.find( i ) == hash.end() ) {
            std::cout << "Error: " << i << std::endl;
            return;
        }
    }

    end = std::chrono::steady_clock::now();
    std::cout << "All good - unordered_set: " << std::chrono::duration_cast< std::chrono::microseconds >( end - start ).count() << std::endl;
}

// Single thread
void test0() { test_int(); }
void test1() { test_int_rev(); }
void test2() { test_string(); }
void test3() { test_string_rev(); }


void test_int_stress( int num_threads, int num_per_thread, bool print ) {
    std::vector< std::thread > add_threads;

    for ( int i = 0; i < num_threads; ++i ) {
        add_threads.push_back( std::thread( insert_range, i * num_per_thread, ( i + 1 ) * num_per_thread ) );
    }

    for ( std::thread &thd : add_threads ) {
        thd.join();
    }

    printf( "Inserted\n" );

    if ( print )
        lfht_int_thd.print();

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        if ( !lfht_int_thd.HashSearch( i ) ) {
            std::cout << "Error: " << i << std::endl;
            return;
        }
    }

    printf( "Found all\n" );

    std::vector< std::thread > del_threads;

    for ( int i = 0; i < num_threads; ++i ) {
        del_threads.push_back( std::thread( delete_range, i * num_per_thread, ( i + 1 ) * num_per_thread ) );
    }

    for ( std::thread &thd : del_threads ) {
        thd.join();
    }

    for ( int i = 0; i < num_threads * num_per_thread; ++i ) {
        if ( lfht_int_thd.HashSearch( i ) ) {
            std::cout << "Error: " << i << std::endl;
            return;
        }
    }

    printf( "Deleted all\n" );
}

// Multi thread
void test4() { test_int_thd( 2, 10, false ); }
void test5() { test_int_thd( 4, 10, false ); }
void test6() { test_int_thd( 4, 25, false ); }
void test7() { test_int_thd( 4, 40, false ); }
void test8() { test_int_thd( 4, 80, false ); }
void test9() { test_int_thd( 4, 100, false ); }
void test10() { test_int_thd( 8, 60, false ); }
void test11() { test_int_thd( 8, 100, false ); }
void test12( int num_threads, int num_per_thread ) { test_int_thd( num_threads, num_per_thread, false ); }
void test13( int num_threads, int num_per_thread ) { test_int_cmp_map( num_threads, num_per_thread ); }
void test14( int num_threads, int num_per_thread ) { test_int_cmp_set( num_threads, num_per_thread ); }
void test15( int num_threads, int num_per_thread ) { test_int_thd( num_threads, num_per_thread, true ); }

void ( *pTests[] )() = {
    test0, test1, test2, test3, test4, test5, test6, test7, test8, test9, test10, test11 };

int main( int argc, char **argv ) {
    int test = 0;
    std::sscanf( argv[1], "%i", &test );
    if ( test == 12 && argc == 4 ) {
        int num_threads = 0;
        int num_per_thread = 0;
        std::sscanf( argv[2], "%i", &num_threads );
        std::sscanf( argv[3], "%i", &num_per_thread );

        test12( num_threads, num_per_thread );
    } else if ( test == 13 && argc == 4 ) {
        int num_threads = 0;
        int num_per_thread = 0;
        std::sscanf( argv[2], "%i", &num_threads );
        std::sscanf( argv[3], "%i", &num_per_thread );

        test13( num_threads, num_per_thread );
    } else if ( test == 14 && argc == 4 ) {
        int num_threads = 0;
        int num_per_thread = 0;
        std::sscanf( argv[2], "%i", &num_threads );
        std::sscanf( argv[3], "%i", &num_per_thread );

        test14( num_threads, num_per_thread );
    } else if ( test == 15 && argc == 4 ) {
        int num_threads = 0;
        int num_per_thread = 0;
        std::sscanf( argv[2], "%i", &num_threads );
        std::sscanf( argv[3], "%i", &num_per_thread );

        test15( num_threads, num_per_thread );
    } else if ( argc == 2 ) {
        try {
            pTests[test]();
        } catch ( const char *msg ) {
            std::cerr << msg << std::endl;
        }
    }

    return 0;
}
