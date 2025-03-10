/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// * Dynamic Random Access Memory
#include <string.h>
#include <assert.h>
#include "headers/cpu.h"
#include "headers/memory.h"
#include "headers/common.h"
#include "headers/address.h"

/*
    x86-64 little endian integer encoding

    so wirte 0x7a02ae00 to cahe, to memory lapping shoule be:
        00 ae 02 7a
*/

uint8_t pm[PHYSICAL_MEMORY_SPACE];



// read-write cache
uint8_t sram_cache_read(uint64_t paddr_value);
void sram_cache_write(uint64_t paddr_value, uint8_t data);


// memory accessing used in instructions
uint64_t cpu_read64bits_dram(uint64_t paddr) {
    if (DEBUG_ENABLE_SRAM_CACHE == 1) {
        // * try to load uint64_t from SRAM cache
        uint64_t val = 0x0;
        // read 64bits, little-endian
        for (int i = 0; i < 8; i++) {
            // read 8 bits each time
            val += (uint64_t)(sram_cache_read(paddr + i) << (i * 8));
        }

        return val;
    } else {
        // * read from DRAM directly
        uint64_t val = 0x0;

        // * read 64bits
        for(int i = 0, j = 0; i < 8; i++, j+=8)
            val += (((uint64_t)pm[paddr + i]) << j);

        return val;
    }
}

void cpu_write64bits_dram(uint64_t paddr, uint64_t data) {
    if (DEBUG_ENABLE_SRAM_CACHE == 1) {
        // try to load uint64_t from SRAM cache
        // write 64bits, little-endian
        for (int i = 0; i < 8; i++) {
            // write 8 bits each time
            sram_cache_write(paddr + i, (data >> (i * 8)) & 0xff);
        }
        
        return;
    } else {

        for(int i = 0, j = 0; i < 8; i++, j += 8) {
            pm[paddr + i] = (data >> j) & 0xff;
            // printf("mm addr: %p  value: %x\n", &mm[paddr + i], mm[paddr + i]); test
        }
    }
}

void cpu_readinst_dram(uint64_t paddr, char *buf) {

    // 类似 dram的读内存
    for (int i = 0; i < MAX_INSTRUCTION_CHAR; i++) 
        buf[i] = (char)pm[paddr + i];
}

void cpu_writeinst_dram(uint64_t paddr, const char *str) {

    // dram 的写内存 多余位用 0填充
    int len = strlen(str);
    assert(len < MAX_INSTRUCTION_CHAR);     // max char

    for (int i = 0; i < MAX_INSTRUCTION_CHAR; i++) {
        if ( i < len)
            pm[paddr + i] = (uint8_t)str[i];
        else
            pm[paddr + i] = 0;
    }
}



/* 
 * interface of I/O Bus: read and write between the SRAM cache and DRAM memory
 */
void bus_read_cacheline(uint64_t paddr, uint8_t *block) {
    uint64_t dram_base = ((paddr >> SRAM_CACHE_OFFSET_LEN) << SRAM_CACHE_OFFSET_LEN);

    // handle 64-bit cacheline
    for (int i = 0; i < (1 << SRAM_CACHE_OFFSET_LEN); i++) {
        block[i] = pm[dram_base + i];
    }
}


void bus_write_cacheline(uint64_t paddr, uint8_t *block) {
    uint64_t dram_base = ((paddr >> SRAM_CACHE_OFFSET_LEN) << SRAM_CACHE_OFFSET_LEN);

    // handle 64-bit cacheline
    for (int i = 0; i < (1 << SRAM_CACHE_OFFSET_LEN); i++) {
        pm[dram_base + i] = block[i];
    }
}