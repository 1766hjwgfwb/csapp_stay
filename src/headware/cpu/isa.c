/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "headers/cpu.h"
#include "headers/instruction.h"
#include "headers/memory.h"
#include "headers/common.h"
#include "headers/algorithm.h"


// * extern
cpu_reg_t cpu_reg;
cpu_flags_t cpu_flags;
cpu_pc_t cpu_pc;






/* ================================= */
/*          parse asm inst           */
/* ================================= */

// * 函数将汇编代码映射为 inst实例
static void parse_instruction(const char *str, isnt_t *inst);
static void parse_operand(const char *str, od_t *od);

static uint64_t decode_operand(od_t *od);

// convert trie
// static uint64_t reflact_register(const char *str); 

// * trie mothods
static void lazy_init_trie();
static uint64_t try_get_from_trie(trie_node_t **root, char *key);
static void trie_cleanup(); 

// * trie reconfig
static trie_node_t *register_mapping = NULL;
static trie_node_t *operator_mapping = NULL;
static bool trie_inited = false;




/// @brief parse operand
/// @param od 
/// @return od type
static uint64_t decode_operand(od_t *od) {
    if (od->type == IMM)
        return *(uint64_t *)&od->imm;   // imm maybe is a negative
    else if (od->type == REG)
        return od->reg1;    // default reg1     note: od.reg1 is a &reg, need *(ui64_t *)
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


// * lookup table
/*static const char *reg_name_list[72] = {
    "%rax","%eax","%ax","%ah","%al",
    "%rbx","%ebx","%bx","%bh","%bl",
    "%rcx","%ecx","%cx","%ch","%cl",
    "%rdx","%edx","%dx","%dh","%dl",
    "%rsi","%esi","%si","%sih","%sil",
    "%rdi","%edi","%di","%dih","%dil",
    "%rbp","%ebp","%bp","%bph","%bpl",
    "%rsp","%esp","%sp","%sph","%spl",
    "%r8","%r8d","%r8w","%r8b",
    "%r9","%r9d","%r9w","%r9b",
    "%r10","%r10d","%r10w","%r10b",
    "%r11","%r11d","%r11w","%r11b",
    "%r12","%r12d","%r12w","%r12b",
    "%r13","%r13d","%r13w","%r13b",
    "%r14","%r14d","%r14w","%r14b",
    "%r15","%r15d","%r15w","%r15b",
};*/


static void trie_cleanup() {
    trie_free(register_mapping);
    trie_free(operator_mapping);
}


// * @brief return &reg, so reg1/reg2 = &reg.rax/xxx 
/* static uint64_t reflact_register(const char *reg_name) {

    // * lookup table
    uint64_t reg_addr[72] = {
         (uint64_t)&(cpu_reg.rax),(uint64_t)&(cpu_reg.eax),(uint64_t)&(cpu_reg.ax),(uint64_t)&(cpu_reg.ah),(uint64_t)&(cpu_reg.al),
        (uint64_t)&(cpu_reg.rbx),(uint64_t)&(cpu_reg.ebx),(uint64_t)&(cpu_reg.bx),(uint64_t)&(cpu_reg.bh),(uint64_t)&(cpu_reg.bl),
        (uint64_t)&(cpu_reg.rcx),(uint64_t)&(cpu_reg.ecx),(uint64_t)&(cpu_reg.cx),(uint64_t)&(cpu_reg.ch),(uint64_t)&(cpu_reg.cl),
        (uint64_t)&(cpu_reg.rdx),(uint64_t)&(cpu_reg.edx),(uint64_t)&(cpu_reg.dx),(uint64_t)&(cpu_reg.dh),(uint64_t)&(cpu_reg.dl),
        (uint64_t)&(cpu_reg.rsi),(uint64_t)&(cpu_reg.esi),(uint64_t)&(cpu_reg.si),(uint64_t)&(cpu_reg.sih),(uint64_t)&(cpu_reg.sil),
        (uint64_t)&(cpu_reg.rdi),(uint64_t)&(cpu_reg.edi),(uint64_t)&(cpu_reg.di),(uint64_t)&(cpu_reg.dih),(uint64_t)&(cpu_reg.dil),
        (uint64_t)&(cpu_reg.rbp),(uint64_t)&(cpu_reg.ebp),(uint64_t)&(cpu_reg.bp),(uint64_t)&(cpu_reg.bph),(uint64_t)&(cpu_reg.bpl),
        (uint64_t)&(cpu_reg.rsp),(uint64_t)&(cpu_reg.esp),(uint64_t)&(cpu_reg.sp),(uint64_t)&(cpu_reg.sph),(uint64_t)&(cpu_reg.spl),
        (uint64_t)&(cpu_reg.r8),(uint64_t)&(cpu_reg.r8d),(uint64_t)&(cpu_reg.r8w),(uint64_t)&(cpu_reg.r8b),
        (uint64_t)&(cpu_reg.r9),(uint64_t)&(cpu_reg.r9d),(uint64_t)&(cpu_reg.r9w),(uint64_t)&(cpu_reg.r9b),
        (uint64_t)&(cpu_reg.r10),(uint64_t)&(cpu_reg.r10d),(uint64_t)&(cpu_reg.r10w),(uint64_t)&(cpu_reg.r10b),
        (uint64_t)&(cpu_reg.r11),(uint64_t)&(cpu_reg.r11d),(uint64_t)&(cpu_reg.r11w),(uint64_t)&(cpu_reg.r11b),
        (uint64_t)&(cpu_reg.r12),(uint64_t)&(cpu_reg.r12d),(uint64_t)&(cpu_reg.r12w),(uint64_t)&(cpu_reg.r12b),
        (uint64_t)&(cpu_reg.r13),(uint64_t)&(cpu_reg.r13d),(uint64_t)&(cpu_reg.r13w),(uint64_t)&(cpu_reg.r13b),
        (uint64_t)&(cpu_reg.r14),(uint64_t)&(cpu_reg.r14d),(uint64_t)&(cpu_reg.r14w),(uint64_t)&(cpu_reg.r14b),
        (uint64_t)&(cpu_reg.r15),(uint64_t)&(cpu_reg.r15d),(uint64_t)&(cpu_reg.r15w),(uint64_t)&(cpu_reg.r15b),
    };

    // ? 暴力匹配地址
    for (int i = 0; i < 72; i++)
        if (strcmp(reg_name, reg_name_list[i]) == 0)
            return reg_addr[i];

    // @bug
    // printf("reg match error");
    debug_printf(DEBUG_PARSEINST, "parse operand %s\ncannot parse register\n", reg_name);
    exit(0);
} */

// use trie to match register name
static void lazy_init_trie() {
    if (trie_inited)
        return;

    // init trie
    // "%rax" -> &cpu_reg.rax
    trie_insert(&register_mapping, "%rax", (uint64_t)&(cpu_reg.rax));
    trie_insert(&register_mapping, "%eax", (uint64_t)&(cpu_reg.eax));
    trie_insert(&register_mapping, "%ax", (uint64_t)&(cpu_reg.ax));
    trie_insert(&register_mapping, "%ah", (uint64_t)&(cpu_reg.ah));
    trie_insert(&register_mapping, "%al", (uint64_t)&(cpu_reg.al));
    trie_insert(&register_mapping, "%rbx", (uint64_t)&(cpu_reg.rbx));
    trie_insert(&register_mapping, "%ebx", (uint64_t)&(cpu_reg.ebx));
    trie_insert(&register_mapping, "%bx", (uint64_t)&(cpu_reg.bx));
    trie_insert(&register_mapping, "%bh", (uint64_t)&(cpu_reg.bh));
    trie_insert(&register_mapping, "%bl", (uint64_t)&(cpu_reg.bl));
    trie_insert(&register_mapping, "%rcx", (uint64_t)&(cpu_reg.rcx));
    trie_insert(&register_mapping, "%ecx", (uint64_t)&(cpu_reg.ecx));
    trie_insert(&register_mapping, "%cx", (uint64_t)&(cpu_reg.cx));
    trie_insert(&register_mapping, "%ch", (uint64_t)&(cpu_reg.ch));
    trie_insert(&register_mapping, "%cl", (uint64_t)&(cpu_reg.cl));
    trie_insert(&register_mapping, "%rdx", (uint64_t)&(cpu_reg.rdx));
    trie_insert(&register_mapping, "%edx", (uint64_t)&(cpu_reg.edx));
    trie_insert(&register_mapping, "%dx", (uint64_t)&(cpu_reg.dx));
    trie_insert(&register_mapping, "%dh", (uint64_t)&(cpu_reg.dh));
    trie_insert(&register_mapping, "%dl", (uint64_t)&(cpu_reg.dl));
    trie_insert(&register_mapping, "%rsi", (uint64_t)&(cpu_reg.rsi));
    trie_insert(&register_mapping, "%esi", (uint64_t)&(cpu_reg.esi));
    trie_insert(&register_mapping, "%si", (uint64_t)&(cpu_reg.si));
    trie_insert(&register_mapping, "%sih", (uint64_t)&(cpu_reg.sih));
    trie_insert(&register_mapping, "%sil", (uint64_t)&(cpu_reg.sil));
    trie_insert(&register_mapping, "%rdi", (uint64_t)&(cpu_reg.rdi));
    trie_insert(&register_mapping, "%edi", (uint64_t)&(cpu_reg.edi));
    trie_insert(&register_mapping, "%di", (uint64_t)&(cpu_reg.di));
    trie_insert(&register_mapping, "%dih", (uint64_t)&(cpu_reg.dih));
    trie_insert(&register_mapping, "%dil", (uint64_t)&(cpu_reg.dil));
    trie_insert(&register_mapping, "%rbp", (uint64_t)&(cpu_reg.rbp));
    trie_insert(&register_mapping, "%ebp", (uint64_t)&(cpu_reg.ebp));
    trie_insert(&register_mapping, "%bp", (uint64_t)&(cpu_reg.bp));
    trie_insert(&register_mapping, "%bph", (uint64_t)&(cpu_reg.bph));
    trie_insert(&register_mapping, "%bpl", (uint64_t)&(cpu_reg.bpl));
    trie_insert(&register_mapping, "%rsp", (uint64_t)&(cpu_reg.rsp));
    trie_insert(&register_mapping, "%esp", (uint64_t)&(cpu_reg.esp));
    trie_insert(&register_mapping, "%sp", (uint64_t)&(cpu_reg.sp));
    trie_insert(&register_mapping, "%sph", (uint64_t)&(cpu_reg.sph));
    trie_insert(&register_mapping, "%spl", (uint64_t)&(cpu_reg.spl));
    trie_insert(&register_mapping, "%r8", (uint64_t)&(cpu_reg.r8));
    trie_insert(&register_mapping, "%r8d", (uint64_t)&(cpu_reg.r8d));
    trie_insert(&register_mapping, "%r8w", (uint64_t)&(cpu_reg.r8w));
    trie_insert(&register_mapping, "%r8b", (uint64_t)&(cpu_reg.r8b));
    trie_insert(&register_mapping, "%r9", (uint64_t)&(cpu_reg.r9));
    trie_insert(&register_mapping, "%r9d", (uint64_t)&(cpu_reg.r9d));
    trie_insert(&register_mapping, "%r9w", (uint64_t)&(cpu_reg.r9w));
    trie_insert(&register_mapping, "%r9b", (uint64_t)&(cpu_reg.r9b));
    trie_insert(&register_mapping, "%r10", (uint64_t)&(cpu_reg.r10));
    trie_insert(&register_mapping, "%r10d", (uint64_t)&(cpu_reg.r10d));
    trie_insert(&register_mapping, "%r10w", (uint64_t)&(cpu_reg.r10w));
    trie_insert(&register_mapping, "%r10b", (uint64_t)&(cpu_reg.r10b));
    trie_insert(&register_mapping, "%r11", (uint64_t)&(cpu_reg.r11));
    trie_insert(&register_mapping, "%r11d", (uint64_t)&(cpu_reg.r11d));
    trie_insert(&register_mapping, "%r11w", (uint64_t)&(cpu_reg.r11w));
    trie_insert(&register_mapping, "%r11b", (uint64_t)&(cpu_reg.r11b));
    trie_insert(&register_mapping, "%r12", (uint64_t)&(cpu_reg.r12));
    trie_insert(&register_mapping, "%r12d", (uint64_t)&(cpu_reg.r12d));
    trie_insert(&register_mapping, "%r12w", (uint64_t)&(cpu_reg.r12w));
    trie_insert(&register_mapping, "%r12b", (uint64_t)&(cpu_reg.r12b));
    trie_insert(&register_mapping, "%r13", (uint64_t)&(cpu_reg.r13));
    trie_insert(&register_mapping, "%r13d", (uint64_t)&(cpu_reg.r13d));
    trie_insert(&register_mapping, "%r13w", (uint64_t)&(cpu_reg.r13w));
    trie_insert(&register_mapping, "%r13b", (uint64_t)&(cpu_reg.r13b));
    trie_insert(&register_mapping, "%r14", (uint64_t)&(cpu_reg.r14));
    trie_insert(&register_mapping, "%r14d", (uint64_t)&(cpu_reg.r14d));
    trie_insert(&register_mapping, "%r14w", (uint64_t)&(cpu_reg.r14w));
    trie_insert(&register_mapping, "%r14b", (uint64_t)&(cpu_reg.r14b));
    trie_insert(&register_mapping, "%r15", (uint64_t)&(cpu_reg.r15));
    trie_insert(&register_mapping, "%r15d", (uint64_t)&(cpu_reg.r15d));
    trie_insert(&register_mapping, "%r15w", (uint64_t)&(cpu_reg.r15w));
    trie_insert(&register_mapping, "%r15b", (uint64_t)&(cpu_reg.r15b));

    // init trie
    // "mov" -> INST_MOV
    trie_insert(&operator_mapping, "mov", INST_MOV);
    trie_insert(&operator_mapping, "movq", INST_MOV);
    trie_insert(&operator_mapping, "push", INST_PUSH);
    trie_insert(&operator_mapping, "pop", INST_POP);
    trie_insert(&operator_mapping, "leaveq", INST_LEAVE);
    trie_insert(&operator_mapping, "callq", INST_CALL);
    trie_insert(&operator_mapping, "retq", INST_RET);
    trie_insert(&operator_mapping, "add", INST_ADD);
    trie_insert(&operator_mapping, "sub", INST_SUB);
    trie_insert(&operator_mapping, "cmpq", INST_CMP);
    trie_insert(&operator_mapping, "jne", INST_JNE);
    trie_insert(&operator_mapping, "jmp", INST_JMP);


    // trie_print(register_mapping);
    // trie_print(operator_mapping);

    trie_inited = true;
    // add the cleanup function to events
    add_cleanup_event(&trie_cleanup);
}


static uint64_t try_get_from_trie(trie_node_t **root, char *key) {
    // * lazy init trie
    if (*root == NULL) {
        lazy_init_trie();
    }

    uint64_t val;

    // * match key -> val
    int ret = trie_get(*root, key, &val);


    // not found
    if (ret == 0) {
        printf("trie_get error, key not found\n");
        exit(0);
    }

    return val;
}



/// @brief parse instruction 可构建抽象语法树
/// @param str e.g. "mov %rax, %rbx"
/// @param inst callback need args
static void parse_instruction(const char *str, isnt_t *inst) {
    
    // * 状态机解析
    char op_str[64] = {'\0'};
    int op_len = 0;
    char src_str[64] = {'\0'};
    int src_len = 0;
    char dst_str[64] = {'\0'};
    int dst_len = 0;

    char c;
    int count_parent = 0;   // * ()
    int state = 0;
    
    for (int i = 0; i <strlen(str); i++) {
        // * 逐字符解析
        c = str[i];
        if (c == '(' || c == ')') {
            count_parent++;
        }
        
        if (state == 0 && c != ' ') {
            state = 1;
        } else if (state == 1 && c == ' ') {
            state = 2;
            continue;
        } else if (state == 2 && c != ' ') {
            state = 3;
        } else if (state == 3 && c == ',' && (count_parent == 0 || count_parent == 2)) {
            state = 4;
            continue;
        } else if (state == 4 && c != ' ' && c != ',') {
            state = 5;
        } else if (state == 5 && c == ' ') {
            state = 6;
            continue;
        }


        if (state == 1) {
            op_str[op_len] = c;
            op_len ++;
            continue;
        } else if (state == 3) {
            src_str[src_len] = c;
            src_len ++;
            continue;
        } else if (state == 5) {
            dst_str[dst_len] = c;
            dst_len ++;
            continue;
        }

    }

    // op_str, src_str, dst_str
    // strlen(str)
    // * 已实现: 用字典树来构建
    parse_operand(src_str, &(inst->src));
    parse_operand(dst_str, &(inst->dst));

    // * 未考虑实际汇编代码操作数 e.g. lea and xmm reg (punpcklqdq, movdqa)
    // * movq 'q'为操作数 8字节大小指示
    // * movdqa 'dqa'指示 xmm 浮点数寄存器
    /*if (strcmp(op_str, "mov") == 0 || strcmp(op_str, "movq") == 0) {
        inst->op_t = INST_MOV;
    } else if (strcmp(op_str, "push") == 0) {
        inst->op_t = INST_PUSH;
    } else if (strcmp(op_str, "pop") == 0) {
        inst->op_t = INST_POP;
    } else if (strcmp(op_str, "leaveq") == 0) {
        inst->op_t = INST_LEAVE;
    } else if (strcmp(op_str, "callq") == 0) {
        inst->op_t = INST_CALL;
    } else if (strcmp(op_str, "retq") == 0) {
        inst->op_t = INST_RET;
    } else if (strcmp(op_str, "add") == 0) {
        inst->op_t = INST_ADD;
    } else if (strcmp(op_str, "sub") == 0) {
        inst->op_t = INST_SUB;
    } else if (strcmp(op_str, "cmpq") == 0) {
        inst->op_t = INST_CMP;
    } else if (strcmp(op_str, "jne") == 0) {
        inst->op_t = INST_JNE;
    } else if (strcmp(op_str, "jmp") == 0) {
        inst->op_t = INST_JMP;
    }*/

    // search op_t in trie
    inst->op_t = (op_t)try_get_from_trie(&operator_mapping, op_str);


    debug_printf(DEBUG_PARSEINST, "[%s (%d)] [%s (%d)] [%s (%d)]\n", op_str, inst->op_t, src_str, inst->src.type, dst_str, inst->dst.type);
}

// * @brief operand 
// * 函数解析内存访问 11种格式
// * 应当注意类似 mov %rsp, %rbp 会被拆分为 od, od 
static void parse_operand(const char *str, od_t *od) {
    // * str: assembly code string, e.g. %rsp

    od->type = EMPTY;
    od->imm = 0;
    od->scal = 0;
    od->reg1 = 0;
    od->reg2 = 0; 

    int str_len = strlen(str);
    if (str_len == 0)
        return;     // * empty operand string

    if (str[0] == '$') {
        // * imm
        od->type = IMM;
        od->imm = string2uint_range(str, 1, -1);
        return;     // ? return
    } else if (str[0] == '%') {
        // * reg
        od->type = REG;
        // * match reg name
        // od->reg1 = reflact_register(str);

        // * math reg name in trie
        od->reg1 = try_get_from_trie(&register_mapping, (char*)str);
        return;
    } else {
        // *  memory access
        // * set char buffer
        char imm[64] = {'\0'};
        int imm_len = 0;
        char reg1[8] = {'\0'};
        int reg1_len = 0;
        char reg2[8] = {'\0'};
        int reg2_len = 0;
        char scal[2] = {'\0'};
        int scal_len = 0;

        int count_parent = 0;     // ()
        int count_comma = 0;     // comma ,

        for (int i = 0; i < str_len; i++) {
            char c = str[i];

            if ( c == '(' || c == ')') {
                count_parent++;   // '()'
            }
            else if (c == ',') {
                count_comma++;   // ','
            } else {
                // * parse imm(reg1,reg2,scal)
                if (count_parent == 0) {
                    // * 把字符存放 len++  xxx
                    imm[imm_len] = c;
                    imm_len++;
                } else if (count_parent == 1) {
                    // * maybe ???(, xxx)
                    if (count_comma == 0) {
                        // * ???(xxx)
                        // * (xxxx)
                        reg1[reg1_len] = c;
                        reg1_len++;
                    } else if (count_comma == 1) {
                        // ???(,xxx)
                        // (???, xxx)
                        // (, xxx)
                        // ???(, xxx)
                        reg2[reg2_len] = c; 
                        reg2_len++;
                    } else if (count_comma == 2) {
                        // * ???(???, ???, scal)
                        scal[scal_len] = c;
                        scal_len++;
                    }
                }
            }
        }

        // * Imm
        if (imm_len > 0) {
            od->imm = string2uint(imm);

            if (count_parent == 0) {
                // * no ()      e.g. "0xxxx" 
                od->type = MEM_IMM;
                return;
            }
        }

        // * Scal
        if (scal_len > 0) {
            od->scal = string2uint(scal);
            if (od->scal != 0x1 && od->scal != 0x2 && od->scal != 0x4 && od->scal != 0x8) {
                // printf("scal error\n");
                debug_printf(DEBUG_PARSEINST, "parse operand is -> %s\nscal is 1/2/4/8\n", str);
                exit(0);
            }
        }

        // * reg1
        if (reg1_len > 0) {
            // od->reg1 = reflact_register(reg1);

            od->reg1 = try_get_from_trie(&register_mapping, (char*)reg1);
        }

        // * reg2
        if (reg2_len > 0) {
            // od->reg2 = reflact_register(reg2);

            od->reg2 = try_get_from_trie(&register_mapping, (char*)reg2);
        }


        // * 判断 ','
        if (count_comma == 0) {
            if (imm_len > 0) {
                od->type = MEM_IMM_REG1;
                return;
            } else {
                od->type = MEM_REG1;
                return;
            }
        } else if (count_comma == 1) {
            if (imm_len > 0) {
                od->type = MEM_IMM_REG1_REG2;
                return;
            } else {
                od->type = MEM_REG1_REG2;
                return;
            }
        } else if (count_comma == 2) {
            if (reg1_len > 0) {
                // * reg1存在
                if (imm_len > 0) {
                    od->type = MEM_IMM_REG1_REG2_SCAL;
                    return;
                } else {
                    od->type = MEM_REG1_REG2_SCAL;
                    return;
                }
            } else {
            // no reg1
            if (imm_len > 0) {
                    od->type = MEM_IMM_REG2_SCAL;
                    return;
            } else {
                    od->type = MEM_REG2_SCAL;
                    return;
                }
            } 
        }
    }
}

/* ================================= */
/*         instruction handlers      */
/* ================================= */

// * handler instruction function 

/// @brief callback handler
/// @param src_od 
/// @param dst_od 
/// @param cr 
static void mov_handler             (od_t *src_od, od_t *dst_od);
static void push_handler            (od_t *src_od, od_t *dst_od);
static void pop_handler             (od_t *src_od, od_t *dst_od);
static void leave_handler           (od_t *src_od, od_t *dst_od);
static void call_handler            (od_t *src_od, od_t *dst_od);
static void ret_handler             (od_t *src_od, od_t *dst_od);
static void add_handler             (od_t *src_od, od_t *dst_od);
static void sub_handler             (od_t *src_od, od_t *dst_od);
static void cmp_handler             (od_t *src_od, od_t *dst_od);
static void jne_handler             (od_t *src_od, od_t *dst_od);
static void jmp_handler             (od_t *src_od, od_t *dst_od);


// * function pointer -> callback funciton
typedef void (*handler_t)(od_t *, od_t *); 


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
// static inline void reset_cflags(core_t* cr) {
//     cr->CF = 0;
//     cr->ZF = 0;
//     cr->SF = 0;
//     cr->OF = 0;
// }

static inline void next_rip() {
    // * array length max_instruction_char
    // * 也可以处理变长数组
    cpu_pc.rip = cpu_pc.rip + sizeof(char) * MAX_INSTRUCTION_CHAR;
}


// * instruction handlers
// * 将寄存器和内存统一为地址和数据的组合，通过解引用实现对数据的读写操作
static void mov_handler(od_t *src_od, od_t *dst_od) {
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    // * MOV reg, reg
    if (src_od->type == REG && dst_od->type == REG) {
        *(uint64_t *)dst = *(uint64_t *)src;

        next_rip();
        // reset_cflags(cr);
        cpu_flags.flag_value = 0;
        return;    
    } else if (src_od->type == REG && dst_od->type == MEM_IMM_REG1) {
        // * mov %rsi, -0x20(%rbp)
        write64bits_dram(va2pa(dst), *(uint64_t *)src);
        next_rip();
        // reset_cflags(cr);
        cpu_flags.flag_value = 0;
        return;
    } else if (src_od->type == MEM_IMM_REG1 && dst_od->type == REG) {
        // * mov -0x20(%rbp), %rsi
        *(uint64_t *)dst = read64bits_dram(va2pa(src));
        next_rip();
        // reset_cflags(cr);
        cpu_flags.flag_value = 0;
        return;
    } else if (src_od->type == IMM && dst_od->type == REG) {
        // * mov 0x20, %rbp
        // todo note: dst = src
        // * 当类型为 REG 时返回的是`&cr->reg.xxx`,而不是值,在 `decode` 函数中查看

        *(uint64_t *)dst = src;
        next_rip();
        // reset_cflags(cr);
        cpu_flags.flag_value = 0;
        return;
    }
}

static void push_handler(od_t *src_od, od_t *dst_od) {
    uint64_t src = decode_operand(src_od);
    // * don't need dst

    // * e.g push %rbp
    if (src_od->type == REG) {
        cpu_reg.rsp = cpu_reg.rsp - 8;
        write64bits_dram(va2pa(cpu_reg.rsp), *(uint64_t *)src);

        next_rip();
        // reset_cflags(cr);
        cpu_flags.flag_value = 0;
        return;
    }
}

static void pop_handler(od_t *src_od, od_t *dst_od) {
    uint64_t src = decode_operand(src_od);

    // * pop %rbx
    if (src_od->type == REG) {
        uint64_t val = read64bits_dram(va2pa(cpu_reg.rsp));
        // * pop push 约定 8字节对齐
        // * 如果函数分配了额外的栈空间,类似 `sub $0x10, rsp`,在 `pop`前要自己手动释放 `add $0x10, rsp`
        cpu_reg.rsp = cpu_reg.rsp + 8;

        *(uint64_t *)src = val;
        next_rip();
        // reset_cflags(cr);
        cpu_flags.flag_value = 0;
        return;
    }
}

// * @brief
// * leave == mov rbp,rsp + pop rbp
static void leave_handler(od_t *src_od, od_t *dst_od) {
    // uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);

    // * leave == mov rbp, rsp  pop rbp 结束当前函数调用 -> 退栈
    cpu_reg.rsp = cpu_reg.rbp;  // 恢复栈顶
    
    cpu_reg.rbp = read64bits_dram(va2pa(cpu_reg.rsp));     // 恢复 rbp保存的值

    cpu_reg.rsp = cpu_reg.rsp + 8;

    next_rip();

    cpu_flags.flag_value = 0;
}

static void call_handler(od_t *src_od, od_t *dst_od) {
    uint64_t src = decode_operand(src_od);


    // * call 0xxxx/function addres
    cpu_reg.rsp = cpu_reg.rsp - 8;

    write64bits_dram(va2pa(cpu_reg.rsp), cpu_pc.rip + sizeof(char) * MAX_INSTRUCTION_CHAR);

    cpu_pc.rip = src;
    // todo 
    // elf重定位引用时会相对偏移量 这里需要修改
    // refaddr = ADDR(s) + r.offset
    // cpu.rip = cpu.rip + (ADDR(r.symbol) + r.addend - refaddr)

    // reset_cflags(cr);
    cpu_flags.flag_value = 0;
}

static void ret_handler(od_t *src_od, od_t *dst_od) {
    uint64_t ret_addr = read64bits_dram(va2pa(cpu_reg.rsp));

    cpu_reg.rsp = cpu_reg.rsp + 8;

    cpu_pc.rip = ret_addr;
    // reset_cflags(cr);
    cpu_flags.flag_value = 0;
}

static void add_handler(od_t *src_od, od_t *dst_od) {
    // * add src, dst
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG && dst_od->type == REG) {
        uint64_t val = *(uint64_t *)src + *(uint64_t *)dst;

        // ?
        *(uint64_t *)dst = val;


        // * set cpu_flags


        // 取运算数符号位
        uint8_t src_sign = (*(uint64_t *)src >> 63) & 0x1;
        uint8_t dst_sign = (*(uint64_t *)dst >> 63) & 0x1;
        uint8_t val_sign = (val >> 63) & 0x1;


        // * a + b < a || a < a - 1
        cpu_flags.CF = (val < *(uint64_t *)src);    // * 无符号溢出
        // * val == 0
        cpu_flags.ZF = (val == 0x0);
        // * neg
        cpu_flags.SF = val_sign;                    // * 判断最高位符号

        // *  ' 卡诺图判断化简 '  a == b && c = 0 / 1  
        //    // src_sign = 0 && dst_sign = 0 && val_sign = 1
        //    // src_sign = 1 && dst_sign = 1 && val_sign = 0
        cpu_flags.OF = (src_sign == dst_sign) ^ val_sign;

        next_rip();
        return;
    }
}

