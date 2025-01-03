/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "headers/linker.h"
#include "headers/common.h"
#include "headers/algorithm.h"


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

    /*array_t *arr = array_construct(16);
    array_insert(&arr, 1);
    array_insert(&arr, 2);
    array_insert(&arr, 3);


    array_delete(arr, 3);

    printf("array count: %d, size: %d\n", arr->count, arr->size);

    print_array(arr);


    array_free(arr);
    */


   linkedlist_t *list = linkedlist_construct();

   linkedlist_add(&list, 1);
   linkedlist_add(&list, 2);
   linkedlist_add(&list, 3);
   linkedlist_add(&list, 4);
   linkedlist_add(&list, 5);


   linkedlist_delete(list, linkedlist_get(list, 1));


   linkedlist_free(list);


    return 0;
}