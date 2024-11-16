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



// * print symbols from isa and sram
void print_register(core_t *cr);
void print_stack(core_t *cr);

// * test
void TestParsingOperand();


int main() {
    // TestAddFunctionCallAndComputation();
    // TestUintstring();
    TestParsingOperand();

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
    ac->reg.rsi = 0x7ffffffee208;
    ac->reg.rdi = 0x1;
    ac->reg.rbp = 0x7ffffffee110;
    ac->reg.rsp = 0x7ffffffee0f0;

    ac->flags.CF = 0;
    ac->flags.ZF = 0;
    ac->flags.SF = 0;
    ac->flags.OF = 0;
    
    wirte64bits_dram(va2pa(0x7ffffffee110, ac), 0x0000000000000000, ac);    // * rbp
    wirte64bits_dram(va2pa(0x7ffffffee108, ac), 0x0000000000000000, ac);
    wirte64bits_dram(va2pa(0x7ffffffee100, ac), 0x0000000012340000, ac);
    wirte64bits_dram(va2pa(0x7ffffffee0f8, ac), 0x000000000000abcd, ac);
    wirte64bits_dram(va2pa(0x7ffffffee0f0, ac), 0x0000000000000000, ac);    // * rsp


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

    // * 打印第一条指令运行时地址
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