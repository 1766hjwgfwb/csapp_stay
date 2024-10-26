#include"memory/myinstruction.h"
#include"cpu/mmu.h"
#include"cpu/register.h"
#include"memory/dram.h"


#include<stdio.h>


reg_t reg;
handler_t handler_table[NUM_INSTRTYPE];


static uint64_t decode_od(od_t od) {
    if (od.type == IMM)
    {
        // * 有符号数转无符号数
        // * 0x >> 32 ！！
        // * Imm存在32位负数情况  -64bit 转 +32bit  
        return *((uint64_t* )&od.imm);
    }
    else if (od.type == REG) 
    {
        return (uint64_t)od.reg1;
    }
    else
    {
        // * mm
        uint64_t addr = 0;

        if (od.type == MM_IMM) {
            // * addr = od.imm;
            addr = od.imm;
        }else if (od.type == MM_REG) {
            /* // return *(uint64_t *)od.reg1; // * 寄存器地址解引用寻址
            return (uint64_t)od.reg1;       // * 返回地址
            */
            // * store reg
            addr = *(od.reg1);
        } else if (od.type == MM_IMM_REG) {
            // * error wire addr = *(od.reg1 + od.imm);
            addr = *(od.reg1) + od.imm;
        } else if (od.type == MM_REG1_REG2) 
        {
            addr = *(od.reg1) + *(od.reg2);
        } else if (od.type == MM_IMM_REG1_REG2) 
        {
            // * error wirte od.reg2 = *(od.reg1) + od.imm
            addr = *(od.reg1) + *(od.reg2) + od.imm;
        } else if (od.type == MM_REG1_S) 
        {
            addr = (*(od.reg1)) * od.scal;
        } else if (od.type == MM_IMM_REG1_S) 
        {
            addr = ((*(od.reg1)) * od.scal) + od.imm;
        } else if (od.type == MM_REG1_REG2_S) 
        {
            addr = *(od.reg1) + ((*od.reg2) * od.scal);
        } else if (od.type == MM_IMM_REG1_REG2_S) 
        {
            addr = *(od.reg1) + od.imm + ((*(od.reg2)) * od.scal);
        }

        return va2pa(addr);
    }
}

// * 指令处理周期函数
void instruction_cycle() {
    // * 将当前 rip 指向的地址赋值于 inst_t结构体指针
    // * rip 指针指向下一条指令地址 rip = rip + (当前指令字节大小)
    inst_t* instr = (inst_t*)reg.rip;

    // * reg: value   mm: paddr 
    uint64_t src = decode_od(instr->src);
    uint64_t dst = decode_od(instr->dst);

    // add rax rbx
    // op = add_reg_reg = 3
    // handler_table[add_reg_reg] == handler_table[3] == add_reg_reg_handler

    handler_t handler = handler_table[instr->op];
    

    // * add_reg_reg_handler(src = &rax, dst = &rbx)
    handler(src, dst);

    
    printf("%s\n", instr->code); 
}

// * 类似抽象类的派生   多态
void init_handler_table() {
    // * 注册函数  回调？ 信号处理？
    handler_table[mov_reg_reg] = &mov_reg_reg_handler;
    handler_table[call] = &call_handler;
    handler_table[add_reg_reg] = &add_reg_reg_handler;
}

void mov_reg_reg_handler(uint64_t src, uint64_t dst) {
    // * src: reg
    // * dst: reg

    *(uint64_t* )dst = *(uint64_t* )src;
    // * 下一条 rip指令的地址 = 当前 rip指针 + 结构体大小 
    reg.rip = reg.rip + sizeof(inst_t);
}

void call_handler(uint64_t src, uint64_t dst) {
    // * rsp 
    reg.rsp = reg.rsp - 8;
    

    // * rsp 指针内存放下一条指令的地址 return value
    wirtebits_dram(va2pa(reg.rsp), reg.rip + sizeof(inst_t));

    reg.rip = src;
}

void add_reg_reg_handler(uint64_t src, uint64_t dst) {
    // * dst = src + dst
    *(uint64_t *)dst = *(uint64_t *)dst + *(uint64_t *)src;

    reg.rip = reg.rip + sizeof(inst_t);
}
