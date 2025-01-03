/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// * double declaration of any identifiers
// * such as types, enums and static var
#ifndef CPU_GUARD
#define CPU_GUARD




#include <stdint.h>
#include <stdlib.h>


/* ============================== */
/*            registers           */
/* ============================== */




// Todo cpu register
// ? This is a model of cpu reg 
// * each core

typedef struct REGISTER_STRUCT {

    // * return value
    union {
        uint64_t rax;
        uint32_t eax;
        uint16_t ax;

        struct {
            uint8_t al;
            uint8_t ah;
        };
    };

    // * callee saved
    union {
        uint64_t rbx;
        uint32_t ebx;
        uint16_t bx;

        struct {
            uint8_t bl;
            uint8_t bh;
        };
    };

    // * 4th arg
    union {
        uint64_t rcx;
        uint32_t ecx;
        uint16_t cx;

        struct {
            uint8_t cl;
            uint8_t ch;
        };  
    };

    // * 3th arg
    union {
        uint64_t rdx;
        uint32_t edx;
        uint16_t dx;

        struct {
            uint8_t dl;
            uint8_t dh;
        };
    };

    // * 2th arg
    union {
        uint64_t rsi;
        uint32_t esi;
        uint16_t si;

        struct {
            uint8_t sil;
            uint8_t sih;
        };
    };

    // * 1th arg
    union {
        uint64_t rdi;
        uint32_t edi;
        uint16_t di;

        struct {
            uint8_t dil;
            uint8_t dih;
        };
    };

    // * callee saved frame pointer
    union {
        uint64_t rbp;
        uint32_t ebp;
        uint16_t bp;

        struct {
            uint8_t bpl;
            uint8_t bph;
        };
    };

    // * stack pointer
    union {
        uint64_t rsp;
        uint32_t esp;
        uint16_t sp;

        struct {
            uint8_t spl;
            uint8_t sph;
        };
    };

    // * 5th arg
    union {
        uint64_t r8;
        uint32_t r8d;
        uint16_t r8w;
        uint8_t  r8b;
    };

    // * 6th arg
    union {
        uint64_t r9;
        uint32_t r9d;
        uint16_t r9w;
        uint8_t  r9b;
    };

    // * caller saved
    union {
        uint64_t r10;
        uint32_t r10d;
        uint16_t r10w;
        uint8_t  r10b;
    };

    // * caller saved
    union {
        uint64_t r11;
        uint32_t r11d;
        uint16_t r11w;
        uint8_t  r11b;
    };

    // * caller saved
    union {
        uint64_t r12;
        uint32_t r12d;
        uint16_t r12w;
        uint8_t  r12b;
    };

    // * caller saved
    union {
        uint64_t r13;
        uint32_t r13d;
        uint16_t r13w;
        uint8_t  r13b;
    };

    // * caller saved
    union {
        uint64_t r14;
        uint32_t r14d;
        uint16_t r14w;
        uint8_t  r14b;
    };

    // * caller saved
    union {
        uint64_t r15;
        uint32_t r15d;
        uint16_t r15w;
        uint8_t  r15b;
    };    
}cpu_reg_t;

extern cpu_reg_t cpu_reg;

// * extern reg_t reg;      由内核封装 core_struct   




    /* integer arithmetic instructions
        inc     increment 1
        dec     decrement 1
        neg     negate
        not     complement
        ----------------------------
        add     add
        sub     subtract
        imul    multiply
        xor     exclusive or
        or      or
        and     and
        ----------------------------
        sal     left shift
        shl     left shift (same as sal)
        sar     arithmetic right shift
        shr     logical right shift
    */
    
    /* comparison and test instructions
        cmp     compare
        test    test
    */

// * 4 flags is a uint64_t
typedef struct CPU_FLAGS_STRUCT {
    union {
        uint64_t flag_value;

        // * 4*16 = 64 bits
        struct {
            // uint_t overflow
            uint16_t CF;
            // expression zero value                
            uint16_t ZF;
            // neg
            uint16_t SF;
            // int_t overflow 
            uint16_t OF;
        };
    };
}cpu_flags_t;

extern cpu_flags_t cpu_flags;

/*=======================*/
// !     cpu core (only one core) 
/*====================== */

/* typedef struct CORE_STRUCT {

    // * program counter(pc) or instruction pointer
    union {
        uint64_t rip;
        uint32_t eip;
    };

    cpu_flag_t flags;

    // * reg files
    reg_t reg;

    // uint64_t pdbr;
}core_t;

*/
typedef union PROGRAM_COUNTER_UNION {
    uint64_t rip;

    uint64_t eip;
}cpu_pc_t;

extern cpu_pc_t cpu_pc;


// * define cpu core array
// #define NUM_CORES            1

// extern core_t cores[NUM_CORES];    // 内核数组

// extern uint64_t ACTIVE_CORE;


#define MAX_INSTRUCTION_CHAR (64)
#define NUM_INSTRTYPE        (14)


// TODO CPU exe cycle
void instruction_cycle();


// * mmu function
uint64_t va2pa(uint64_t vaddr);

#endif