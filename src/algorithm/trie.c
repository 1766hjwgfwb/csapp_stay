/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// * Like smap mapping structure

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<headers/cpu.h>
#include<headers/memory.h>
#include<headers/common.h>
#include<headers/algorithm.h>


static void trie_dfs_print(trie_node_t *x, int level, char c);


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
        p = &(*p)->next[(int)key[i]];
    }

    if (*p == NULL)
        *p = malloc(sizeof(trie_node_t));   // for val creat a new node

    // map the value to the last node
    (*p)->address = val;
}


int trie_get(trie_node_t *root, char *key, uint64_t *val) {
    trie_node_t *p = root;
    for (int i = 0; i < strlen(key); i++) {
        if (p == NULL)
            return 0;

        p = p->next[(int)key[i]];
    }

    *val = p->address;  // get the value from the last node
    return 1; 
}


void trie_free(trie_node_t *root) {
    if (root == NULL)
        return;

    for (int i = 0; i < 128; i++)
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

        for (int i = 0; i < 128; i++)
            trie_dfs_print(x->next[i], level + 1, (char)i);
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