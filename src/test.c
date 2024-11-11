#include<stdio.h>
#include<stdint.h>


struct cl {
    int a;
};



int main() {
    int a = 6;
    struct cl c;


    printf("%p\n", &a);
    printf("%p\n", &c);
}