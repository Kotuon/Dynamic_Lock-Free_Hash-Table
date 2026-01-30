# Dynamic_Lock-Free_Hash-Table

Using the method discussed in High Performance Dynamic Lock-Free Hash Tables and List-Based Sets by Maged M. Michael, my goal was to create a mostly lock-free hash table.

​

For this project, a memory bank class I had previously created with locks was used to handle the allocation, deallocation, and acquisition of memory. This was the only part of the project that wasn't lock-free, a choice made so that I could focus all my attention on implementing the algorithm presented in the paper. If I were to revisit this project, changing the memory bank to a lock-free container would be my first step.

​

The hash table is constructed of a std::array with a predefined number of buckets. Each bucket is a linked list of nodes, with each node containing a key, value, and custom atomic struct called MarkPtrType. The MarkPtrType contains a pointer to the next node in the linked list, a boolean for signaling if the node has been deleted, and a tag for preventing the ABA problem.

​

There are four functions required for this project, Insert, Delete, Find, and Search.

 

Insert starts by constructing the new node with memory from the memory bank. It then searches the existing hash table to guarantee the key hasn't been used. If the key isn't used, it performs a compare and swap (CAS) process until successful.

 

Delete works in a very similar way to Insert. It first tries to locate the node in the list. If successful, it uses CAS to mark the node as deleted and then another CAS operation is used to remove the node from the list. If both these CAS operations are completed the memory is returned to the memory bank.

 

Search is a short function that takes the head node of a bucket, a key, and calls the Find function after creating prev, curr, and next pointers. Find starts to move along the linked list starting at the provided head, updating the provided prev, curr, and next pointers. If the node prev points to is changed by another thread it has to restart traversing from the beginning of the list.
