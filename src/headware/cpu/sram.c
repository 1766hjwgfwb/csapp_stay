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
#include "headers/address.h"
#include "headers/memory.h"


#define NUM_CACHE_LINE_PER_SET (8)


/*
 * internal I/O Bus functions
*/
static void bus_write_from_sram_to_dram(address_t paddr, uint8_t *block);
static void bus_read_from_dram_to_sram(address_t paddr, uint8_t *block);


// read-write cache
uint8_t sram_cache_read(address_t paddr);
void sram_cache_write(address_t paddr, uint8_t data);


// write-back and write-allocate
typedef enum {
    // state of cache line
    CACHE_LINE_INVALID = 0,
    CACHE_LINE_CLEAN = 1,
    CACHE_LINE_DIRTY = 2,
}sram_cacheline_state_t;


// write-through and no-write-allocate
/*
typedef enum {
    CACHE_LINE_INVALID2 = 0,
    CACHE_LINE_CLEAN2 = 1,
    CACHE_LINE_DIRTY2 = 2,
}sram_cacheline_state_t2;
*/


typedef struct {
    sram_cacheline_state_t state;

    uint64_t time;  //used for LRU algorithm
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


uint8_t sram_cache_read(address_t paddr) {
    // set index
    sram_cacheset_t set = cache.sets[paddr.CI];

    // update LRU time
    sram_cacheline_t *victim = NULL;
    sram_cacheline_t *invalid = NULL;
    int max_time = -1;

    for (int i = 0; i < NUM_CACHE_LINE_PER_SET; i++) {
        set.lines[i].time++;
        // find the victim cache line
        if (max_time < set.lines[i].time) {
            // select this line as victim by LRU algorithm
            // replace it when all lines are invalid
            victim = &set.lines[i];
            max_time = set.lines[i].time;
        }

        if (set.lines[i].state == CACHE_LINE_INVALID) {
            // exist one invalid cache line as candidate for cache miss
            invalid = &set.lines[i];
        }
    }

    // try cache hit
    for (int i = 0; i < NUM_CACHE_LINE_PER_SET; i++) {
        // get cache line
        sram_cacheline_t line = set.lines[i];

        if (line.state != CACHE_LINE_INVALID && line.tag == paddr.CT) {
            // cache hit, return data (one byte)
            // set time to 0, update LRU
            line.time = 0;
            // find the cache byte
            return line.blcok[paddr.CO];
        }
    }

    // * cache miss, fetch data from memory
    
    // try to fine one free cache line
    if (invalid != NULL) {
        // load data from memory to invalid cache line
        bus_read_from_dram_to_sram(paddr, &invalid->blcok);
        
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
        // * "write-back" the dirty cache line to dram
        bus_write_from_sram_to_dram(paddr, victim);

        // update victim cache line state
        victim->state = CACHE_LINE_INVALID;
    }
    
    // if CACHE_LINE_CLEAN discard this victim directly
    victim->state = CACHE_LINE_CLEAN;

    // read from dram
    bus_read_from_dram_to_sram(paddr, &victim->blcok);

    // update LRU time
    victim->time = 0;

    // update cache line tag
    victim->tag = paddr.CT;

    // return cache line data
    return victim->blcok[paddr.CO];
}


void sram_cache_write(address_t paddr, uint8_t data) {
    // set index
    sram_cacheset_t set = cache.sets[paddr.CI];
    for (int i = 0; i < NUM_CACHE_LINE_PER_SET; i++) {
        // get cache line
        sram_cacheline_t line = set.lines[i];

        if (line.state != CACHE_LINE_INVALID && line.tag == paddr.CT) {
            // cache hit, return data (one byte)
            // todo: update LRU
            line.blcok[paddr.CO] = data;

            // * if write-through, no-write-allocate, write data to memory
            // get the cache base address
            uint64_t base = (paddr.address_value >> SRAM_CACHE_OFFSET_LEN) << SRAM_CACHE_OFFSET_LEN;

            for (int j = 0; j < ((1 << SRAM_CACHE_OFFSET_LEN) / sizeof(uint64_t)); j++) { 
                // base start 0, 8, 16, 24, 32, 40, 48, 56
                // write 64 bits data to dram
                // *(ui64_t *) can handle 64-bit data of uint8* block(one byte per block)
                write64bits_dram(base + (j * sizeof(uint64_t)), *(uint64_t *)&line.blcok[j * sizeof(uint64_t)]);
            }
            return;
        }
    }

    // cache miss, fetch data from memory
    // todo: update LRU

    return 0;
}


static void bus_write_from_sram_to_dram(address_t paddr, uint8_t *block) {
    uint64_t size64 = sizeof(uint64_t);

    // * >> + << clean the cache line offset bits
    // get the cache base address
    uint64_t dram_base = ((paddr.address_value >> SRAM_CACHE_OFFSET_LEN) << SRAM_CACHE_OFFSET_LEN);
    
    // block pointer to base address of cache line
    uint64_t block_base = (uint64_t)block;

    for (int i = 0; i < ((1 << SRAM_CACHE_OFFSET_LEN) / size64); i++) {
        uint64_t dram_addr = dram_base + (i * size64);
        uint64_t block_addr = block_base + (i * size64);

        // write dram data to cache line, 64 byte
        // *(uint64_t *) can handle 64-bit data of uint8* block(one byte per block)
        uint64_t value = *(uint64_t *)block_addr;
        write64bits_dram(dram_addr, value);
    }
}


static void bus_read_from_dram_to_sram(address_t paddr, uint8_t *block) {
    uint64_t size64 = sizeof(uint64_t);

    // * >> + << clean the cache line offset bits
    // get the cache base address
    uint64_t dram_base = ((paddr.address_value >> SRAM_CACHE_OFFSET_LEN) << SRAM_CACHE_OFFSET_LEN);
    
    // block pointer to base address of cache line
    uint64_t block_base = (uint64_t)block;

    for (int i = 0; i < ((1 << SRAM_CACHE_OFFSET_LEN) / size64); i++) {
        uint64_t dram_addr = dram_base + (i * size64);
        uint64_t block_addr = block_base + (i * size64);

        // read dram data to cache line, 64 byte
        // *(uint64_t *) can handle 64-bit data of uint8* block(one byte per block)
        *(uint64_t *)block_addr = read64bits_dram(dram_addr);
    }
}