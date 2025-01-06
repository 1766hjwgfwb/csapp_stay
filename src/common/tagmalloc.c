/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */


// manage malloc and free for tags (GC)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headers/algorithm.h"
#include "headers/common.h"



static uint64_t compute_tag(char *str);
static void tag_destroy();


typedef struct {
    uint64_t tag;       // tag of the block
    uint64_t size;      // size of the block
    void *ptr;          // pointer to the block
}tag_block_t;


static linkedlist_t *tag_list = NULL;



void *tag_malloc(uint64_t size, char *tagstr) {
    // allocate memory for the block
    tag_block_t *block = (tag_block_t *)malloc(sizeof(tag_block_t));
    if (!block)
        return NULL;
    
    // set the tag and size of the block
    block->tag = compute_tag(tagstr);
    block->size = size;
    block->ptr = malloc(size);
    if (!block->ptr) {
        free(block);
        return NULL;
    }


    // create linked list node
    if (!tag_list) {
        tag_list = linkedlist_construct();
        // add cleanup event to free the list and blocks
        add_cleanup_event(&tag_destroy);
        // printf("tag_malloc: created tag list\n");
    }

    // add the block to the list
    linkedlist_add(&tag_list, (uint64_t)block);

    return block->ptr;
}


int tag_free(void *ptr) {
    if (!tag_list)
        return 0;

    // find the block in the list
    linkedlist_node_t *p = tag_list->head;
    for (int i = 0; i < tag_list->count; i++) {
        tag_block_t *bl = (tag_block_t *)p->value;
        if (bl->ptr == ptr) {
            // free heap memory
            linkedlist_delete(tag_list, p);
            free(bl->ptr);
            free(bl);
            return 1;
        }

        // iterate to the next node
        p = linkedlist_next(tag_list);
    }

    return 0;
}


void tag_sweep(char *tagstr) {
    if (!tag_list)
        return;

    // find the block in the list
    linkedlist_node_t *p = tag_list->head;
    for (int i = 0; i < tag_list->count; i++) {
        tag_block_t *bl = (tag_block_t *)p->value;
        if (bl->tag == compute_tag(tagstr)) {
            // free heap memory
            linkedlist_node_t *next_node = linkedlist_next(tag_list);

            // free heap memory
            free(bl->ptr);
            // free block
            free(bl);
            // tag_list->count--;
            // remove
            p = next_node;
        } else {
            // iterate to the next node
            p = linkedlist_next(tag_list);
        }
    }
}


static void tag_destroy() {
    printf("tag_destroy: freeing tag list\n");
    if (!tag_list)
        return;

    linkedlist_node_t *node = tag_list->head;
    for (int i = 0; i < tag_list->count; i++) {
        // get the block from the list
        tag_block_t *bl = (tag_block_t*)node->value;
        // free heap memory
        free(bl->ptr);
        // free block
        free(bl);
        // tag_list->count--;
        // iterate to the next node
        node = linkedlist_next(tag_list);
    }

    linkedlist_free(tag_list);
    tag_list = NULL;
}


// * hash
static uint64_t compute_tag(char *str) {
    int p = 31;
    int m = 1000000007;

    int k = p;
    int v = 0;
    for (int i = 0; i < strlen(str); ++ i)
    {
        v = (v + ((int)str[i] * k) % m) % m;
        k = (k * p) % m;
    }
    return v;
}


