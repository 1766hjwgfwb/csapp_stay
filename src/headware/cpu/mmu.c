/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "headers/cpu.h"
#include "headers/memory.h"
#include "headers/common.h"



uint64_t va2pa(uint64_t vaddr) {
    // * vaddr % 0x7fff 保留15位地址
    // return vaddr & (0xffffffffffffffff >> (64 - MAX_INDEX_PHYSICAL_PACE));
    return vaddr % PHYSICAL_MEMORY_SPACE;
}






