/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// * Like smap mapping structure

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "headers/cpu.h"
#include "headers/memory.h"
#include "headers/common.h"
#include "headers/algorithm.h"


static void trie_dfs_print(trie_node_t *x, int level, char c);
static int get_index(char c);
static char get_char(int index);


static int get_index(char c) {
    if (c == '%')
        return 36;
    else if ('0' <= c && c <= '9')   // c - '0' will char to int
        return c - '0';
    else if ('a' <= c && c <= 'z')   // c - 'a' + 10 will char to int
        return c - 'a' + 10;

    return -1;
}


static char get_char(int index) {
    assert(0 <= index && index <= 36);

    if (index == 36)
        return '%';
    else if (0 <= index && index <= 9)
        return (char)('0' + index);
    else if (10 <= index && index <= 35)
        return (char)('a' + index - 10);


    return 0;
}


void trie_insert(trie_node_t **root, char *key, uint64_t val) {

    trie_node_t **p = root;
    for (int i = 0; i < strlen(key); i++) {
        if (*p == NULL)
            *p = malloc(sizeof(trie_node_t));

        // e.g. if key is "cat", then we need to create a node for "c", "a", "t" -> address = val
        // if key is "car", then we need to create a node for "c", "a", "r" -> address = val
        /*
        root
        |
        └── 'c' ->
                    |
                    └── 'a' -> 
                                |
                                ── 't' -> { address = 42 }
                                |

                                └── 'r' -> { address = ??? }   (new node)

        */

        // use ascii code to index the next node
        p = &(*p)->next[get_index(key[i])];
    }

    if (*p == NULL)
        *p = malloc(sizeof(trie_node_t));   // for val creat a new node

    // map the value to the last node
    (*p)->address = val;
}


// * add isvalid flag to check if the node is valid or not
int trie_insert_with_isvalid(trie_node_t **root, char *key, uint64_t val) {
    trie_node_t *p = *root;

    if (p == NULL)
        return -1;

    for (int i = 0; i < strlen(key); i++) {
        p->isvalid = 1;   // set root node isvalid flag to 1

        int id = get_index(key[i]);
        if (p->next[id] == NULL) {
            p->next[id] = malloc(sizeof(trie_node_t));
            p->next[id]->isvalid = 0;   // set new node isvalid flag to 0
            p->next[id]->address = 0;
        }

        p = p->next[id];
    }

    p->address = val; // set the value to the last node
    p->isvalid = 1;   // set last node isvalid flag to 1

    return 1;
}


int trie_get(trie_node_t *root, char *key, uint64_t *val) {
    trie_node_t *p = root;
    for (int i = 0; i < strlen(key); i++) {
        if (p == NULL)  // add isvalid || isvalid == 0
            return 0;

        p = p->next[get_index(key[i])];
    }

    *val = p->address;  // get the value from the last node
    return 1; 
}


void trie_free(trie_node_t *root) {
    if (root == NULL)
        return;

    for (int i = 0; i < MAX_TRIE_NODE_NUM; i++)
        trie_free(root->next[i]);

    free(root);
}


static void trie_dfs_print(trie_node_t *x, int level, char c) {
    if (x != NULL) {
        if (level > 0) {
            for (int i = 0; i < level - 1; i++)
                printf("\t");
            printf("[%c] %p\n", c, x);

            if (x->address != 0)
                printf("\t\t\t value: %ld\n", x->address);
        }

        for (int i = 0; i < MAX_TRIE_NODE_NUM; i++)
            trie_dfs_print(x->next[i], level + 1, get_char(i));
    }
}


void trie_print(trie_node_t *root) {
    if ((DEBUG_VERBOSE_SET & DEBUG_PARSEINST) == 0 )
        return;

    if (root == NULL) {
        printf("NULL\n");
        return;
    } else {
        printf("print trie:\n");
    }

    trie_dfs_print(root, 0, 0);
}