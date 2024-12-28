/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#ifndef ALGORITHM_H
#define ALGORITHM_H




//* =========================== */
//* ===== data structres ====== */
//* =========================== */


// * trie - prefix tree
typedef struct TRIE_NODE_STRUCT {
    // 128 is ascll code index
    struct TRIE_NODE_STRUCT* next[128];
    uint64_t address;
}trie_node_t;


// public api
void trie_insert(trie_node_t **root, char *key, uint64_t val);
int trie_get(trie_node_t *root, char *key, uint64_t *val);
void trie_free(trie_node_t *root);
void trie_print(trie_node_t *root);



// * hash table
typedef struct {
    int localdepth;     // local depth of this bucket
    int counter;
    char **karray;      // key array, Multiple key value
    uint64_t *varray;   // value array, Multiple key value
}bucket_t;

typedef struct {
    int size;           // bucket number
    int num;            // current hashtable element number
    int globaldepth;    

    bucket_t *barry;    // hash global depth, need Dynamic growth
}hash_table_t;


// public
hash_table_t *hashtable_construct(int bsize);
void hashtable_free(hash_table_t *tab);
int hashtable_get(hash_table_t *tab, char *key, uint64_t *val);
int hashtable_insert(hash_table_t *tab, char *key, uint64_t val);
void hashtable_print(hash_table_t *tab);
void hashtable_expand(hash_table_t *tab);



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



// * Dynammic array
typedef struct {
    uint32_t size;
    uint32_t count;
    uint64_t *table;
}array_t;


array_t *array_construct(int size);
void array_free(array_t *arr);
int array_insert(array_t **arr, uint64_t val);
int array_delete(array_t *arr, int index);
int array_get(array_t *arr, int index, uint64_t *valptr);
void print_array(array_t *arr);














#endif /* ALGORITHM_H */