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







int main() {
    // char buf[MAX_ELF_FILE_LENGTH][MAX_ELF_FILE_WIDTH];

    elf_t src[2];
    parse_elf("/csapp_stay/files/exe/sum.elf.txt", &src[0]);
    printf("\n");
    parse_elf("/csapp_stay/files/exe/main.elf.txt", &src[1]);


    // parse_elf already free the memory of elf, so we don't need to free it again.
    // free_elf(&elf);    


    return 0;
}