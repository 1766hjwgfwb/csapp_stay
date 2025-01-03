/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#ifndef ALGORITHM_H
#define ALGORITHM_H


#include <stdint.h>


//* =========================== */
//* ===== data structres ====== */
//* =========================== */

#define MAX_TRIE_NODE_NUM (37)


// * trie - prefix tree
typedef struct TRIE_NODE_STRUCT {
    // 128 is ascll code index
    struct TRIE_NODE_STRUCT* next[MAX_TRIE_NODE_NUM];
    uint64_t address;
    int isvalid;    // todo: add more flags
}trie_node_t;


// public api
void trie_insert(trie_node_t **root, char *key, uint64_t val);
int trie_get(trie_node_t *root, char *key, uint64_t *val);
void trie_free(trie_node_t *root);
void trie_print(trie_node_t *root);



// * hash table
typedef struct Pair {
    char* key;          // k-v pair key
    uint64_t val;
}pair_t;


typedef struct {
    int localdepth;     // local depth of this bucket, same localdepth bits int the one bucket
    int counter;        // current number of elements in this bucket
    int capacity;       // maximum number of elements in this bucket
    // char **karray;      // key array, Multiple key value
    // uint64_t *varray;   // value array, Multiple key value
    pair_t *data;        // key-value pairs array, Single key value
}bucket_t;


typedef struct Hash_Table {
    int bucket_capacity;           // bucket number
    // int num;            // current hashtable element number
    int directory_size;           // directory size, need Dynamic growth
    int globaldepth;    

    bucket_t **directory;    // hash global depth, need Dynamic growth
}hash_table_t;


extern hash_table_t *link_constant_dict;


// public
hash_table_t *hashtable_construct(int bsize);
void hashtable_free(hash_table_t *tab);
int hashtable_get(hash_table_t *tab, char* key, uint64_t *val);
int hashtable_insert(hash_table_t *tab_addr, char* key, uint64_t val);
int hashtable_delete(hash_table_t *tab, char* key);
void hashtable_print(hash_table_t *tab);




// * linked list - Circular Doubly Linked List
typedef struct LINKED_LIST_NODE_STRUCT {
    uint64_t value;
    struct LINKED_LIST_NODE_STRUCT *next;
    struct LINKED_LIST_NODE_STRUCT *prev;
}linkedlist_node_t;


typedef struct {
    linkedlist_node_t *head;
    uint64_t count; // number of nodes in the list
}linkedlist_t;


linkedlist_t *linkedlist_construct();
void linkedlist_free(linkedlist_t *list);
int linkedlist_add(linkedlist_t **address, uint64_t value);
int linkedlist_delete(linkedlist_t *list, linkedlist_node_t *node);
linkedlist_node_t *linkedlist_get(linkedlist_t *list, uint64_t value);
linkedlist_node_t *linkedlist_next(linkedlist_t *list);
void print_list(linkedlist_t *list);



// * Dynammic array
typedef struct {
    uint32_t size;      // maximum number of elements in this array
    uint32_t count;     // current number of elements in this array
    uint64_t *table;
}array_t;


array_t *array_construct(int size);
void array_free(array_t *arr);
int array_insert(array_t **arr, uint64_t val);
int array_delete(array_t *arr, int index);
int array_get(array_t *arr, int index, uint64_t *valptr);
void print_array(array_t *arr);














#endif /* ALGORITHM_H */