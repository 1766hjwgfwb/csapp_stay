/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<dlfcn.h>
#include<headers/linker.h>
#include<headers/common.h>

const char *EXECUTABLE_DIR = "/csapp_stay/files/exe";


int main(int argc, char *argv[]) {
    char *elf_fn[64];
    char *eof_fn;
    int elf_num = 0;


    // parse command line arguments
    int eof_flag = 0;
    for (int i = 1; i < argc; i++) {
        char *str = argv[i];
        if (strcmp(str, "-h") == 0 || strcmp(str, "--help") == 0) { 
            printf("./bin/link <ELF file> ... <ELF file> -o <EOF file>\n");
            exit(0);
        } else if (strcmp(argv[i], "-o") == 0) { 
            eof_flag = 1;
            continue;
        }

        if (eof_flag == 0) { 
            // elf files
            elf_fn[elf_num] = str;  // e.g. linker.c "xxx.elf.txt"
            elf_num++;
        } else {
            // eof file
            eof_fn = str;         // e.g. linker.c ... -o "xxx.eof.txt"
        }
    }

    // parse the arguments

    // dynamically load the shared libraries
    void *linklib = dlopen("/csapp_stay/bin/staticLink.so", RTLD_LAZY);
    // `RTLD_LAZY` flag is used to avoid loading the library immediately
    if (linklib == NULL) {
        printf("%s\n", dlerror());
        exit(1);
    }

    // functions from shared library
    void (*link_elf)(elf_t **, int, elf_t *);
    void (*write_eof)(const char *, elf_t *);
    void (*parse_elf)(const char *, elf_t *);
    void (*free_elf)(elf_t *);

    link_elf = dlsym(linklib, "link_elf");
    write_eof = dlsym(linklib, "write_eof");
    parse_elf = dlsym(linklib, "parse_elf");
    free_elf = dlsym(linklib, "free_elf");


    // do front end logic
    printf("we are DYNAMICALLY LINKING ./bin/linker.so to do STATIC linking:\nlinking ");
    elf_t **srcs = malloc(elf_num * sizeof(elf_t *));
    for (int i = 0; i < elf_num; i++) {
        char elf_fullpath[100];
        sprintf(elf_fullpath, "%s/%s.elf.txt", EXECUTABLE_DIR, elf_fn[i]);

        srcs[i] = malloc(sizeof(elf_t));
        parse_elf(elf_fullpath, srcs[i]);
    }


    // link the ELFs
    elf_t linked;
    link_elf(srcs, elf_num, &linked);

    // write the EOF
    char eof_fullpath[100];
    sprintf(eof_fullpath, "%s/%s.eof.txt", EXECUTABLE_DIR, eof_fn);

    write_eof(eof_fullpath, &linked);


    // free the memory
    for (int i = 0; i < elf_num; i++) {
        free_elf(srcs[i]);
    }

    for (int i = 0; i < elf_num; i++) {
        free(srcs[i]);
    }

    free(srcs);

    return 0;
}