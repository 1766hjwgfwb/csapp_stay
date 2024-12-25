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
#define MAX_RELOCATION_LINES 64


// static link (a.o b.o) -> ab.o
// update symbol table and relocation table
// handle symbol precedence


// internal mapping
typedef struct {
    elf_t *elf;
    sym_entry_t *src;
    // relocation entry (section, sybol) to ref symbol

    sym_entry_t *dst;
    // used for relocation
}smap_t;    // store the mapping of src -> dst

// * symbol table processing
// handle symbol table
static void symbol_processing(elf_t **srcs, int num_srcs, elf_t *dst, smap_t *smap_table, int *smap_count);
// handle relocation table
static void simple_resolution(sym_entry_t *sym, elf_t *sym_elf, smap_t *candidate);
// handle symbol precedence
static int symbol_precedence(sym_entry_t *sym);

// * section merging
static void compute_section_header(elf_t *dst, smap_t *smap_table, int *smap_count);
static void merge_section(elf_t **srcs, int num_srcs, elf_t *dst, smap_t *smap_table, int *smap_count);

// * relocation processing
// relocation symbol
static void relocation_processing(elf_t **src, int num_srcs, elf_t *dst, smap_t *smap_table, int *smap_count);
static void R_X86_64_32_handler(elf_t *dst, st_entry_t *sh , int row_reference, int col_reference, int addend, sym_entry_t *sym_referenced);
static void R_X86_64_PC32_handler(elf_t *dst, st_entry_t *sh, int row_reference, int col_reference, int addend, sym_entry_t *sym_referenced);
static void R_X86_64_PLT32_handler(elf_t *dst, st_entry_t *sh, int row_reference, int col_reference, int addend, sym_entry_t *sym_referenced);

typedef void (*rela_handler_t)(elf_t *, st_entry_t *, int, int, int, sym_entry_t *);

static rela_handler_t rela_handler_table[3] = {
    &R_X86_64_32_handler,
    &R_X86_64_PC32_handler,
    &R_X86_64_PLT32_handler,
};

// * helper functions
// parse enum
static const char *get_stb_string(st_bind_t bind);
static const char *get_stt_string(st_type_t type);






static const char *get_stb_string(st_bind_t bind) {
    switch (bind) { 
        case STB_LOCAL:
            return "STB_LOCAL";
        case STB_GLOBAL:
            return "STB_GLOBAL";
        case STB_WEAK:
            return "STB_WEAK";
        default:
            debug_printf(DEBUG_LINKER, "unknown bind type %d\n", bind);
            exit(0);
    }
}


static const char *get_stt_string(st_type_t type) {
    switch (type) {
        case STT_NOTYPE:
            return "STT_NOTYPE";
        case STT_OBJECT:
            return "STT_OBJECT";
        case STT_FUNC:
            return "STT_FUNC";
        default:
            debug_printf( DEBUG_LINKER, "unknown type %d\n", type);
            exit(0);
    }
}


