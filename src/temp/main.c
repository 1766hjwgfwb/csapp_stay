#include<stdio.h>

#include"cpu/register.h"
#include"cpu/mmu.h"
#include"memory/myinstruction.h"
#include"memory/dram.h"
#include"disk/elf.h"


// reg_t reg;

int main() {
    init_handler_table();


    // * vc value 
    reg.rax = 0x12340000;
    reg.rbx = 0x0;
    reg.rcx = 0x8000660;
    reg.rdx = 0xabcd;
    reg.rsi = 0x1;
    reg.rbp = 0x7ffffffee210;
    reg.rsp = 0x7ffffffee1f0;


    // * 反汇编指示 rip的指向指令结构体地址 ps: <main + 11>
    reg.rip = (uint64_t)&program[11];


    wirtebits_dram(va2pa(0x7ffffffee210), 0x8000660);   // * rbp
    wirtebits_dram(va2pa(0x7ffffffee208), 0x0);
    wirtebits_dram(va2pa(0x7ffffffee200), 0xabcd);
    wirtebits_dram(va2pa(0x7ffffffee1f8), 0x12340000);
    wirtebits_dram(va2pa(0x7ffffffee1f0), 0x8000660);   // * rsp

    // ? init value
    print_register();
    print_stack();

    // * run

    for(int i = 0; i < 15; i++) {
        instruction_cycle();
        print_register();
        print_stack();   // @bug 结构体地址不符合
    }
    /*  *((uint64_t *)(&mm[pa]))  一种强行访问内存的方式   8字节 64位 访问数组连续 64位地址 注意大小端 
        // printf("0x%lx\n", *((uint64_t* )(&mm[pa])));
    */

    // printf("0x%lx\n", read64bits_dram(va2pa(0x7ffffffee210))); // todo √


    // ? verify
    int match = 1;
    match = match && (reg.rax == 0x1234abcd);
    match = match && (reg.rbx == 0x0);
    match = match && (reg.rcx == 0x8000660);
    match = match && (reg.rdx == 0x12340000);
    match = match && (reg.rsi == 0xabcd);
    match = match && (reg.rdi == 0x12340000);
    match = match && (reg.rbp == 0x7ffffffee210);
    match = match && (reg.rsp == 0x7ffffffee1f0);

    printf("\n");
    if (match == 1) {
        printf("\tregister match\n");
    } else {
        printf("\tregister not match\n");
    }
}

/*  
*        ------rbp------
*        ---------------
*        ---------------
*        ------rsp------     
* 8bits  ---------------     rsp - 8 store rbp
*        ---------------     
*        ---------------     function local value                    
*        ---------------
*        ---------------     rsp -0x18\-0x10
*/

