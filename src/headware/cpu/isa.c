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

    uint64_t  imm;  // ? int64_t
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
static uint64_t reflact_register(const char *str, core_t *cr); 


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


// * lookup table
static const char *reg_name_list[72] = {
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
};

static uint64_t reflact_register(const char *str, core_t *cr) {

    // * lookup table
    reg_t *reg = &(cr->reg);
    uint64_t reg_addr[72] = {
        (uint64_t)&(reg->rax),(uint64_t)&(reg->eax),(uint64_t)&(reg->ax),(uint64_t)&(reg->ah),(uint64_t)&(reg->al),
        (uint64_t)&(reg->rbx),(uint64_t)&(reg->ebx),(uint64_t)&(reg->bx),(uint64_t)&(reg->bh),(uint64_t)&(reg->bl),
        (uint64_t)&(reg->rcx),(uint64_t)&(reg->ecx),(uint64_t)&(reg->cx),(uint64_t)&(reg->ch),(uint64_t)&(reg->cl),
        (uint64_t)&(reg->rdx),(uint64_t)&(reg->edx),(uint64_t)&(reg->dx),(uint64_t)&(reg->dh),(uint64_t)&(reg->dl),
        (uint64_t)&(reg->rsi),(uint64_t)&(reg->esi),(uint64_t)&(reg->si),(uint64_t)&(reg->sih),(uint64_t)&(reg->sil),
        (uint64_t)&(reg->rdi),(uint64_t)&(reg->edi),(uint64_t)&(reg->di),(uint64_t)&(reg->dih),(uint64_t)&(reg->dil),
        (uint64_t)&(reg->rbp),(uint64_t)&(reg->ebp),(uint64_t)&(reg->bp),(uint64_t)&(reg->bph),(uint64_t)&(reg->bpl),
        (uint64_t)&(reg->rsp),(uint64_t)&(reg->esp),(uint64_t)&(reg->sp),(uint64_t)&(reg->sph),(uint64_t)&(reg->spl),
        (uint64_t)&(reg->r8),(uint64_t)&(reg->r8d),(uint64_t)&(reg->r8w),(uint64_t)&(reg->r8b),
        (uint64_t)&(reg->r9),(uint64_t)&(reg->r9d),(uint64_t)&(reg->r9w),(uint64_t)&(reg->r9b),
        (uint64_t)&(reg->r10),(uint64_t)&(reg->r10d),(uint64_t)&(reg->r10w),(uint64_t)&(reg->r10b),
        (uint64_t)&(reg->r11),(uint64_t)&(reg->r11d),(uint64_t)&(reg->r11w),(uint64_t)&(reg->r11b),
        (uint64_t)&(reg->r12),(uint64_t)&(reg->r12d),(uint64_t)&(reg->r12w),(uint64_t)&(reg->r12b),
        (uint64_t)&(reg->r13),(uint64_t)&(reg->r13d),(uint64_t)&(reg->r13w),(uint64_t)&(reg->r13b),
        (uint64_t)&(reg->r14),(uint64_t)&(reg->r14d),(uint64_t)&(reg->r14w),(uint64_t)&(reg->r14b),
        (uint64_t)&(reg->r15),(uint64_t)&(reg->r15d),(uint64_t)&(reg->r15w),(uint64_t)&(reg->r15b),
    };

    // ? 暴力匹配地址
    for (int i = 0; i < 72; i++)
        if (strcmp(str, reg_name_list[i]) == 0)
            return reg_addr[i];

    // @bug
    printf("reg match error");
    exit(0);
}

static void parse_instruction(const char *str, isnt_t *inst, core_t *cr) {

}

