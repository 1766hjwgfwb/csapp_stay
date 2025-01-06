/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/common.h"
#include "headers/algorithm.h"



typedef void (*cleanup_t)();

// * use dynamic array to store cleanup events
// cleanup events array
static array_t *cleanup_events = NULL;



// add a cleanup event to the array
void add_cleanup_event(void *func) {
    assert(func != NULL);

    if (cleanup_events == NULL) {
        // init - lazy malloc
        cleanup_events = array_construct(10);
    }


    // fill in the first event slot
    array_insert(&cleanup_events, (uint64_t)func);
    return;
}


// run all cleanup events
void finally_cleanup() {
    for (int i = 0; i < cleanup_events->count; i++) {
        // get the address of the cleanup function
        uint64_t address;
        assert(array_get(cleanup_events, i, &address) != 0);

        // *(uint64_t *)&func = (address);
        cleanup_t func = (cleanup_t)address;
        // call the cleanup function
        (*func)();
    }


    // clean up the array
    array_free(cleanup_events);
    cleanup_events = NULL;
    
    return;
}