static void relocation_processing(elf_t **src, int num_srcs, elf_t *dst, smap_t *smap_table, int *smap_count) {
    // get eof sections, text and data
    st_entry_t *eof_text_sh = NULL;
    st_entry_t *eof_data_sh = NULL;

    for (int i = 0; i < dst->sh_count; i++) {
        if (strcmp(dst->sht[i].sh_name, ".text") == 0)
            eof_text_sh = &(dst->sht[i]);
        else if (strcmp(dst->sht[i].sh_name, ".data") == 0)
            eof_data_sh = &(dst->sht[i]);
    }
        

    // update the relocation entry r_row, r_col
    for (int i = 0; i < num_srcs; i++) {
        elf_t *elf = src[i];

        // .rela.text
        for (int j = 0; j < elf->reltext_count; j++) { 
            rl_entry_t *rela = &(elf->reltext[j]);

            // search the referenced symbol
            for (int k = 0; k < elf->sym_count; k++) { 
                sym_entry_t *sym = &(elf->symt[k]);     // symtab of elf[i]

                if (strcmp(sym->st_shndx, ".text") == 0) {
                    // must be a symbol in.text section
                    // check if this sybol is referenced by this relocation entry

                    int sym_text_start = sym->st_value;     // look as a symbol in.text section line_count
                    int sym_text_end = sym->st_value + sym->st_size;

                    // the address is between st_value and st_value + st_size.
                    if (sym_text_start <= rela->r_row && rela->r_row <= sym_text_end) {
                        // symt[k] is referenced by rela[j].sym
                        int smap_found = 0;
                        // first for loop to find the sym in smap_table
                        // second for loop to find the sym in smap_table.dst
                        for (int t = 0; t < *smap_count; t++) {
                            if (smap_table[t].src == sym) {         //  sym have been mapped to dst
                                smap_found = 1;
                                sym_entry_t *eof_referencing = smap_table[t].dst;

                                // get smap.dst
                                for (int u = 0; u < *smap_count; ++u) {
                                    // dst.bind == STB_GLOBAL, so it's a strong symbol
                                    // only one sybmol can be mapped to a strong symbol
                                    if (strcmp(elf->symt[rela->sym].st_name, smap_table[u].dst->st_name) == 0 && smap_table[u].dst->bind == STB_GLOBAL) {
                                        // relative address (computing) == (r_row - sym->st_value)(offset) + smap_table[u].dst->st_value(runtime_addressalgorithm)
                                        sym_entry_t *eof_referenced = smap_table[u].dst;
                                        (rela_handler_table[(int)rela->r_type])(dst, eof_text_sh, rela->r_row - sym->st_value + eof_referencing->st_value, rela->r_col, rela->r_addend, eof_referenced);
                                        // index 0,1,2....
                                    }
                                }
                            }
                        }

                        // must be a symbol in.text section
                        assert(smap_found == 1);
                    }
                }
            }
        }


        // .rela.data
        for (int j = 0; j < elf->reldata_count; j++) {
            rl_entry_t *rela = &(elf->reldata[j]);

            // search the referenced symbol
            for (int k = 0; k < elf->sym_count; k++) {
                sym_entry_t *sym = &(elf->symt[k]);     // symtab of elf[i]

                if (strcmp(sym->st_shndx, ".data") == 0) {
                    // must be a symbol in.data section
                    // check if this sybol is referenced by this relocation entry

                    int sym_data_start = sym->st_value;     // look as a symbol in.data section line_count
                    int sym_data_end = sym->st_value + sym->st_size;

                    // the address is between st_value and st_value + st_size.
                    if (sym_data_start <= rela->r_row && rela->r_row <= sym_data_end) {
                        // symt[k] is referenced by rela[j].sym
                        int smap_found = 0;
                        // first for loop to find the sym in smap_table
                        // second for loop to find the sym in smap_table.dst
                        for (int t = 0; t < *smap_count; t++) {
                            if (smap_table[t].src == sym) {         //  sym have been mapped to dst
                                smap_found = 1;
                                sym_entry_t *eof_referencing = smap_table[t].dst;
                                // get smap.dst
                                for (int u = 0; u < *smap_count; ++u) {
                                    // dst.bind == STB_GLOBAL, so it's a strong symbol
                                    // only one sybmol can be mapped to a strong symbol
                                    if (strcmp(elf->symt[rela->sym].st_name, smap_table[u].dst->st_name) == 0 && smap_table[u].dst->bind == STB_GLOBAL) {
                                        // relative address (computing) == (r_row - sym->st_value)(offset) + smap_table[u].dst->st_value(runtime_addressalgorithm)
                                        sym_entry_t *eof_referenced = smap_table[u].dst;
                                        (rela_handler_table[(int)rela->r_type])(dst, eof_data_sh, rela->r_row - sym->st_value + eof_referencing->st_value, rela->r_col, rela->r_addend, eof_referenced);
                                        // index 0,1,2....
                                    }
                                }
                            }
                        }

                        // must be a symbol in.data section
                        assert(smap_found == 1);
                    }
                }
            }
        }
    }
}


