/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// virtual memory management for SRAM, L1, L2 cache and L3 cache
// cache hit : cache block is in cache and is valid, can be used directly
// cache miss : cache block is not in cache or is invalid, need to fetch from memory
// soluition : use a cache table to store cache block information, including valid bit, tag, data, etc.

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/address.h"
#include "headers/memory.h"

/*
    S = 2^s, s = 6
    B = 2^b, b = 6
    E is defined in sram.c, 8
    For correctness verification, E can be 1, 2, 8, 1024
    monitoring:
    cache_hit_count
    cache_miss_count
    cache_evict_count
    dirty_bytes_in_cache_count
    dirty_bytes_evicted_count
 */

#ifdef CACHE_SIMULATION_VERIFICATION

int cache_hit_count = 0;
int cache_miss_count = 0;
int cache_evict_count = 0;
int dirty_bytes_in_cache_count = 0;
int dirty_bytes_evicted_count = 0;

char trace_buf[20];
char *trace_ptr = (char *)&trace_buf;
#else
#define NUM_CACHE_LINE_PER_SET (8)

#endif


// write-back and write-allocate
typedef enum {
    // state of cache line
    CACHE_LINE_INVALID = 0,
    CACHE_LINE_CLEAN = 1,
    CACHE_LINE_DIRTY = 2,
}sram_cacheline_state_t;



typedef struct {
    sram_cacheline_state_t state;

    int time;  //used for LRU algorithm
    uint64_t tag;
    // i7 use 40 bytes
    uint8_t blcok[(1 << SRAM_CACHE_OFFSET_LEN)];
}sram_cacheline_t;


typedef struct {
    // i7 use 8 cache lines per set
    sram_cacheline_t lines[NUM_CACHE_LINE_PER_SET];
}sram_cacheset_t;


typedef struct {
    // i7 use 40 groups of cache sets
    sram_cacheset_t sets[( 1 << SRAM_CACHE_INDEX_LEN)];
}sram_cache_t;


// * interior variables for cache management
static sram_cache_t cache;


uint8_t sram_cache_read(uint64_t paddr_value) {
    address_t paddr = {
        .paddr_value = paddr_value,
    };

    // set index
    sram_cacheset_t *set = &cache.sets[paddr.CI];

    // update LRU time
    sram_cacheline_t *victim = NULL;
    sram_cacheline_t *invalid = NULL;
    int max_time = -1;

    for (int i = 0; i < NUM_CACHE_LINE_PER_SET; i++) {
        // get cache line address
        sram_cacheline_t *line = &(set->lines[i]);

        line->time++;

        // find the victim cache line
        if (max_time < line->time) {
            // select this line as victim by LRU algorithm
            // replace it when all lines are invalid
            victim = line;
            max_time = line->time;
        }

        if (line->state == CACHE_LINE_INVALID) {
            // exist one invalid cache line as candidate for cache miss
            invalid = line;
        }
    }

    // * try cache hit
    for (int i = 0; i < NUM_CACHE_LINE_PER_SET; i++) {
        // get cache line
        // sram_cacheline_t line = set.lines[i];
        sram_cacheline_t *line = &(set->lines[i]);

        if (line->state != CACHE_LINE_INVALID && line->tag == paddr.CT) {

    #ifdef CACHE_SIMULATION_VERIFICATION
            sprintf(trace_buf, "hit");  // hit ++
            cache_hit_count++;
    #endif
            // cache hit, return data (one byte)
            // set time to 0, update LRU
            line->time = 0;
            // find the cache byte
            return line->blcok[paddr.CO];
        }
    }

    // * cache miss, fetch data from memory
    #ifdef CACHE_SIMULATION_VERIFICATION
        sprintf(trace_buf, "miss");  // miss ++
        cache_miss_count++;
    #endif
    
    // try to fine one free cache line
    if (invalid != NULL) {
    #ifndef CACHE_SIMULATION_VERIFICATION
        // load data from memory to invalid cache line
        bus_read_cacheline(paddr.paddr_value, invalid->blcok);
    #endif
        
        // update cache line state
        invalid->state = CACHE_LINE_CLEAN;

        // update LRU time
        invalid->time = 0;

        // update cache line tag
        invalid->tag = paddr.CT;

        return invalid->blcok[paddr.CO];
    }

    assert(victim != NULL);
    // no free cache line, use LRU policy to replace cache line
    if (victim->state == CACHE_LINE_DIRTY) {
    #ifndef CACHE_SIMULATION_VERIFICATION
        // * "write-back" the dirty cache line to dram
        bus_write_cacheline(paddr.paddr_value, victim->blcok);
    #else
        // have data consistency, dirty bytes in cache count
        dirty_bytes_evicted_count += (1 << SRAM_CACHE_OFFSET_LEN);
        dirty_bytes_in_cache_count -= (1 << SRAM_CACHE_OFFSET_LEN);
    #endif

        // update victim cache line state
        // victim->state = CACHE_LINE_INVALID;
    }

    #ifdef CACHE_SIMULATION_VERIFICATION
        // discard this victim directly "CACHE_LINE_CLEAN"
        sprintf(trace_buf, "miss eviction");
        cache_evict_count++;
    #endif
    
    // if CACHE_LINE_CLEAN discard this victim directly
    victim->state = CACHE_LINE_INVALID;

    #ifndef CACHE_SIMULATION_VERIFICATION
    // read from dram
        bus_read_cacheline(paddr.paddr_value, victim->blcok);
    #endif

    // update LRU time
    victim->time = 0;

    // update cache line state, from dram to cache
    victim->state = CACHE_LINE_CLEAN;

    // update cache line tag
    victim->tag = paddr.CT;

    // return cache line data
    return victim->blcok[paddr.CO];
}


