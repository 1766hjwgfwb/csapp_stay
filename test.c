#include<stdio.h>
#include<stdarg.h>
#include<stdint.h>





int main() {
    uint64_t a = 0x2;
    uint64_t b = 0x3;
    
    *(uint64_t *)a = *(uint64_t *)b;

    printf("%lx\n", a);
}
