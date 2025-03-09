/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

/* MESI protocol simulator */

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


// cacheline state (MESI)
typedef enum {
    MODIFIED,
    EXCLUSIVE,
    SHARED,
    INVALID,
}state_t;

typedef struct {
    state_t state;
    int value;
}cacheline_t;

#ifndef NUM_PROCESSOR
#define NUM_PROCESSOR (4)
#endif

// cache multi-processor
cacheline_t cache[NUM_PROCESSOR];

// mem cache (shared)
int mem_value = 15213;

int state_count[4];


// init and check cache state
int check_state() {
    state_count[MODIFIED] = 0;  // M
    state_count[EXCLUSIVE] = 0; // E
    state_count[SHARED] = 0;    // S
    state_count[INVALID] = 0;   // I


    for (int i = 0; i < NUM_PROCESSOR; i++) {
        state_count[(int)cache[i].state] += 1;
    }

    /*  MESI Rules Matrix:
        M   E   S   I
    M   X   X   X   O
    E   X   X   X   O
    S   X   X   O   O
    I   O   O   O   O
    */

    // verify MESI protocol
    if ((state_count[MODIFIED] == 1 && state_count[INVALID] == NUM_PROCESSOR - 1) || (state_count[EXCLUSIVE] == 1 && state_count[INVALID] == NUM_PROCESSOR - 1) || (state_count[SHARED] + state_count[INVALID] == NUM_PROCESSOR))
        return 1;

#ifdef DEBUG
    printf("state_count: M %d, E %d, S %d, I %d\n", state_count[MODIFIED], state_count[EXCLUSIVE], state_count[SHARED], state_count[INVALID]);
    exit(0);
#endif

    // verify error
    return 0;
}


// i for core
int read_cacheline(int i) {

    if (cache[i].state == MODIFIED) {
        // read hit
#ifdef DEBUG
        printf("[%d] read hit, value: %d\n", i, cache[i].value);
#endif
        return 1;
    } else if (cache[i].state == EXCLUSIVE) {
#ifdef DEBUG
        printf("[%d] read hit, value: %d\n", i, cache[i].value);
#endif
        return 1;
    } else if (cache[i].state == SHARED) {
#ifdef DEBUG
        printf("[%d] read hit, value: %d\n", i, cache[i].value);
#endif
        return 1;
    } else if (cache[i].state == INVALID) {

#ifdef DEBUG
        printf("\tbus broadcast: [%d] read", i);
#endif
        // read miss
        // bus broadcast read miss
        for (int j = 0; j < NUM_PROCESSOR; j++) {
            if (j != i) { 
                // another cache
                if (cache[j].state == MODIFIED ) {
                    // write back the dirty data to L3 cache
                    mem_value = cache[j].value;
                    cache[j].state = SHARED;

                    // update cache state, sync data
                    cache[i].state = SHARED;
                    cache[i].value = cache[j].value;

#ifdef DEBUG
                    printf("[%d] read miss, modified value: %d\n", i, cache[j].value);
#endif

                    return 1;
                } else if (cache[j].state == EXCLUSIVE) { 
                    cache[i].state = SHARED;
                    cache[i].value = cache[j].value;

                    cache[j].state = SHARED;

#ifdef DEBUG
                    printf("[%d] read miss, shared value: %d\n", i, cache[j].value);
#endif

                    return 1;
                } else if (cache[j].state == SHARED) {
                    // update cache state, sync data
                    cache[i].state = SHARED;
                    cache[i].value = cache[j].value;    
#ifdef DEBUG
                    printf("[%d] read miss, [%d] supplies data: %d\n", i, j, cache[j].value);
#endif
                    return 1;
                }
            }
        }

        // all others are invalid, update cache state, access L3 cache
        cache[i].state = EXCLUSIVE;
        cache[i].value = mem_value;

#ifdef DEBUG
        printf("[%d] read miss, exclusive value: %d\n", i, cache[i].value);
#endif
        return 1;
    }

    return -1;
}