static void parse_operand(const char *str, od_t *od, core_t *cr) {
    // * str: assembly code string, e.g. mov %rsp, %rbp

    od->type = EMPTY;
    od->imm = 0;
    od->scal = 0;
    od->reg1 = 0;
    od->reg2 = 0;

    int str_len = strlen(str);
    if (str_len == '0')
        return;     // * empty operand string

    if (str[0] == '$') {
        // * imm
        od->type = IMM;
        od->imm = string2uint_range(str, 1, -1);
        return;
    } else if (str[0] == '%') {
        // * reg
        od->type = REG;
        od->reg1 = reflact_register(str, cr);
        return;
    } else {
        // *  memory access
        char imm[64] = {'\0'};
        int imm_len = 0;
        char reg1[64] = {'\0'};
        int reg1_len = 0;
        char reg2[64] = {'\0'};
        int reg2_len = 0;
        char scal[64] = {'\0'};
        int scal_len = 0;

        int ca = 0;     // ()
        int cb = 0;     // comma ,

        for (int i = 0; i < str_len; i++) {
            char c = str[i];
            if ( c == '(' || c == ')') {
                ca++;   // '()'
                continue;
            }
            else if (c == ',') {
                cb++;   // ','
                continue;
            } else {
                // * parse imm(reg1,reg2,scal)
                if (ca == 0) {
                    // * 把字符存放 len++  xxx
                    imm[imm_len] = c;
                    imm_len++;
                    continue;
                } else if (ca == 1) {
                    // * maybe ???(, xxx)
                    if (cb == 0) {
                        // * ???(xxx)
                        // * (xxxx)
                        reg1[reg1_len] = c;
                        reg1_len++;
                        continue;
                    } else if (cb == 1) {
                        // ???(,xxx)
                        // (???, xxx)
                        // (, xxx)
                        // ???(, xxx)
                        reg2[reg2_len] = c; 
                        reg2_len++;
                        continue;
                    } else if (cb == 2) {
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

            if (ca == 0) {
                // no ()
                od->type = MEM_IMM;
                return;
            }
        }

        // * Scal
        if (scal_len > 0) {
            od->scal = string2uint(scal);
            if ((od->scal & 0xF) == 0) {
                printf("scal error\n");
                exit(0);
            }
        }

        // * reg1
        if (reg1_len > 0) {
            od->reg1 = reflact_register(reg1, cr);
        }

        // * reg2
        if (reg2_len > 0) {
            od->reg2 = reflact_register(reg2, cr);
        }


        // 
        if (cb == 0) {
            if (imm_len > 0) {
                od->type = MEM_IMM_REG1;
                return;
            } else {
                od->type = MEM_REG1;
                return;
            }
        } else if (cb == 1) {
            if (imm_len > 0) {
                od->type = MEM_IMM_REG1_REG2;
                return;
            } else {
                od->type = MEM_REG1_REG2;
                return;
            }
        } else if (cb == 2) {
            if (imm_len > 0) {
                od->type = MEM_IMM_REG1_REG2_SCAL;
                return;
            } else {
                od->type = MEM_REG1_REG2_SCAL;
                return;
            }
        } else {
            // no reg
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
// static inline void reset_cflags(core_t* cr) {
//     cr->CF = 0;
//     cr->ZF = 0;
//     cr->SF = 0;
//     cr->OF = 0;
// }

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
        // reset_cflags(cr);
        cr->flags.__cpu_flag_value = 0;
        return;    
    } else if (src_od->type == REG && dst_od->type == MEM_IMM) {
        // * mov %rsi, -0x20(%rbp)
        wirte64bits_dram(va2pa(dst, cr), *(uint64_t *)src, cr);
        next_rip(cr);
        // reset_cflags(cr);
        cr->flags.__cpu_flag_value = 0;
        return;
    } else if (src_od->type == MEM_IMM && dst_od->type == REG) {
        // * mov -0x20(%rbp), %rsi
        *(uint64_t *)dst = read64bits_dram(va2pa(src, cr), cr);
        next_rip(cr);
        // reset_cflags(cr);
        cr->flags.__cpu_flag_value = 0;
        return;
    } else if (src_od->type == IMM && dst_od->type == REG) {
        // * mov 0x20, %rbp
        *(uint64_t *)dst = src;
        next_rip(cr);
        // reset_cflags(cr);
        cr->flags.__cpu_flag_value = 0;
        return;
    }
}

static void push_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
    uint64_t src = decode_operand(src_od);
    // * don't need dst

    // * e.g push %rbp
    if (src_od->type == REG) {
        cr->reg.rsp = cr->reg.rsp - 8;
        wirte64bits_dram(va2pa(cr->reg.rsp, cr), *(uint64_t *)src, cr);

        next_rip(cr);
        // reset_cflags(cr);
        cr->flags.__cpu_flag_value = 0;
        return;
    }
}

static void pop_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
    uint64_t src = decode_operand(src_od);

    // * pop %rbx
    if (src_od->type == REG) {
        uint64_t val = read64bits_dram(va2pa(src, cr), cr);
        cr->reg.rsp = cr->reg.rsp + 8;

        *(uint64_t *)src = val;
        next_rip(cr);
        // reset_cflags(cr);
        cr->flags.__cpu_flag_value = 0;
        return;
    }
}

static void leave_handler(od_t *src_od, od_t *dst_od, core_t *cr) {

}

static void call_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
    uint64_t src = decode_operand(src_od);


    // * call 0xxxx/function addres
    cr->reg.rsp = cr->reg.rsp - 8;

    wirte64bits_dram(va2pa(cr->reg.rsp, cr), cr->rip + sizeof(char) * MAX_INSTRUCTION_CHAR, cr);

    cr->rip = src;
    // reset_cflags(cr);
    cr->flags.__cpu_flag_value = 0;
}

static void ret_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
    uint64_t ret_addr = read64bits_dram(va2pa(cr->reg.rsp, cr), cr);

    cr->reg.rsp = cr->reg.rsp + 8;

    cr->rip = ret_addr;
    // reset_cflags(cr);
    cr->flags.__cpu_flag_value = 0;
}

