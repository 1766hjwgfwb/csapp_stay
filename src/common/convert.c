#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<headers/common.h>
#include "common.h"


// * convert uint32_t to float
uint32_t uint2float(uint32_t u) {

}


// * convert string to uint64
uint64_t string2uint(const char* str) {
    return string2uint_range(str, 0, -1);
}

uint64_t string2uint_range(const char* str, int start, int end) {
    return 0x0;
}