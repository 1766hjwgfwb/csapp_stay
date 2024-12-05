/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */


#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<headers/linker.h>
#include<headers/common.h>



int read_elf(const char *filename, uint64_t bufaddr);





int main() {
    char buf[MAX_ELF_FILE_LENGTH][MAX_ELF_FILE_WIDTH];

    int count = read_elf("./files/exe/sum.elf.txt", (uint64_t)&buf);

    for(int i = 0; i < count; i++) { 
        printf("%s\n", buf[i]);
    }

    printf("\n");
    return 0;
}