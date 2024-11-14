#include"dram.h"
#include"cpu/register.h"
#include"cpu/mmu.h"
#include<stdio.h>


#define SRAM_CACHE_SETTING 0    // ? 哨兵   用于开启测试开关


uint8_t mm[MM_LEN];
// reg_t reg;


uint64_t read64bits_dram(uint64_t paddr) {
    if(SRAM_CACHE_SETTING == 1)
        return 0;

    uint64_t val = 0x0;

    // * 读取 64bits
    for(int i = 0, j = 0; i < 8; i++, j+=8)
        val += (((uint64_t)mm[paddr + i]) << j);

    return val;
}

void wirtebits_dram(uint64_t paddr, uint64_t data) {
    if(SRAM_CACHE_SETTING == 1)
        return;

    // * 拆分 64bits
    for(int i = 0, j = 0; i < 8; i++, j += 8) {
        mm[paddr + i] = (data >> j) & 0xff;
        // printf("mm addr: %p  value: %x\n", &mm[paddr + i], mm[paddr + i]); test
    }
}


void print_register() {
    printf("rax = 0x%lx\trbx = 0x%lx\trcx = 0x%lx\trdx = 0x%lx\n", reg.rax, reg.rbx, reg.rcx, reg.rdx);
    printf("rsi = 0x%lx\trdi = 0x%lx\trbp = 0x%lx\trsp = 0x%lx\n", reg.rsi, reg.rdi, reg.rbp, reg.rsp);
    printf("rip = 0x%lx\n", reg.rip);
    printf("\n");
}


void print_stack() {
    int n = 10;

    uint64_t *high = (uint64_t*)&mm[va2pa(reg.rsp)];
    high = &high[n];

    uint64_t rsp_start = reg.rsp + n * 8;

    for (int i = 0; i < 2 * n; ++ i)
    {
        uint64_t *ptr = (uint64_t *)(high - i);
        printf("0x%016lx : %16lx", rsp_start, (uint64_t)*ptr);

        if (i == n)
        {
            printf(" <== rsp");
        }

        rsp_start = rsp_start - 8;

        printf("\n");
    }
}