#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<headers/cpu.h>
#include<headers/memory.h>
#include<headers/common.h>



/*======================================*/
/*        instruction architecture      */
/*======================================*/

// * op_type
typedef enum INST_OPERATOR {
    INST_MOV,
    INST_PUSH,
    INST_POP,
    INST_LEAVE,
    INST_CALL,
    INST_RET,
    INST_ADD,
    INST_SUB,
    INST_CMP,
    INST_JNE,
    INST_JMP,
}op_t;

// * inst_type
typedef enum OPERAND_TYPE {
    EMPTY,
    IMM,
    REG,
    MEM_IMM,
    MEM_REG1,
    MEM_IMM_REG1,
    MEM_REG1_REG2,
    MEM_IMM_REG1_REG2,
    MEM_REG2_SCAL,
    MEM_IMM_REG2_SCAL,
    MEM_REG1_REG2_SCAL,
    MEM_IMM_REG1_REG2_SCAL,
}od_type_t;

// * inst_one
typedef struct OPERAND_STRUCT {
    od_type_t type;

    uint64_t imm;
    uint64_t scal;
    uint64_t reg1;
    uint64_t reg2;
}od_t;


// * complete_inst
typedef struct INST_STRUCT {
    op_t op_t;  // * e.g mov, call, etc

    od_t src;
    od_t dst;
}isnt_t;



/* ================================= */
/*          parse asm inst           */
/* ================================= */

// * 函数将汇编代码映射为 inst实例
static void parse_instruction(const char *str, isnt_t *inst, core_t *cr);
static void parse_operand(const char *str, od_t *od, core_t *cr);
static uint64_t decode_operand(od_t *od);


// * parse operand
static uint64_t decode_operand(od_t *od) {
    if (od->type == IMM)
        return *(uint64_t *)&od->imm;   // imm maybe is a negative
    else if (od->type == REG)
        return od->reg1;    // default reg1
    else if (od->type == EMPTY)
        return 0x0;
    else {
        // access memory: reutrn the physical address
        uint64_t vaddr = 0;

        if (od->type == MEM_IMM)
            vaddr = od->imm;
        else if (od->type == MEM_REG1)
            vaddr = (*(uint64_t *)od->reg1);
        else if (od->type == MEM_IMM_REG1)
            vaddr = (*(uint64_t *)od->reg1) + od->imm;
        else if (od->type == MEM_REG1_REG2)
            vaddr = (*(uint64_t *)od->reg1) + (*(uint64_t *)od->reg2);
        else if (od->type == MEM_IMM_REG1_REG2)
            vaddr = (*(uint64_t *)od->reg1) + (*(uint64_t *)od->reg2) + od->imm;
        else if (od->type == MEM_REG2_SCAL)
            vaddr = (*(uint64_t *)od->reg2) * od->scal;
        else if (od->type == MEM_IMM_REG2_SCAL)
            vaddr = od->imm + (*(uint64_t *)od->reg2) * od->scal;
        else if (od->type == MEM_REG1_REG2_SCAL)
            vaddr = (*(uint64_t *)od->reg1) + (*(uint64_t *)od->reg2) * od->scal;
        else if (od->type == MEM_IMM_REG1_REG2_SCAL)
            vaddr = od->imm + (*(uint64_t *)od->reg1) + (*(uint64_t *)od->reg2) * od->scal;

        return vaddr;
    }

    // * empty
    return 0x0;  
}

static void parse_instruction(const char *str, isnt_t *inst, core_t *cr) {

}

static void parse_operand(const char *str, od_t *od, core_t *cr) {

}

/* ================================= */
/*         instruction handlers      */
/* ================================= */

// * handler instruction function 

/// @brief callback handler
/// @param src_od 
/// @param dst_od 
/// @param cr 
static void mov_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void push_handler            (od_t *src_od, od_t *dst_od, core_t *cr);
static void pop_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void leave_handler           (od_t *src_od, od_t *dst_od, core_t *cr);
static void call_handler            (od_t *src_od, od_t *dst_od, core_t *cr);
static void ret_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void add_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void sub_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void cmp_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void jne_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void jmp_handler             (od_t *src_od, od_t *dst_od, core_t *cr);


// * function pointer -> callback funciton
typedef void (*handler_t)(od_t *, od_t *, core_t *); 


static handler_t handler_table[NUM_INSTRTYPE] = {
    &mov_handler,               // 0
    &push_handler,              // 1
    &pop_handler,               // 2
    &leave_handler,             // 3
    &call_handler,              // 4
    &ret_handler,               // 5
    &add_handler,               // 6
    &sub_handler,               // 7
    &cmp_handler,               // 8
    &jne_handler,               // 9
    &jmp_handler,               // 10
};



// reset the condition flags
static inline void reset_cflags(core_t* cr) {
    cr->CF = 0;
    cr->ZF = 0;
    cr->SF = 0;
    cr->OF = 0;
}

static inline void next_rip(core_t *cr) {
    // * array length max_instruction_char
    // * 也可以处理变长数组
    cr->rip = cr->rip + sizeof(char) * MAX_INSTRUCTION_CHAR;
}


// instruction handlers

static void mov_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    // * MOV reg, reg
    if (src_od->type == REG && dst_od->type == REG) {
        *(uint64_t *)dst = *(uint64_t *)src;

        next_rip(cr);
        reset_cflags(cr);
        return;    
    }
}

static void push_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
    
}

static void pop_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void leave_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void call_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void ret_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void add_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void sub_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void cmp_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void jne_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void jmp_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}
