/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// * Dynamic Random Access Memory
#include<headers/cpu.h>
#include<headers/memory.h>
#include<headers/common.h>

/*
    x86-64 little endian integer encoding

    so wirte 0x7a02ae00 to cahe, to memory lapping shoule be:
        00 ae 02 7a
*/

uint8_t pm[PHYSICAL_MEMORY_SPACE];


// memory accessing used in instructions
uint64_t read64bits_dram(uint64_t paddr, core_t *cr) {
    if (DEBUG_ENABLE_SRAM_CACHE == 1) {
        // * try to load uint64_t from SRAM cache
        return 0x0;
    } else {
        // * read from DRAM directly
        uint64_t val = 0x0;

        // * read 64bits
        for(int i = 0, j = 0; i < 8; i++, j+=8)
            val += (((uint64_t)pm[paddr + i]) << j);

        return val;
    }
}

void wirte64bits_dram(uint64_t paddr, uint64_t data, core_t *cr) {
    if (DEBUG_ENABLE_SRAM_CACHE == 1) {
        return;
    } else {

        for(int i = 0, j = 0; i < 8; i++, j += 8) {
            pm[paddr + i] = (data >> j) & 0xff;
            // printf("mm addr: %p  value: %x\n", &mm[paddr + i], mm[paddr + i]); test
        }
    }
}