static void add_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
    // * add src, dst
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG && dst_od->type == REG) {
        uint64_t val = *(uint64_t *)src + *(uint64_t *)dst;

        // ?
        *(uint64_t *)dst = val;
        // next_rip(cr);
        cr->flags.__cpu_flag_value = 0;
        return;
    }
}

static void sub_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void cmp_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void jne_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}

static void jmp_handler(od_t *src_od, od_t *dst_od, core_t *cr) {
}


// * @brief
// * 指令周期在cpu中实现
// * 外部接口
void instruction_cycle(core_t *cr) {

    // * get and print rip addres
    const char *inst_str = (const char *)cr->rip;
    debug_printf(DEBUG_INSTRUCTIONCYCLE, "%lx   %s\n", cr->rip, inst_str);

    // * 形参 isnt 带出信息
    isnt_t inst;
    parse_instruction(inst_str, &inst, cr);


    // * callback
    handler_t handler = handler_table[inst.op_t];

    handler(&inst.src, &inst.dst, cr);
}

void print_register(core_t *cr) {
    if ((DEBUG_VERBOSE_SET & DEBUG_REGISTERS) == 0x0)
        return;

    reg_t reg = cr->reg;

    printf("rax = 0x%lx\trbx = 0x%lx\trcx = 0x%lx\trdx = 0x%lx\n", reg.rax, reg.rbx, reg.rcx, reg.rdx);
    printf("rsi = 0x%lx\trdi = 0x%lx\trbp = 0x%lx\trsp = 0x%lx\n", reg.rsi, reg.rdi, reg.rbp, reg.rsp);
    printf("rip = 0x%lx\n", cr->rip);
    printf("CF = %u\tZF = %u\tSF = %u\tOF = %u\n",
        cr->flags.CF, cr->flags.ZF, cr->flags.SF, cr->flags.OF);
    printf("\n");
}

void print_stack(core_t *cr){
    if ((DEBUG_VERBOSE_SET & DEBUG_PRINTSTACK) == 0x0)
        return;

    int n = 10;
    uint64_t *high = (uint64_t*)&pm[va2pa(cr->reg.rsp, cr)];

    high = &high[n];    // 向上增长 （10 * 8）bits 0x50
    uint64_t va = cr->reg.rsp + n * 8;

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
    ACTIVE_CORE = 0x0;

    // 确认操作内核
    core_t *cr = (core_t *)&cores[ACTIVE_CORE];

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

    printf("rcx: %p\n", &cr->reg.rcx);
    printf("rbx: %p\n", &cr->reg.rbx);
    printf("rsp: %p\n", &cr->reg.rsp);

    for (int i = 0; i < 11; i++) {
        od_t od;
        parse_operand(strs[i], &od, cr);

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