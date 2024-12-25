/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#ifndef LINKER_H_GUARD
#define LINKER_H_GUARD

#include<stdint.h>
#include<stdlib.h>



#define MAX_CHAR_SECTIONS_NAME (32)


// * section header table entry *
typedef struct {
    char sh_name[MAX_CHAR_SECTIONS_NAME];
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;       // line offset or effective line index
}st_entry_t;


#define MAX_CHAR_SYMBOLS_NAME (64)
#define MAX_CHAR_SYMBOLS_SHNDX (16)


typedef enum {
    STB_LOCAL = 0,
    STB_GLOBAL = 1,
    STB_WEAK = 2,
}st_bind_t;


typedef enum {
    STT_NOTYPE = 0,
    STT_OBJECT = 1,
    STT_FUNC = 2,
}st_type_t;


// * symbol table entry *
typedef struct {
    char st_name[MAX_CHAR_SYMBOLS_NAME];
    char st_shndx[MAX_CHAR_SYMBOLS_SHNDX];
    st_bind_t bind;
    st_type_t type;
    uint64_t st_value;
    uint64_t st_size;
}sym_entry_t;



#define MAX_ELF_FILE_LENGTH (64)    // max length of elf file name effective line
#define MAX_ELF_FILE_WIDTH (128)    // max width of elf file name effective line


// * relocation table entry *

typedef enum {
    R_X86_64_32,
    R_X86_64_PC32,
    R_X86_64_PLT32,     // dynamic linking
}reltype_t;

// relocation entry type
typedef struct { 
    uint64_t r_row;     // line index

    uint64_t r_col;     // column index
    reltype_t r_type;   // relocation type
                        // rel.text -> .text section
                        // rel.data -> .data section
    uint32_t sym;       // symbol index
    int64_t r_addend;   // constant addend for instruction line
}rl_entry_t;



typedef struct {
    char buffer[MAX_ELF_FILE_LENGTH][MAX_ELF_FILE_WIDTH];

    uint64_t line_count;
    uint64_t sh_count;

    st_entry_t *sht;

    uint64_t sym_count;
    sym_entry_t *symt;

    uint64_t reltext_count;
    rl_entry_t *reltext;

    uint64_t reldata_count;
    rl_entry_t *reldata;
}elf_t;




// * visible functions * API
void parse_elf(const char *filename, elf_t *elf);
void link_elf(elf_t **srcs, int num_srcs, elf_t *dst);
void free_elf(elf_t *elf);


#endif /* LINKER_H_GUARD */