void sram_cache_write(uint64_t paddr_value, uint8_t data) {
    address_t paddr = {
        .paddr_value = paddr_value,
    };

    // set index
    sram_cacheset_t *set = &(cache.sets[paddr.CI]);

    // update LRU time
    sram_cacheline_t *victim = NULL;
    sram_cacheline_t *invalid = NULL;
    int max_time = -1;

    for (int i = 0; i < NUM_CACHE_LINE_PER_SET; i++) {
        // get cache line address
        sram_cacheline_t *line = &(set->lines[i]);

        // update LRU time
        line->time++;

        if (max_time < line->time) {
            // select this line as victim by LRU algorithm
            // replace it when all lines are invalid
            victim = line;
            max_time = line->time;
        }

        if (line->state == CACHE_LINE_INVALID) {
            // exist one invalid cache line as candidate for cache miss
            invalid = line;
        }
    }

    // try cache hit
    for (int i = 0; i < NUM_CACHE_LINE_PER_SET; i++) {
        // get cache line address, becasue we need to update cache line data
        // sram_cacheline_t line = set.lines[i];
        sram_cacheline_t *line = &(set->lines[i]);

        if (line->state != CACHE_LINE_INVALID && line->tag == paddr.CT) {
    #ifdef CACHE_SIMULATION_VERIFICATION
            // cache hit
            sprintf(trace_buf, "hit");  // hit ++
            cache_hit_count++;

            if (line->state == CACHE_LINE_CLEAN) {
                dirty_bytes_in_cache_count += (1 << SRAM_CACHE_OFFSET_LEN);
            }
    #endif

            // update LRU time
            line->time = 0;

            line->blcok[paddr.CO] = data;
            
            // data 
            line->state = CACHE_LINE_DIRTY;

            return;
        }
    }
 
    #ifdef CACHE_SIMULATION_VERIFICATION
    // cache miss, fetch data from memory
        sprintf(trace_buf, "miss");  // miss ++
        cache_miss_count++;
    #endif

    // * write-allocate

    // try to fine one free cache line
    if (invalid != NULL) {
    #ifndef CACHE_SIMULATION_VERIFICATION
        // load data from memory to invalid cache line
        bus_read_cacheline(paddr.paddr_value, invalid->blcok);
    #else
        dirty_bytes_in_cache_count += (1 << SRAM_CACHE_OFFSET_LEN);
    #endif

        // update cache line state
        invalid->state = CACHE_LINE_DIRTY;

        // update LRU time
        invalid->time = 0;

        // update cache line tag
        invalid->tag = paddr.CT;

        // update cache line data
        invalid->blcok[paddr.CO] = data;

        return;
    }

    // no free cache line, use LRU policy
    assert(victim != NULL);

    // if CACHE_LINE_DIRTY discard this victim directly
    if (victim->state == CACHE_LINE_DIRTY) {
    #ifndef CACHE_SIMULATION_VERIFICATION
        // * "write-back" the dirty cache line to dram
        // Write-back `data consistency`
        bus_write_cacheline(paddr.paddr_value, victim->blcok);
    #else
        dirty_bytes_evicted_count += (1 << SRAM_CACHE_OFFSET_LEN);
        dirty_bytes_in_cache_count -= (1 << SRAM_CACHE_OFFSET_LEN);
    #endif
    }

    #ifdef CACHE_SIMULATION_VERIFICATION
    // if CACHE_LINE_CLEAN Direct update state
        sprintf(trace_buf, "miss eviction");
        cache_evict_count++;
        dirty_bytes_in_cache_count += (1 << SRAM_CACHE_OFFSET_LEN);
    #endif

    // update state
    victim->state = CACHE_LINE_INVALID;

    #ifndef CACHE_SIMULATION_VERIFICATION
    // read from dram
    // load data from memory to victim cache line
    bus_read_cacheline(paddr.paddr_value, victim->blcok);
    #endif

    // update cache line state
    victim->state = CACHE_LINE_DIRTY;

    // update LRU time
    victim->time = 0;

    // update cache line tag
    victim->tag = paddr.CT;


    // update cache line data
    victim->blcok[paddr.CO] = data;
}