static void sub_handler(od_t *src_od, od_t *dst_od) {
    // * sub 0x28, reg
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);


    if (src_od->type == IMM && dst_od->type == REG) {
        uint64_t val = *(uint64_t *)dst + (~src + 1);
        // * 5 = 0000000000000101, 3 = 0000000000000011, -3 = 1111111111111101, 5 + (-3) = 0000000000000010
        // ?
        // // next_rip(cr);
        // // cr->flags.__cpu_flag_value = 0;

        // * set cpu_flags

        // copy paste
        // 取运算数符号位
        uint8_t src_sign = (src >> 63) & 0x1;
        uint8_t dst_sign = (*(uint64_t *)dst >> 63) & 0x1;
        uint8_t val_sign = (val >> 63) & 0x1;


        // * a + b < a || a < a - 1
        cpu_flags.CF = (val > *(uint64_t *)dst);    // * 无符号溢出
        // * val == 0
        cpu_flags.ZF = (val == 0x0);
        // * neg
        cpu_flags.SF = val_sign;                    // * 判断最高位符号

        // *  ' 卡诺图判断化简 '  a == b && c = 0 / 1  
        //    // src_sign = 0 && dst_sign = 0 && val_sign = 1
        //    // src_sign = 1 && dst_sign = 1 && val_sign = 0
        cpu_flags.OF = ((src_sign == dst_sign) ^ val_sign);

        *(uint64_t *)dst = val;
        next_rip();
        return;
    }
}

