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
#include<headers/algorithm.h>


// * static link
// 1. parse elf file, create smap_table
// 2. elf.sections merge to eof.segments(section to segment), combine to elf.segments runtime_address
// 3. will elf.symbols relocation to eof.symbols(symbol to segment)




int main() {
    // char buf[MAX_ELF_FILE_LENGTH][MAX_ELF_FILE_WIDTH];

    /*elf_t src[2];
    parse_elf("/csapp_stay/files/exe/sum.elf.txt", &src[0]);
    printf("----------------- elf seg ------------------\n");
    parse_elf("/csapp_stay/files/exe/main.elf.txt", &src[1]);

    elf_t dst;
    elf_t *scrp[2];
    scrp[0] = &src[0];
    scrp[1] = &src[1];

    link_elf((elf_t**)&scrp, 2, &dst);

    write_eof("/csapp_stay/files/exe/output.eof.txt", &dst);

    // parse_elf already free the memory of elf, so we don't need to free it again.
    // free_elf(&elf);    

    free_elf(&src[0]);  
    free_elf(&src[1]);
    free_elf(&dst);
    */

    hash_table_t *ht = hashtable_construct(4);

    // insert 11,4,21,25,15,14,1,9,20,2,17,26,3,24,30,99
    hashtable_insert(ht, 11, 1);
    hashtable_insert(ht, 4, 2);
    hashtable_insert(ht, 21, 3);
    hashtable_insert(ht, 25, 4);
    hashtable_insert(ht, 15, 5);
    hashtable_insert(ht, 14, 6);
    hashtable_insert(ht, 1, 7);
    hashtable_insert(ht, 9, 8);
    hashtable_insert(ht, 20, 9);
    hashtable_insert(ht, 2, 10);
    hashtable_insert(ht, 17, 11);
    hashtable_insert(ht, 26, 12);
    hashtable_insert(ht, 3, 13);
    hashtable_insert(ht, 7, 14);
    hashtable_insert(ht, 24, 14);
    hashtable_insert(ht, 30, 15);

    // int key;
    // hashtable_get(ht, 20, &key);
    // printf("key: %d, value: %d\n", 11, key);

    // hashtable_delete(ht, 21);

    hashtable_print(ht);

    hashtable_free(ht);

    return 0;
}