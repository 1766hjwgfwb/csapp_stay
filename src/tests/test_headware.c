/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include<stdio.h>
#include<string.h>
#include<headers/cpu.h>
#include<headers/memory.h>
#include<headers/common.h>



#define MAX_NUM_INSTRUCTION_CYCLE 100


// * extern var 
core_t cores[NUM_CORES];    // 内核数组

uint64_t ACTIVE_CORE;



static void TestAddFunctionCallAndComputation();
static void TestUintstring();
static void TestSumRecursiveCondition();



// * print symbols from isa and sram
void print_register(core_t *cr);
void print_stack(core_t *cr);

// * test
void TestParsingOperand();


int main() {
    // TestAddFunctionCallAndComputation();
    // TestUintstring();
    // TestParsingOperand();
    TestSumRecursiveCondition();

    return 0;
}



static void TestUintstring() {
    const char*nums[12] = {
        "0",
        "-0",
        "0x0",
        "1234",
        "0x1234",
        "0xabcd",
        "-0xabcd",
        "-1234",
        "2147483647",
        "-2147483648",
        "0x8000000000000000",
        "0xffffffffffffffff",
    };

    for (int i = 0; i < 12; ++ i) {
        printf("%s => %lx\n", nums[i], string2uint(nums[i]));
    }
}
 


static void TestAddFunctionCallAndComputation() {
    // * 当前为一个内核
    ACTIVE_CORE = 0x0;

    core_t *ac = (core_t*)&cores[ACTIVE_CORE];


    // Todo init
    ac->reg.rax = 0xabcd;
    ac->reg.rbx = 0x8000670;
    ac->reg.rcx = 0x8000670;
    ac->reg.rdx = 0x12340000;
    ac->reg.rsi = 0x7ffffffee108;
    ac->reg.rdi = 0x1;
    ac->reg.rbp = 0x7ffffffee110;
    ac->reg.rsp = 0x7ffffffee0f0;

    ac->flags.CF = 0;
    ac->flags.ZF = 0;
    ac->flags.SF = 0;
    ac->flags.OF = 0;
    
    write64bits_dram(va2pa(0x7ffffffee110, ac), 0x0000000000000000, ac);    // * rbp
    write64bits_dram(va2pa(0x7ffffffee108, ac), 0x0000000000000000, ac);
    write64bits_dram(va2pa(0x7ffffffee100, ac), 0x0000000012340000, ac);
    write64bits_dram(va2pa(0x7ffffffee0f8, ac), 0x000000000000abcd, ac);
    write64bits_dram(va2pa(0x7ffffffee0f0, ac), 0x0000000000000000, ac);    // * rsp


    char assembly[15][MAX_INSTRUCTION_CHAR] = {
        "push   %rbp",              // 0
        "mov    %rsp,%rbp",         // 1
        "mov    %rdi,-0x18(%rbp)",  // 2
        "mov    %rsi,-0x20(%rbp)",  // 3
        "mov    -0x18(%rbp),%rdx",  // 4
        "mov    -0x20(%rbp),%rax",  // 5
        "add    %rdx,%rax",         // 6
        "mov    %rax,-0x8(%rbp)",   // 7
        "mov    -0x8(%rbp),%rax",   // 8
        "pop    %rbp",              // 9
        "retq",                     // 10
        "mov    %rdx,%rsi",         // 11
        "mov    %rax,%rdi",         // 12
        "callq  0",                 // 13
        "mov    %rax,-0x8(%rbp)",   // 14
    };

    ac->rip = (uint64_t)&assembly[11];

    // * 录入第一条指令运行时地址
    sprintf(assembly[13], "callq $%p", &assembly[0]);

    printf("\n");

    // * run
    int time = 0;
    while (time < 15) {
        instruction_cycle(ac);
        print_register(ac);
        print_stack(ac);
        time++;
    }



    // ? verify
      int match = 1;
    match = match && ac->reg.rax == 0x1234abcd;
    match = match && ac->reg.rbx == 0x8000670;
    match = match && ac->reg.rcx == 0x8000670;
    match = match && ac->reg.rdx == 0xabcd;
    match = match && ac->reg.rsi == 0x12340000;
    match = match && ac->reg.rdi == 0xabcd;
    match = match && ac->reg.rbp == 0x7ffffffee110;
    match = match && ac->reg.rsp == 0x7ffffffee0f0;
    
    if (match)
        printf("register match\n");
    else
        printf("register mismatch\n");

    match = match && (read64bits_dram(va2pa(0x7ffffffee110, ac), ac) == 0x0000000000000000); // rbp
    match = match && (read64bits_dram(va2pa(0x7ffffffee108, ac), ac) == 0x000000001234abcd);
    match = match && (read64bits_dram(va2pa(0x7ffffffee100, ac), ac) == 0x0000000012340000);
    match = match && (read64bits_dram(va2pa(0x7ffffffee0f8, ac), ac) == 0x000000000000abcd);
    match = match && (read64bits_dram(va2pa(0x7ffffffee0f0, ac), ac) == 0x0000000000000000); // rsp
    if (match)
        printf("memory match\n");
    else
        printf("memory mismatch\n");
        
}