static void cmp_handler(od_t *src_od, od_t *dst_od) {
    // * e.g. cmpq   $0x0,-0x8(%rbp)    dst - src
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    if (src_od->type == IMM && dst_od->type == MEM_IMM_REG1) {
        // * if dst - src = 0 set ZF = 0
        
        uint64_t dval = read64bits_dram(va2pa(dst));
        uint64_t val = dval + (~src + 1);

        uint8_t src_sign = (src >> 63) & 0x1;
        uint8_t dst_sign = (dval >> 63) & 0x1;
        uint8_t val_sign = (val >> 63) & 0x1;

        // * unsigned
        cpu_flags.CF = (val > dval);  
        cpu_flags.ZF = (val == 0); 

        // * intsigned
        cpu_flags.SF = val_sign; // neg 
        cpu_flags.OF = ((src_sign == dst_sign) ^ val_sign);


        next_rip();
        return;
    }
}

static void jne_handler(od_t *src_od, od_t *dst_od) {
    // * jne: `zf == 0` jump
    uint64_t src = decode_operand(src_od);

    if (src_od->type == MEM_IMM) {
        if (cpu_flags.ZF == 0) {
            // * note: next_rip 加载了下一条指令的偏移量 并不是直接跳转
            // ! ××× cr->rip = src - MAX_INSTRUCTION_CHAR; ×××

            cpu_pc.rip = src;
        } else {
            next_rip();
        }
    }

    // * 执行跳转指令后清零标志位
    cpu_flags.flag_value = 0; // set zero flags
}

