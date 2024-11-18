/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include<stdarg.h>
#include<stdio.h>
#include<assert.h>
#include<headers/common.h>


// @brief
// * pack printf function
// * ctrl debug var
uint64_t debug_printf(uint64_t open_set, const char* format, ...) {
    if ((open_set & DEBUG_VERBOSE_SET) == 0x0)
        return 0x1;
    
    // * implementation std printf()
    va_list argprt;     // ? 变参
    va_start(argprt, format);
    vfprintf(stderr, format, argprt);
    va_end(argprt);     // ? 类似 free()


    return 0x0;
}