static void TestSumRecursiveCondition()
{
    ACTIVE_CORE = 0x0;
    core_t *cr = (core_t *)&cores[ACTIVE_CORE];

    // init state
    cr->reg.rax = 0x8000630;
    cr->reg.rbx = 0x0;
    cr->reg.rcx = 0x8000650;
    cr->reg.rdx = 0x7ffffffee328;
    cr->reg.rsi = 0x7ffffffee318;
    cr->reg.rdi = 0x1;
    cr->reg.rbp = 0x7ffffffee230;
    cr->reg.rsp = 0x7ffffffee220;

    cr->flags.__cpu_flag_value = 0;

    write64bits_dram(va2pa(0x7ffffffee230, cr), 0x0000000008000650, cr);    // rbp
    write64bits_dram(va2pa(0x7ffffffee228, cr), 0x0000000000000000, cr);
    write64bits_dram(va2pa(0x7ffffffee220, cr), 0x00007ffffffee310, cr);    // rsp

    char assembly[19][MAX_INSTRUCTION_CHAR] = {
        "push   %rbp",              // 0
        "mov    %rsp,%rbp",         // 1
        "sub    $0x10,%rsp",        // 2
        "mov    %rdi,-0x8(%rbp)",   // 3
        "cmpq   $0x0,-0x8(%rbp)",   // 4
        "jne    0x400200",          // 5: jump to 8
        "mov    $0x0,%eax",         // 6
        "jmp    0x400380",          // 7: jump to 14
        "mov    -0x8(%rbp),%rax",   // 8
        "sub    $0x1,%rax",         // 9
        "mov    %rax,%rdi",         // 10
        "callq  0x00400000",        // 11
        "mov    -0x8(%rbp),%rdx",   // 12
        "add    %rdx,%rax",         // 13
        "leaveq ",                  // 14
        "retq   ",                  // 15
        "mov    $0x3,%edi",         // 16
        "callq  0x00400000",        // 17
        "mov    %rax,-0x8(%rbp)",   // 18
    };

    // copy to physical memory
    for (int i = 0; i < 19; ++ i) {
        // * 插入代码块 0x400000的偏移量        
        // * 正常情况不应该允许写内存，代码块为只读操作 only-read
        writeinst_dram(va2pa(i * 0x40 + 0x00400000, cr), assembly[i], cr);
    }
    cr->rip = MAX_INSTRUCTION_CHAR * sizeof(char) * 16 + 0x00400000;

    printf("begin\n");
    int time = 0;
    while ((cr->rip <= 18 * 0x40 + 0x00400000) &&
           time < MAX_INSTRUCTION_CHAR) {
        instruction_cycle(cr);
        print_register(cr);
        print_stack(cr);
        time++;
    } 

    printf("location\n");
    // gdb state ret from func
    int match = 1;
    match = match && cr->reg.rax == 0x6;
    match = match && cr->reg.rbx == 0x0;
    match = match && cr->reg.rcx == 0x8000650;
    match = match && cr->reg.rdx == 0x3;
    match = match && cr->reg.rsi == 0x7ffffffee318;
    match = match && cr->reg.rdi == 0x0;
    match = match && cr->reg.rbp == 0x7ffffffee230;
    match = match && cr->reg.rsp == 0x7ffffffee220;
    
    if (match)
    {
        printf("register match\n");
    }
    else
    {
        printf("register mismatch\n");
    }

    match = match && (read64bits_dram(va2pa(0x7ffffffee230, cr), cr) == 0x0000000008000650); // rbp
    match = match && (read64bits_dram(va2pa(0x7ffffffee228, cr), cr) == 0x0000000000000006);
    match = match && (read64bits_dram(va2pa(0x7ffffffee220, cr), cr) == 0x00007ffffffee310); // rsp

    if (match)
    {
        printf("memory match\n");
    }
    else
    {
        printf("memory mismatch\n");
    }
}