static void jmp_handler(od_t *src_od, od_t *dst_od) {
    // * 直接跳转
    uint64_t src = decode_operand(src_od);

    if (src_od->type == MEM_IMM) {

        cpu_pc.rip = src;
        
        cpu_flags.flag_value = 0;
        return;
    }
}


// * @brief
// * 指令周期在cpu中实现
// * 外部接口
void instruction_cycle() {

    /*
    // * get and print rip addres
        const char *inst_str = (const char *)cr->rip;
        // 将代码块添加到内存后 该行为未定义
    */
    // todo Fetch
    // * get the instruction string by program counter
    char inst_str[MAX_INSTRUCTION_CHAR + 10];
    // * 从' 内存 '中读取代码块的位置
    // * update pc
    readinst_dram(va2pa(cpu_pc.rip), inst_str);

    debug_printf(DEBUG_INSTRUCTIONCYCLE, "%lx   %s\n", cpu_pc.rip, inst_str);

    // printf("isnt_srt:    %c\n", *(const char *)(cr->rip));
    // printf("%p\n", &cr->rip);

    // todo Decode
    // * 形参 isnt 带出信息
    isnt_t inst;
    parse_instruction(inst_str, &inst);


    // todo Execute
    // * callback
    handler_t handler = handler_table[inst.op_t];

    handler(&inst.src, &inst.dst);
}