static void R_X86_64_32_handler(elf_t *dst, st_entry_t *sh, int row_reference, int col_reference, int addend, sym_entry_t *sym_referenced) {
    printf("row = %d, col = %d, addend = %d, sym_referenced = %s\n", row_reference, col_reference, addend, sym_referenced->st_name);
}


static void R_X86_64_PC32_handler(elf_t *dst, st_entry_t *sh, int row_reference, int col_reference, int addend, sym_entry_t *sym_referenced) {
    printf("row = %d, col = %d, addend = %d, sym_referenced = %s\n", row_reference, col_reference, addend, sym_referenced->st_name);
}


static void R_X86_64_PLT32_handler(elf_t *dst, st_entry_t *sh, int row_reference, int col_reference, int addend, sym_entry_t *sym_referenced) {
    printf("row = %d, col = %d, addend = %d, sym_referenced = %s\n", row_reference, col_reference, addend, sym_referenced->st_name);
}


static void compute_section_header(elf_t *dst, smap_t *smap_table, int *smap_count) {
    int count_text = 0, count_data = 0, count_rodata = 0;
    for (int i = 0; i < *smap_count; i++) { 
        sym_entry_t *sym = smap_table[i].src;

        // st_size is line_number, vr bits
        if (strcmp(sym->st_shndx, ".text") == 0)
            count_text += sym->st_size;
        else if (strcmp(sym->st_shndx, ".data") == 0)
            count_data += sym->st_size;
        else if (strcmp(sym->st_shndx, ".rodata") == 0)
            count_rodata += sym->st_size;
    }

    // * count section line

    // set section header
    dst->sh_count = (count_text != 0) + (count_data != 0) + (count_rodata!= 0) + 1;    // +1 symtab

    // .bss section to be confirmed
    dst->line_count = count_text + count_data + count_rodata + *smap_count + 1 + 1 + dst->sh_count; // +1 word for line_count, +1 word for sht_count

    // dst->sym_count = *smap_count;

    // print line to buffer
    sprintf(dst->buffer[0], "%ld", dst->line_count);
    sprintf(dst->buffer[1], "%ld", dst->sym_count);



    // compute runtime address of each section
    uint64_t text_runtime_addr = 0x00400000; // rodata_addr in va2pa
    uint64_t rodata_rutime_addr = text_runtime_addr + count_text * MAX_INSTRUCTION_CHAR * sizeof(char);
    // rodata sotre 64-bit value, so it's 8 bytes
    uint64_t data_runtime_addr = rodata_rutime_addr + count_rodata * sizeof(uint64_t);


    uint64_t symtab_runtime_addr = 0;

    // wirte section header
    assert(dst->sht == NULL);
    dst->sht = malloc(sizeof(st_entry_t) * dst->sh_count);


    // wirte .text, .rodata, .data
    // the start of the offset
    uint64_t section_offset = 1 + 1 + dst->sh_count;
    int sh_index = 0;
    st_entry_t *sh = NULL;

    // handler .text section
    if (count_text > 0) {

        assert(sh_index < dst->sh_count);
        // get section header pointer
        sh = &(dst->sht[sh_index]);


        // sprintf(sh->sh_name, ".text");
        strcpy(sh->sh_name, ".text");
        sh->sh_addr = text_runtime_addr;
        sh->sh_offset = section_offset;
        sh->sh_size = count_text;

        // write buffer
        // skip the line_count and sym_count (+2)
        sprintf(dst->buffer[sh_index + 2], "%s,0x%lx,%ld,%ld", sh->sh_name, sh->sh_addr, sh->sh_offset, sh->sh_size);

        // update runtime address
        sh_index++;
        section_offset += sh->sh_size;  // update offset, point to next section
    }

    if (count_rodata > 0) {
        assert(sh_index < dst->sh_count);
        // get section header pointer
        sh = &(dst->sht[sh_index]);

        strcpy(sh->sh_name, ".rodata");
        sh->sh_addr = rodata_rutime_addr;
        sh->sh_offset = section_offset;
        sh->sh_size = count_rodata * sizeof(uint64_t);


        // write buffer
        // skip the line_count and sym_count (+2)
        sprintf(dst->buffer[sh_index + 2], "%s,0x%lx,%ld,%ld", sh->sh_name, sh->sh_addr, sh->sh_offset, sh->sh_size);
        // update runtime address
        sh_index++;
        section_offset += sh->sh_size;  // update offset, point to next section
    }

    if (count_data > 0) {
        assert(sh_index < dst->sh_count);
        // get section header pointer
        sh = &(dst->sht[sh_index]);
        strcpy(sh->sh_name, ".data");            
        sh->sh_addr = data_runtime_addr;
        sh->sh_offset = section_offset;
        sh->sh_size = count_data;

        // write buffer
        // skip the line_count and sym_count (+2)
        sprintf(dst->buffer[sh_index + 2], "%s,0x%lx,%ld,%ld", sh->sh_name, sh->sh_addr, sh->sh_offset, sh->sh_size);
        // update runtime address
        sh_index++;
        section_offset += sh->sh_size;  // update offset, point to next section
    }

    // wirte .symtab section
    assert(sh_index < dst->sh_count);

    sh = &(dst->sht[sh_index]);

    strcpy(sh->sh_name, ".symtab");
    sh->sh_addr = symtab_runtime_addr;
    sh->sh_offset = section_offset;
    sh->sh_size = *smap_count;  // smap_count is the number of symbols(symb.size)
    
    sprintf(dst->buffer[sh_index + 2], "%s,0x%lx,%ld,%ld", sh->sh_name, sh->sh_addr, sh->sh_offset, sh->sh_size);

    // todo 
    // section header table offset
    sh->sh_offset = section_offset + sh->sh_size;
    // section_offset += sh->sh_size;
    


    // assert(sh_index == dst->sh_count);
    assert(sh_index + 1 == dst->sh_count);

    if (DEBUG_VERBOSE_SET && DEBUG_LINKER) {
        printf("\nSection header:\n");

        for (int i = 0; i < dst->sh_count; i++)
            printf("%s\n", dst->buffer[i + 2]);
    }

}


