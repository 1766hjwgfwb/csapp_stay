/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/common.h"



typedef void (*cleanup_t)();


// cleanup events array
static cleanup_t **events = NULL;
// number of cleanup events
static int events_count = 0;
// size of events array
static int events_size = 0;


// add a cleanup event to the array
void add_cleanup_event(void *func) {
    assert(func != NULL);

    cleanup_t *e = (cleanup_t *)func;

    // malloc if necessary
    if (events_size == 0 && events == NULL) {
        events_size = 8;
        events = (cleanup_t **)malloc(events_size * sizeof(cleanup_t *));

        events[0] = e;  // add first event, ponit to the first element(func)
        events_count++;  // increment count
        return;
    } 

    // expand array if necessary
    if (events_count == events_size) {
        events_size *= 2;

        cleanup_t **temp = (cleanup_t **)malloc(events_size * sizeof(cleanup_t *));

        // copy old events to new array
        for (int i = 0; i < events_count; i++) {
            temp[i] = events[i];
        }

        // add new event to the end of the new array
        temp[events_count] = e;
        events_count++;
        free(events);
        events = temp;
    } else {
        // add event to the end of the array
        events[events_count] = e;
        events_count++;
    }
}


// run all cleanup events
void finally_cleanup() {
    for (int i = 0; i < events_count; i++) { 
        (*events[i])();  // call each cleanup event
    }

    // events[i] no malloc, so no need to free it here
    free(events);  // free events array
}