void print_register() {
    if ((DEBUG_VERBOSE_SET & DEBUG_REGISTERS) == 0x0)
        return;

    // cpu_reg_t reg = cr->reg;

    printf("rax = 0x%lx\trbx = 0x%lx\trcx = 0x%lx\trdx = 0x%lx\n", cpu_reg.rax, cpu_reg.rbx, cpu_reg.rcx, cpu_reg.rdx);
    printf("rsi = 0x%lx\trdi = 0x%lx\trbp = 0x%lx\trsp = 0x%lx\n", cpu_reg.rsi, cpu_reg.rdi, cpu_reg.rbp, cpu_reg.rsp);
    printf("rip = 0x%lx\n", cpu_pc.rip);
    printf("CF = %u\tZF = %u\tSF = %u\tOF = %u\n",
        cpu_flags.CF, cpu_flags.ZF, cpu_flags.SF, cpu_flags.OF);
    printf("\n");
}

void print_stack(){
    if ((DEBUG_VERBOSE_SET & DEBUG_PRINTSTACK) == 0x0)
        return;

    int n = 10;
    uint64_t *high = (uint64_t*)&pm[va2pa(cpu_reg.rsp)];

    high = &high[n];    // 向上增长 （10 * 8）bits 0x50
    uint64_t va = cpu_reg.rsp + n * 8;

    for (int i = 0; i < 2 * n; i++) {
        uint64_t *prt = (uint64_t *)(high - i);
        printf("0x%16lx : %16lx", va, (uint64_t)*prt);

        if (i == n)
            printf(" <=== rsp");

        printf("\n");
        va -=8;
    }
}

void TestParsingOperand() {
    // ACTIVE_CORE = 0x0;

    // 确认操作内核
    // core_t *cr = (core_t *)&cores[ACTIVE_CORE];

    const char *strs[11] = {
        "$0x1234",
        "%rcx",
        "0xabcd",
        "(%rsp)",
        "0xabcd(%rsp)",
        "(%rsp,%rbx)",
        "0xabcd(%rsp,%rbx)",
        "(,%rbx,8)",
        "0xabcd(,%rbx,8)",
        "(%rsp,%rbx,8)",
        "0xabcd(%rsp,%rbx,8)",
    };

    printf("rcx: %p\n", &cpu_reg.rcx);
    printf("rbx: %p\n", &cpu_reg.rbx);
    printf("rsp: %p\n", &cpu_reg.rsp);

    for (int i = 0; i < 11; i++) {
        od_t od;
        parse_operand(strs[i], &od);

        printf("\n");
        printf("%s\n", strs[i]);
        printf("od type: %d\n", od.type);
        printf("od imm:  %lx\n", od.imm);
        printf("od reg1: %lx\n", od.reg1);
        printf("od.reg2: %lx\n", od.reg2);
        printf("od.scal: %lx\n", od.scal);
        printf("\n");
    }
}