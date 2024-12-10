/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// * 1. No multiple strong symbols with the same name are allowed.
// * 2. Among strong symbols and weak symbols, choose the strong symbol.
// * 3. For multiple weak symbols, any one can be chosen.


#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<assert.h>
#include<headers/linker.h>
#include<headers/common.h>


#define MAX_SYMBOL_MAP_LENGHT 64
#define MAX_SECTION_BUFFER_LENGHT 64


// static link (a.o b.o) -> ab.o
// update symbol table and relocation table
// handle symbol precedence


// internal mapping
typedef struct {
    elf_t *elf;
    sym_entry_t *src;
    // todo:
    // relocation entry (section, sybol) to ref symbol

    sym_entry_t *dst;
    // used for relocation
}smap_t;


// handle symbol table
static void symbol_processing(elf_t **srcs, int num_srcs, elf_t *dst, smap_t *smap, int *snum_smap);
// handle relocation table
static void simple_resolution(sym_entry_t *src, smap_t *map);
// handle symbol precedence
static int symbol_precedence(sym_entry_t *sym);



/* ------------------------------------ */
/* external interface for static linker */
/* ------------------------------------ */
void link_elf(elf_t **srcs, int num_srcs, elf_t *dst) {
    // reset dst elf
    memset(dst, 0, sizeof(elf_t));

    // create map src -> dst
    int smap_count = 0;     // * dymamic allocation
    smap_t smap[MAX_SYMBOL_MAP_LENGHT];

    // update the smap table - symbol processing
    symbol_processing(srcs, num_srcs, dst, smap, &smap_count);

    // todo:
    // compute dst section


    // todo:
    // merge the symbol content of srcs into dst
}


static void symbol_processing(elf_t **srcs, int num_srcs, elf_t *dst, smap_t *smap, int *snum_smap) {
    // for every elf files
    for (int i = 0; i < num_srcs; i++) { 
        elf_t *elfp = srcs[i];
        
        // for every symbol table entry
        for (int j = 0; j < elfp->sym_count; j++) { 
            // current symbol table entry
            sym_entry_t *sym = &(elfp->symt[j]);

            if (sym->bind == STB_LOCAL) {
                // local symbol insert into symbol map
                // no local conflict
                assert(*snum_smap < MAX_SYMBOL_MAP_LENGHT);

                // todo:
                // dst symbol -> src symbol
            } else {
                // weak symbol uncommon
                // so we need check sybmbol GLOBAL, handle conflict
                for (int k = 0; k < *snum_smap; k++) { 
                    // todo:
                    // check name conflict
                    if (1) {
                        // handle name conflict, do simple symbol resolution
                        simple_resolution(sym, &smap[k]);
                        goto NEXT_SYMBOL_PROCESS;
                    }
                }
                // no name conflict, insert into symbol map
                assert(*snum_smap < MAX_SYMBOL_MAP_LENGHT);
                // todo: 
                // update map table
            }
            NEXT_SYMBOL_PROCESS:
            // do nothing
            ;
        }
    }

    // all the elf files have been processed
    // cleanup:
    for (int i = 0; i < *snum_smap; i++) {
        // todo: check shn_undef here
        // todo: the remaining common go to .bss
    }
}


static inline int symbol_precedence(sym_entry_t *sym) {
    // shn_undefine is the lowest precedence
    // if (sym->st_shndx == "SHN_UNDEF")
    if ((strcmp(sym->st_shndx, "SHN_UNDEF") == 0))
        return 0;
    
    // halt
    debug_printf(DEBUG_LINKER, "symbol_precedence: %s\n", sym->st_name);
    exit(1);
}


static void simple_resolution(sym_entry_t *src, smap_t *map) {
    // src: symbol from current elf file
    // dst: pointer to the internal map table

    // 3 rules for resolving relocation:
    // update the symbol precedence
    // e.g. int a; int a = 4
    int src_pre = symbol_precedence(src);
    int dst_pre = symbol_precedence(map->src);

    // todo rule 1
    if (src_pre == dst_pre) {
        // throw exception
        // e.g. multiple definition of variable
        debug_printf(DEBUG_LINKER, "simple_resolution: multiple definition of variable %s\n", src->st_name);
        exit(1);
    }

    // todo rule 2
    if (src_pre == dst_pre)
        return;


    // todo rule 3
    // current implementation: choose the strong symbol, src has higher precedence
    map->src = src;
}