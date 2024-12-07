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



// int read_elf(const char *filename, uint64_t bufaddr);




int main() {
    // char buf[MAX_ELF_FILE_LENGTH][MAX_ELF_FILE_WIDTH];

    elf_t elf;
    parse_elf("/csapp_stay/files/exe/sum.elf.txt", &elf);
    // free_elf(&elf);    



    return 0;
}