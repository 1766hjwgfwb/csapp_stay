#ifndef DRAM_H 
#define DRAM_H



#include<stdint.h>


#define MM_LEN 1000

extern uint8_t mm[MM_LEN];     // py memory 8bit


// * 封装内存的 IO操作
uint64_t read64bits_dram(uint64_t paddr);       // ? 根据实际地址读虚拟地址
void wirtebits_dram(uint64_t paddr, uint64_t data);     // ? 根据虚拟地址写实际地址的值
void print_register();
void print_stack();




#endif