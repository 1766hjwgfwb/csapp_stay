/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// * double declaration of any identifiers
// * such as types, enums and static var
#ifndef MEMORY_GUARD
#define MEMORY_GUARD


#include<stdint.h>
#include<headers/cpu.h>




/* =============================== */ 
/*  physical memory on dram chips  */
/* =============================== */


// * MAX pm 2^16
#define PHYSICAL_MEMORY_SPACE 65536
#define MAX_INDEX_PHYSICAL_PACE 15



// * physical memory
// * 16 physical memory pages
extern uint8_t pm[PHYSICAL_MEMORY_SPACE];



/* ================================ */
/*           memory R/W             */
/* ================================ */



// * @brief 模拟器中的寄存器均为模拟(虚拟)地址  寻址操作使用 va2pa + 偏移量赋予物理地址 pm[paddrs]
// * I/O
uint64_t read64bits_dram(uint64_t paddr, core_t *cr);

void write64bits_dram(uint64_t paddr, uint64_t data, core_t *cr);






#endif