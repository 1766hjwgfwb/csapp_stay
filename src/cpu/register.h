#ifndef CPU_H
#define CPU_H




#include<stdint.h>
#include<stdlib.h>




// Todo cpu register
// ? This is a model of cpu reg 


// * 优化cpu结构
typedef struct REG_STRUCT {
    union {

        struct {
            uint8_t al;
            uint8_t ah;
        };
        
        uint16_t ax;
        uint32_t eax;
        uint64_t rax;
    };

    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;

    uint64_t rip;       
}reg_t;

extern reg_t reg;



/*=======================*/
// !     cpu core
/*====================== */












#endif