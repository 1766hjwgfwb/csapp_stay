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
}smap_t;    // store the mapping of src -> dst


// handle symbol table
static void symbol_processing(elf_t **srcs, int num_srcs, elf_t *dst, smap_t *smap_table, int *smap_count);
// handle relocation table
static void simple_resolution(sym_entry_t *sym, elf_t *sym_elf, smap_t *candidate);
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


    printf("---------------------------\n");
    
    for (int i = 0; i < smap_count; i++) {
        sym_entry_t *pr = smap[i].src;
        debug_printf(DEBUG_LINKER, "%s\t%d\t%d\t%s\t%d\t%d\n", pr->st_name, pr->bind, pr->type, pr->st_shndx, pr->st_value, pr->st_size);
    }



    // todo:
    // compute dst section


    // todo:
    // merge the symbol content of srcs into dst
}


static void symbol_processing(elf_t **srcs, int num_srcs, elf_t *dst, smap_t *smap_table, int *smap_count) {
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
                assert(*smap_count < MAX_SYMBOL_MAP_LENGHT);
                // even if local symbol
                smap_table[*smap_count].src = sym;
                smap_table[*smap_count].elf = elfp;
                // update smp_count
                (*smap_count)++;
                
            } else if (sym->bind == STB_GLOBAL) {
                // weak symbol uncommon
                // so we need check sybmbol GLOBAL, handle conflict
                for (int k = 0; k < *smap_count; k++) { 
                    // check name conflict
                    sym_entry_t *candidate = smap_table[k].src;
                    if (candidate->bind == STB_GLOBAL && strcmp(candidate->st_name, sym->st_name) == 0) {
                        // handle name conflict, do simple symbol resolution
                        simple_resolution(sym, elfp, &smap_table[k]);
                        goto NEXT_SYMBOL_PROCESS;
                    }
                }
                // no name conflict, insert into symbol map
                assert(*smap_count < MAX_SYMBOL_MAP_LENGHT);
                // update map table
                smap_table[*smap_count].src = sym;
                smap_table[*smap_count].elf = elfp;
                // update smp_count
                (*smap_count)++;
            }
            NEXT_SYMBOL_PROCESS:
            // do nothing
            ;
        }
    }

    // all the elf files have been processed
    // cleanup: common -> bss int a = 0;
    for (int i = 0; i < *smap_count; i++) {
        sym_entry_t *sym = smap_table[i].src;

        // if st_name or sym.type is not defined, compiler error
        assert(strcmp(sym->st_name, "STT_NOTYPE") != 0);
        assert(sym->type != STT_NOTYPE);

        if (strcmp(sym->st_shndx, "COMMON") == 0) {
            char *cr = ".bss";
            for (int j = 0; j < MAX_CHAR_SYMBOLS_SHNDX; j++) {
                if (j < 4)
                    sym->st_shndx[j] = cr[j];
                else
                    sym->st_shndx[j] = '\0';
            }
            
            sym->st_value = 0;
        }
    }
}


static inline int symbol_precedence(sym_entry_t *sym) {
    // shn_undefine is the lowest precedence
    // * Choose 4 specific types
    /*  we do not consider weak because it's very rare
        and we do not consider local because it's not conflicting
            bind        type        shndx               prec
            --------------------------------------------------
            global      notype      undef               0 - undefined
            global      object      common              1 - tentative
            global      object      data,bss,rodata     2 - defined
            global      func        text                2 - defined
    */
    // only consider global symbol
    assert(sym->bind == STB_GLOBAL);

    // if (sym->st_shndx == "SHN_UNDEF")
    if ((strcmp(sym->st_shndx, "SHN_UNDEF") == 0) && (sym->type == STT_NOTYPE))
        return 0;
    
    if ((strcmp(sym->st_shndx, "COMMON") == 0) && (sym->type == STT_OBJECT))
        return 1;
    
    if ((strcmp(sym->st_shndx, ".text") == 0 && sym->type == STT_FUNC) || 
        (strcmp(sym->st_shndx, ".data") == 0 && sym->type == STT_OBJECT) || 
        (strcmp(sym->st_shndx, ".rodata") == 0 && sym->type == STT_OBJECT) || 
        (strcmp(sym->st_shndx, ".bss") == 0 && sym->type == STT_OBJECT))
        return 2;


    // halt 
    debug_printf(DEBUG_LINKER, "symbol_precedence: %s\n", sym->st_name);
    exit(1);
}


static void simple_resolution(sym_entry_t *sym, elf_t *sym_elf, smap_t *candidate) {
    // src: symbol from current elf file
    // dst: pointer to the internal map table

    // 3 rules for resolving relocation:
    // update the symbol precedence
    // e.g. int a; int a = 4
    int pre1 = symbol_precedence(sym);
    int pre2 = symbol_precedence(candidate->src);

    if (pre1 == 2 && pre2 == 2) {
        /*  rule 1: multiple strong symbols with the same name are allowed.
            pre1    pre2
            2       2
        */
        // throw exception
        // e.g. multiple definition of variable
        debug_printf(DEBUG_LINKER, "simple_resolution: multiple definition of variable %s\n", sym->st_name);
        exit(1);
    } else if (pre1 != 2 && pre2 != 2) {
        /*  rule 3: select higher precedence
            pre1    pre2
            0       0
            0       1
            1       0   when pre1 > pre2, choose pre1
            1       1
        */
        if (pre1 > pre2) {
            // update the symbol map table
            // e.g. pre1 int a (1); pre2 extern int a (0);
            candidate->src = sym;
            candidate->elf = sym_elf;
        }
        return;
    } else if (pre1 == 2) {
        /*  rule 2: select cuurent sybmol, pre1 has higher precedence
            pre1    pre2
            2       0
            2       1
        */
        // current implementation: choose the strong symbol, pre1 has higher precedence
        candidate->src = sym;
        candidate->elf = sym_elf;
    }
    /*  rule 2: select candidate sybmol, do nothing
        pre1    pre2
        0       2
        1       2
    */
}