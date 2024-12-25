/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_


#include<stdint.h>


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






#endif /* _INSTRUCTION_H_ */