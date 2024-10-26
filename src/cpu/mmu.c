#include"cpu/mmu.h"
#include"memory/dram.h"


uint64_t va2pa(uint64_t vaddr) {
    // * 虚拟地址定位到实际地址 
    // ? 取余防止内存溢出
    return vaddr % MM_LEN;
}








