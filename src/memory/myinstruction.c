#include"memory/myinstruction.h"
#include"cpu/mmu.h"



int main() {
}


static uint64_t decode_od(od_t od) {
    if (od.type == IMM)
    {
        return od.imm;
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
            return *((uint64_t *)&od.imm);
        }else if (od.type == MM_REG) {
            /* // return *(uint64_t *)od.reg1; // * 寄存器地址解引用寻址
            return (uint64_t)od.reg1;       // * 返回地址
            */
            // * store reg
            return *(od.reg1);
        } else if (od.type == MM_IMM_REG) {
            // * error wire addr = *(od.reg1 + od.imm);
            addr = *(od.reg1) + od.imm;
        } else if (MM_REG1_REG2) 
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

void instruction_cycle() {
    inst_t* instr = (inst_t*)cpu_reg
}