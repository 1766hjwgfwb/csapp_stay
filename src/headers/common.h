/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// * double declaration of any identifiers
// * such as types, enums and static var
#ifndef DEBUG_GUARD
#define DEBUG_GUARD


#include <stdint.h>
// #include<headers/cpu.h>


#define DEBUG_INSTRUCTIONCYCLE      (0x1)
#define DEBUG_REGISTERS             (0x2)
#define DEBUG_PRINTSTACK            (0X4)
#define DEBUG_PRINTCACHESET         (0X8)
#define BEBUG_CACHEDETAILS          (0X10)
#define DEBUG_MMU                   (0X20)
#define DEBUG_LINKER                (0X40)
#define DEBUG_LOADER                (0X80)
#define DEBUG_PARSEINST             (0X100)

#define DEBUG_VERBOSE_SET          (0X106)


#define DEBUG_ENABLE_PAGE_CACHE     (0)


#define DEBUG_ENABLE_SRAM_CACHE     (1)


// * printf wrapper
uint64_t debug_printf(uint64_t open_set, const char* format, ...);


// * type converter
// * u32_t to float
uint32_t uint2float(uint32_t u);


// * converter string dec or hex to the integer bitmap
uint64_t string2uint(const char* str);
uint64_t string2uint_range(const char* str, int start, int end);


//* ============================= */
//*        clean up events        */
//* ============================= */
void add_cleanup_event(void *func);
void finally_cleanup();



//* ============================= */
//*     wrap of the memory        */
//* ============================= */
void *tag_malloc(uint64_t size, char *tagstr);
int tag_free(void *ptr);
void tag_sweep(char *tagstr);





#endif