// symtab of src elf merge -> smaptable 
// smaptable is a table to store the mapping of src -> dst
// src symbol == smap_table[i].src.st_name -> merge into dst
static void merge_section(elf_t **srcs, int num_srcs, elf_t *dst, smap_t *smap_table, int *smap_count) {
    int line_written = 1 + 1 + dst->sh_count;   // offset of section header table
    int sym_written = 0;    // written symbol count
    int sym_section_offset = 0;    // offset of symbol section in dst

    // scan every section entry of the dst elf file, sh_count
    for (int section_index = 0; section_index < dst->sh_count; section_index++) {
        // get the section by id
        st_entry_t *target_sh = &(dst->sht[section_index]);
        sym_section_offset = 0;
        debug_printf(DEBUG_LINKER, "merge section %s\n", target_sh->sh_name);


        // merge section content
        // scan every input elf file
        for (int i = 0; i < num_srcs; i++) {    // num_srcs is the number of input elf files

            int src_section_index = -1;
            // scan every section entry of the input elf file
            for (int j = 0; j < srcs[i]->sh_count; j++) {
                // same section as target_sh
                if (strcmp(srcs[i]->sht[j].sh_name, target_sh->sh_name) == 0)
                    src_section_index = j;
            }

            // check found this target section from src elf file
            if (src_section_index == -1) {
                // skip, search in the next ELF
                continue;
            } else {
                // found the section in this ELF srcs[i]
                // check its symbol table

                // shndex ?= sh_name
                for (int j = 0; j < srcs[i]->sym_count; j++) {
                    sym_entry_t *sym = &(srcs[i]->symt[j]);

                    // need merge symbol to dst sym table
                    if (strcmp(sym->st_shndx, target_sh->sh_name) == 0) {
                        for (int k = 0; k < *smap_count; k++) { 
                            // scan the smap table cached
                            if (sym == smap_table[k].src) {
                                debug_printf(DEBUG_LINKER, "merge symbol %s\n", sym->st_name);

                                // merge symbol into dst sym table
                                // st_size byte by byte copy
                                for (int l = 0; l < sym->st_size; l++) {
                                    int dst_index = line_written + l;
                                    // srcs[i]->sht[src_section_index].sh_offset is section offset in src elf file
                                    // sym->st_value is the offset of symbol in section of src elf file
                                    int src_index = srcs[i]->sht[src_section_index].sh_offset + sym->st_value + l;

                                    // st_size != 0
                                    assert(dst_index < MAX_ELF_FILE_LENGTH);
                                    assert(src_index < MAX_ELF_FILE_LENGTH);

                                    // srcs[i].buffer[sh_offset + st_value, sh_offset + st_value + st_size] inclusive
                                    strcpy(dst->buffer[dst_index], srcs[i]->buffer[src_index]);

                                    printf("Debug  section: [%d] %s\n", dst_index, dst->buffer[dst_index]);
                                }

                                assert(sym_written < dst->sym_count);

                                strcpy(dst->symt[sym_written].st_name, sym->st_name);
                                // dst->symt[sym_written].st_shndx = sym->st_shndx;
                                strcpy(dst->symt[sym_written].st_shndx, sym->st_shndx);
                                
                                // e.g. a, b, same section, so we need add offset to the section offset
                                dst->symt[sym_written].st_value = sym_section_offset;
                                dst->symt[sym_written].st_size = sym->st_size;
                                dst->symt[sym_written].bind = sym->bind;
                                dst->symt[sym_written].type = sym->type;


                                // update smap table
                                // will be used for relocation
                                smap_table[k].dst = &(dst->symt[sym_written]);


                                // update counter
                                sym_written++;
                                line_written += sym->st_size;
                                sym_section_offset += sym->st_size;
                            }
                        }
                        // symbol srcs[i].synt[j] has been checked
                    }
                }
                // all symbols in this section of srcs[i] has been checked
            }
        }
        // dst.sht[section_index] has been merged
    }
    // all .text, .rodata, .data section has been merged

    // finally, merge symbol table
    for (int i = 0; i < dst->sym_count; i++) {
        sym_entry_t *sym = &(dst->symt[i]);

        sprintf(dst->buffer[line_written], "%s,%s,%s,%s,%ld,%ld", sym->st_name, get_stb_string(sym->bind), get_stt_string(sym->type), sym->st_shndx, sym->st_value, sym->st_size);
        line_written++;
    }

    assert(line_written == dst->line_count);
}



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
        debug_printf(DEBUG_LINKER, "%s\t%s\t%s\t%s\t%d\t%d\n", pr->st_name, get_stb_string(pr->bind), get_stt_string(pr->type), pr->st_shndx, pr->st_value, pr->st_size);
    }

    dst->sym_count = smap_count;
    dst->symt = malloc(sizeof(sym_entry_t) * dst->sym_count);    


    // compute dst section
    compute_section_header(dst, smap, &smap_count);

    
    // merge the symbol content of srcs into dst
    merge_section(srcs, num_srcs, dst, smap, &smap_count);


    printf("------------------------------\n");

    for (int i = 0; i < dst->line_count; i++) {
        printf("merge section: [%d] %s\n", i, dst->buffer[i]);
    }


    // updata buffer
    // relocating: update the relocation entries from ELF files into EOF buffer
    relocation_processing(srcs, num_srcs, dst, smap, &smap_count);

    // check eof file
    if ((DEBUG_LINKER & DEBUG_VERBOSE_SET)) {
        printf("---------- eof file ----------\n");
        for (int i = 0; i < dst->line_count; i++)
            printf("[%d] %s\n", i, dst->buffer[i]);
    }
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
            
            sym->st_value = 0;  // clean the value of common symbol, .bss consistent
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