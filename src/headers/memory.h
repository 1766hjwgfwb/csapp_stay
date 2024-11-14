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
uint8_t pm[PHYSICAL_MEMORY_SPACE];



/* ================================ */
/*           memory R/W             */
/* ================================ */



// * I/O
uint64_t read64bits_dram(uint64_t paddr, core_t *cr);

void wirte64bits_dram(uint64_t paddr, uint64_t data, core_t *cr);






#endif