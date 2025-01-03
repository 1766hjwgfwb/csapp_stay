/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "headers/common.h"
#include "headers/algorithm.h"




static void print_node(linkedlist_t *node) {
    if (node == NULL)
        return;

    linkedlist_node_t *current = node->head;

    do {
        printf("%ld ", current->value);
        current = current->next;
    } while (current != node->head);

    printf("\n");
}


void print_list(linkedlist_t *list) {
    print_node(list);
}


linkedlist_t *linkedlist_construct() {
    // initialize a new linked list
    linkedlist_t *list = (linkedlist_t *)malloc(sizeof(linkedlist_t));

    list->head = NULL;
    list->count = 0;

    return list;
}


void linkedlist_free(linkedlist_t *list) {
    if (list == NULL)
        return;


    if (list->head == NULL) {
        free(list);
        return;
    }

    print_node(list);

    // free the memory of the linked list
    linkedlist_node_t *current = list->head;
    linkedlist_node_t *next;

    /*do {
        next = current->next;
        free(current);
        current = next;
    } while (current != list->head);
    */

    // break link
    list->head->prev->next = NULL;

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }

    free(list);
}


int linkedlist_add(linkedlist_t **address, uint64_t value) {
    // add a new node to the end of the linked list
    linkedlist_t *list = *address;
    if (list == NULL)
        return -1;

    // count == 0
    if (list->count == 0) {
        // init head node
        list->head = (linkedlist_node_t *)malloc(sizeof(linkedlist_node_t));
        list->head->value = value;
        // set the next and prev pointers to itself
        list->head->next = list->head; 
        list->head->prev = list->head;
        list->count++;
        return 0;
    } else {
        // count > 0
        linkedlist_node_t *new_node = (linkedlist_node_t *)malloc(sizeof(linkedlist_node_t));
        new_node->value = value;
        new_node->next = list->head;
        new_node->prev = list->head->prev;

        /*
            * init
            head <- -> head
            * new node
            head <- -> new node <- -> head
        */

        // update the prev pointer of the next node
        new_node->next->prev = new_node;
        // update the next pointer of the prev node
        new_node->prev->next = new_node;

        list->count++;

        return 0;
    }
}


int linkedlist_delete(linkedlist_t *list, linkedlist_node_t *node) {
    if (list == NULL || node == NULL)
        return -1;
    
    
    if (list->head == node) {
        // if the node is the head node
        if (list->count == 1)
            list->head = NULL;
        else {
            // update the head pointer
            list->head = node->next;
            // new node prev pointer to old_head prev pointer
            node->next->prev = node->prev;
            // old head prev pointer to new node
            node->prev->next = list->head;
        }
    } else {
        // if the node is not the head node
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }


    free(node);
    list->count--;



    return 0;
}


linkedlist_node_t *linkedlist_get(linkedlist_t *list, uint64_t value) {
    if (list->head == NULL)
        return NULL;

    linkedlist_node_t *current = list->head;

    while (current!= NULL) {
        if (current->value == value)
            return current;

        current = current->next;
    }

    // if the value is not found, return NULL
    return NULL;
}


// * iterator function to traverse the linked list
linkedlist_node_t *linkedlist_next(linkedlist_t *list) {
    if (list->head == NULL)
        return NULL;
    
    list->head = list->head->next;      // move the head to the next node

    return list->head->prev;            // return the head node or prev node
}
