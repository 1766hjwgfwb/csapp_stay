#include<stdlib.h>


#include"cpu/register.h"
#include"memory/myinstruction.h"


reg_t reg;

inst_t program[15] = {
    // * 指令执行周期
    {   
        push_reg,
        {REG, 0, 0, (uint64_t* )&reg.rbp, NULL},
        {EMPTY, 0, 0, NULL, NULL},
        "push   %rbp"   // * store 上一个函数栈帧
    },
    {
        mov_reg_reg,
        {REG, 0, 0, (uint64_t* )&reg.rsp, NULL},
        {REG, 0, 0, (uint64_t* )&reg.rbp, NULL},
        "mov    %rsp, %rbp"     // * 将上一个函数的栈顶 设置为当前执行函数的栈底  rsp = rbp rsp = rsp - 0x10(0x8)
    },
    {
        mov_reg_mem,
        {REG, 0, 0, (uint64_t* )&reg.rdi, NULL},
        {MM_IMM_REG, -0x18, 0, (uint64_t* )&reg.rbp, NULL},
        "mov    %rdi, -0x18(%rbp)"
    },
    {
        mov_reg_mem,
        {REG, 0, 0, (uint64_t* )&reg.rsi, NULL},
        {MM_IMM_REG, -0x20, 0, (uint64_t* )&reg.rbp, NULL},
        "mov    %rsi, -0x20(%rbp)"
    },
    {
        mov_mem_reg,
        {MM_IMM_REG, -0x18, 0, (uint64_t* )&reg.rbp, NULL},
        {REG, 0, 0, (uint64_t* )&reg.rdx, NULL},
        "mov    -0x18(%rbp), %rdx"
    },
    {
        mov_mem_reg,
        {MM_IMM_REG, -0x20, 0, (uint64_t* )&reg.rbp, NULL},
        {REG, 0, 0, (uint64_t* )&reg.rax, NULL},
        "mov    -0x20(%rbp),%rax"
    },
    {
        add_reg_reg,
        {REG, 0, 0, (uint64_t* )&reg.rdx, NULL},
        {REG, 0, 0, (uint64_t* )&reg.rax, NULL},
        "add    %rdx,%rax"
    },
    {
        mov_reg_mem,
        {REG, 0, 0, (uint64_t* )&reg.rax, NULL},
        {MM_IMM_REG, -0x8, 0, (uint64_t* )&reg.rbp, NULL},
        "mov    %rax,-0x8(%rbp)"
    },
    {
        mov_mem_reg,
        {MM_IMM_REG, -0x8, 0, (uint64_t* )&reg.rbp, NULL},
        {REG, 0, 0, (uint64_t* )&reg.rax, NULL},
        "mov    -0x8(%rbp),%rax"
    },
    {
        pop_reg,
        {REG, 0, 0, (uint64_t* )&reg.rbp, NULL},
        {EMPTY, 0, 0, NULL, NULL},
        "pop    %rbp"   // * pop指令将
    },
    {
        ret,
        {EMPTY, 0, 0, NULL, NULL},
        {EMPTY, 0, 0, NULL, NULL},
        "retq"      // * 和callq指令搭配 返回控制权 rsp + 8 并访问 rsp内的 rip地址
    },
    {
        mov_reg_reg,
        {REG, 0, 0, (uint64_t* )&reg.rdx, NULL},
        {REG, 0, 0, (uint64_t* )&reg.rsi, NULL},
        "mov    %rdx,%rsi"
    },
    {
        mov_reg_reg,
        {REG, 0, 0, (uint64_t* )&reg.rax, NULL},
        {REG, 0, 0, (uint64_t* )&reg.rdi, NULL},
        "mov    %rax,%rdi"
    },
    {
        call,   // * 指令跳转地址 head
        // {EMPTY, 0, 0, (uint64_t* )(&program[0]), NULL},
        {IMM, (uint64_t)(&program[0]), 0, NULL, NULL},  // ? decode mothod -> call addrs value
        {EMPTY, 0, 0, NULL, NULL},
        "callq  5fa <add>"
    },
    {
        mov_reg_mem,
        {REG, 0, 0, (uint64_t* )&reg.rax, NULL},
        {MM_IMM_REG, -0x8, 0, (uint64_t* )&reg.rbp, NULL},
        "mov    %rax,-0x8(%rbp)"
    }
};