int write_cacheline(int i, int write_value) {

    if (cache[i].state == MODIFIED) {
        // write hit
        // why can update cacheline value here?
        // because we have to update cacheline state to MODIFIED (local data)
        cache[i].value = write_value;

#ifdef DEBUG
        printf("[%d] write hit, value: %d\n", i, cache[i].value);
#endif  
        return 1;
    } else if (cache[i].state == EXCLUSIVE) {
        // write hit
        cache[i].value = write_value;
        cache[i].state = MODIFIED;  // dirty data
#ifdef DEBUG
        printf("[%d] write hit, value: %d\n", i, cache[i].value);
#endif
        return 1;
    } else if (cache[i].state == SHARED) { 
        // write hit, but need to update other cachelines
        for (int j = 0; j < NUM_PROCESSOR; j++) {
            if (j != i && cache[j].state != INVALID) {
                // another cacheline is invalid
                cache[j].state = INVALID;
                cache[j].value = -1;    // mark state as invalidated
#ifdef DEBUG
                printf("[%d] write hit, need invalidated cache [%d]\n", i, j);
#endif
            }
        }


        cache[i].value = write_value;
        cache[i].state = MODIFIED;  // dirty data

#ifdef DEBUG
        printf("[%d] write hit, value: %d\n", i, cache[i].value);
#endif
        return 1;
    } else if (cache[i].state == INVALID) {

#ifdef DEBUG
        printf("\tbus broadcast: [%d] write", i);
#endif
        
        for (int j = 0; j < NUM_PROCESSOR; j++) { 
            if (j != i && cache[j].state != INVALID) {
                if (cache[j].state == MODIFIED) {
                    // exisit only one modified copy, just ivalidate it
                    cache[j].state = INVALID;
                    cache[j].value = -1;    // mark state as invalidated

                    // update current cache
                    cache[i].state = MODIFIED;
                    cache[i].value = write_value;

#ifdef DEBUG
                    printf("[%d] write miss, ivaildated the modified value: %d\n", i, cache[j].value);
#endif
                    return 1;
                } else if (cache[j].state == EXCLUSIVE) {
                    // exisit only one exclusive copy, just ivalidate it
                    cache[j].state = INVALID;
                    cache[j].value = -1;    // mark state as invalidated

                    // update current cache
                    cache[i].state = MODIFIED;
                    cache[i].value = write_value;

#ifdef DEBUG
                    printf("[%d] write miss, ivalidated the exclusive value: %d\n", i, cache[j].value);
#endif
                    return 1;
                } else if (cache[j].state == SHARED) {
                    for (int k = 0; k < NUM_PROCESSOR; k++) {
                        if (k != i && cache[k].state != INVALID) {
                            cache[k].state = INVALID;
                            cache[k].value = -1;    // mark state as invalidated
                        }
                    }

                    cache[i].state = MODIFIED;
                    cache[i].value = write_value;

#ifdef DEBUG
                    printf("[%d] write miss, boadcast writing: %d\n", i, write_value);
#endif
                    return 1;
                }

            }
        }

        // all others are invalid, update cache state, access L3 cache
        cache[i].value = mem_value; // ?

        // update cache state, dirty data
        cache[i].state = MODIFIED;
        cache[i].value = write_value;
#ifdef DEBUG
        printf("[%d] write miss; no copies in chip; update value in place %d\n", i, write_value);
#endif
        return 1;
    }

    return -1;
}


// i - the index of current processor
int evict_cacheline(int i)
{
    if (cache[i].state == MODIFIED)
    {
        // write back to mem and transit to invalid
        mem_value = cache[i].value;

        // invalid this cache line since the physical address is no longer in the cache
        cache[i].state = INVALID;
        cache[i].value = 0;
#ifdef DEBUG
        printf("[%d] evict; write back value %d ** BUS WRITE **\n", i, mem_value);
#endif 
        return 1;
    }
    else if (cache[i].state != INVALID)
    {
        // we do not consider invalid evict since the paddr is already detached
        // for other states: invalid, exclusive, shared
        // they are all clean, so no bus I/O transaction
        cache[i].state = INVALID;
        cache[i].value = 0;
#ifdef DEBUG
        printf("[%d] evict in place\n", i);
#endif 
        return 1;
    }
    
    return 0;
}


#ifdef DEBUG
void print_cache()
{
    for (int i = 0; i < NUM_PROCESSOR; ++ i)
    {
        char c;
        switch (cache[i].state)
        {
            case MODIFIED:
                c = 'M';
                break;
            case EXCLUSIVE:
                c = 'E';
                break;
            case SHARED:
                c = 'S';
                break;
            case INVALID:
                c = 'I';
                break;
            default:
                break;
        }

        printf("\t[%4d]   state %c   value %d\n", i, c, cache[i].value);
    }
        printf("                            mem Shared cache copy: %d\n", mem_value);
}
#endif



int main() { 
    srand(12345);

    for (int i = 0; i < NUM_PROCESSOR; i++) { 
        cache[i].state = INVALID;
        cache[i].value = 0;
    }

#ifdef DEBUG
    print_cache();
#endif    

    int pc = 0;
    for (int i = 0; i < 20; ++ i) {
        int op_case = rand() % 3;
        int index_proc = rand() % NUM_PROCESSOR;

        if (op_case == 0) {
            pc = read_cacheline(index_proc);
        } else if (op_case == 1) {
            pc = write_cacheline(index_proc, rand());
        } else if (op_case == 2) {
            pc = evict_cacheline(index_proc);
        }

#ifdef DEBUG
    if (pc == 1) {
        print_cache();
    }
#endif

    if (check_state() == 0) {
        printf("Failed\n");
        return 0;
    }
}

    printf("Pass\n");
    

    return 0;
}