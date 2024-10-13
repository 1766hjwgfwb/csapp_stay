#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include<stdint.h>
#include<stdlib.h>
#include"cpu/register.h"



#define NUM_INSTRTYPE 30


// todo 操作数类型
typedef enum OP {
    /* MOV,    // 0
    PUSH,   // 1
    CALL    // 2
    */  
    mov_reg_reg,
    mov_reg_mem,
    mov_mem_reg,
    push_reg,
     pop_reg,
    call,
    ret,
    add_reg_reg
}op_t;  


// todo 指令类型
typedef enum OP_TYPE {
    EMPTY,
    IMM,REG,        // ? 立即数 -> 寄存器
    MM_IMM,MM_REG,MM_IMM_REG,MM_REG1_REG2,MM_IMM_REG1_REG2,MM_REG1_S,MM_IMM_REG1_S,MM_REG1_REG2_S,MM_IMM_REG1_REG2_S
}od_type_t;


// todo 指令信息
typedef struct OD {
    od_type_t type; // * 定义指令
    
    int64_t imm;    // * 立即数存在负数(-Imm)
    int64_t scal;   // * 比例因子
    uint64_t *reg1; // * 指针存储寄存器地址
    uint64_t *reg2;

}od_t;


// todo 一条完整指令
typedef struct INSTRUCT_STRUCT {
    op_t op;    // * mov or push ...
    od_t stc;   // * 源操作数
    od_t dst;   // * 目的操作数

    char code[100]; // * 存储指令信息
}inst_t;

#define INST_LEN 100


inst_t program[INST_LEN];   // * 宏定义指令集长度 100条



// ************ mothod *************


// 类似c++的抽象方法 定义一个函数指针
typedef void (*handler_t)(uint64_t, uint64_t);


// 函数指针数组 存放各个"方法"
handler_t handler_table[NUM_INSTRTYPE];

void init_handler_table();

void mov_reg_reg_handler(uint64_t src, uint64_t dst);

void add_reg_reg_handler(uint64_t src, uint64_t dst);



 
static uint64_t decode_od(od_t od);// * 译码方法 

void instruction_cycle();  // * 指令解析周期




#endif