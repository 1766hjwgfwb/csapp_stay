/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<assert.h>
#include<headers/linker.h>
#include<headers/common.h>




static int parse_table_entry(char *str, char ***ent);
static void free_table_entry(char **ent, int n);
static void parse_sh(char *str, st_entry_t *sh);
static void parse_symtab(char *str, sym_entry_t *symtab);
static void print_sh_entry(st_entry_t *sh);
static void print_sym_entry(sym_entry_t *sym);
static int read_elf(const char *filename, uint64_t bufaddr);
static void free_elf(elf_t *elf);



static int parse_table_entry(char *str, char ***ent) {
    // * parse line as table entry
    int count_col = 1;          // begin with 1, because we need to count the first column
    int len = strlen(str);

    for (int i = 0; i < len; i++) 
        if (str[i] == ',')
            count_col++;

    // char ** p = malloc(*p)
    char **arr = malloc(count_col * sizeof(char *));

    if (arr == NULL) {
        debug_printf(DEBUG_LINKER, "Error: malloc failed\n");
        exit(1);
    }

    *ent = arr;     // we need to update the pointer to the array [][]



    int col_index = 0;
    int col_width = 0;
    char col_buf[32];
    for (int i = 0; i < len + 1; i++) {
        if (str[i] == ',' || str[i] == '\0') {
            assert(col_index < count_col);
            
            // * malloc and copy, update col_index and col_width
            char *col = malloc((col_width + 1) * sizeof(char));
            for (int j = 0; j < col_width; j++)
                col[j] = col_buf[j];

            col[col_width] = '\0';

            // * add to array
            arr[col_index] = col;               // e.g. {"sum", "STB_LOCAL"}

            // printf("col_index: %d, col_width: %d, col: %s, count_col: %d\n", col_index, col_width, col, count_col);
            col_index++;
            col_width = 0;
        } else {

            assert(col_width < 32);
            col_buf[col_width] = str[i];        // e.g. sum, STB_LOCAL
            col_width++;
        }
    }

    return count_col;
}


static void free_table_entry(char **ent, int n) {
    // 1. free strings in array
    // 2. free array
    for (int i = 0; i < n; i++) {
        if (ent[i]!= NULL)
            free(ent[i]);
    }

    free(ent);
}


static void parse_sh(char *str, st_entry_t *sh) {
    char **cols;
    // why we need to &cols?
    // because we need to update the pointer to the array [][]
    int n_cols = parse_table_entry(str, &cols);
    assert(n_cols == 4);


    // * parse sh_name, sh_addr, sh_offset, sh_size
    // .text, 0x0, 4, 22
    strcpy(sh->sh_name, cols[0]);
    sh->sh_addr = string2uint(cols[1]);
    sh->sh_offset = string2uint(cols[2]);
    sh->sh_size = string2uint(cols[3]);


    free_table_entry(cols, n_cols);
}


static void parse_symtab(char *str, sym_entry_t *symtab) { 
    char **cols;

    int n_cols = parse_table_entry(str, &cols);
    assert(n_cols == 6);

    // * parse st_name, bind, type, st_shndex, st_value, st_size
    // sum,STB_GLOBAL,STT_FUNC,.text,0,22
    strcpy(symtab->st_name, cols[0]);
    
    // * bind
    if (strcmp(cols[1], "STB_GLOBAL") == 0)
        symtab->bind = STB_GLOBAL;
    else if (strcmp(cols[1], "STB_WEAK") == 0)
        symtab->bind = STB_WEAK;
    else if (strcmp(cols[1], "STB_LOCAL") == 0)
        symtab->bind = STB_LOCAL;
    else {
        debug_printf(DEBUG_LINKER, "Error: unknown bind type %s\n", cols[1]);
        exit(1);
    }

    // * type
    if (strcmp(cols[2], "STT_NOTYPE") == 0)
        symtab->type = STT_NOTYPE;
    else if (strcmp(cols[2], "STT_OBJECT") == 0)
        symtab->type = STT_OBJECT;
    else if (strcmp(cols[2], "STT_FUNC") == 0)
        symtab->type = STT_FUNC;
    else {
        debug_printf(DEBUG_LINKER, "Error: unknown type %s\n", cols[2]);
    }

    strcpy(symtab->st_shndx, cols[3]);
    symtab->st_value = string2uint(cols[4]);
    symtab->st_size = string2uint(cols[5]);

    free_table_entry(cols, n_cols);
}


static void print_sh_entry(st_entry_t *sh) {
    debug_printf(DEBUG_LINKER, "sh_name: %s\tsh_addr: %lx\tsh_offset: %lx\tsh_size: %lx\n", sh->sh_name, sh->sh_addr, sh->sh_offset, sh->sh_size);
}


static void print_sym_entry(sym_entry_t *sym) {
    debug_printf(DEBUG_LINKER, "syt_name: %s\tbind: %d\ttype: %d\tsyt_shndx: %s\tsyt_value: %lx\tsyt_size: %lx\n", sym->st_name, sym->bind, sym->type, sym->st_shndx, sym->st_value, sym->st_size);
}


/// @brief read elf file and store it in buffer 
/// @param filename 
/// @param bufaddr 
/// @return count of lines in file
static int read_elf(const char *filename, uint64_t bufaddr) {
    // * open file and read
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) { 
        debug_printf(DEBUG_LINKER, "Error: Cannot open file %s\n", filename);
        exit(1);
    }

    // * read text section
    char line[MAX_ELF_FILE_WIDTH];
    int line_count = 0;

    while (fgets(line, MAX_ELF_FILE_WIDTH, fp) != NULL) {
        // * skip comments and empty lines
        int len = strlen(line);
        if ((len == 0) || (len >= 1 && (line[0] == '\n' || line[0] == '\r')) || (len >= 2 && (line[0] == '/' && line[1] == '/')))
            continue;

        // * check if is empty line
        int iswhite = 1;

        for (int i = 0; i < len; i++)
            iswhite = iswhite && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r');

        if (iswhite == 1)
            continue;



        // * parse line

        if (line_count < MAX_ELF_FILE_LENGTH) {
            // * store line in buffer[line_count]
            uint64_t addr = bufaddr + line_count * MAX_ELF_FILE_WIDTH * sizeof(char);
            char *linebuf = (char *)addr;
            
            // * paste line to linebuffer
            int i = 0;
            while (i < len && i < MAX_ELF_FILE_WIDTH) { 
                // ? line[i] == '\n' || line[i] == '\r'
                if ((line[i] == '\n') || (line[i] == '\r') || ((i + 1 < len) && (i + 1 < MAX_ELF_FILE_WIDTH) && (line[i] == '/') && (line[i + 1] == '/')))
                    break;

                // copy char to linebuffer
                linebuf[i] = line[i];
                i++;
            }
            
            // * add null terminator
            // printf("linebuf: %s\n, i: %d\n", linebuf, i);
            linebuf[i] = '\0';
            line_count++;

        } else {
            debug_printf(DEBUG_LINKER, "Error: Too many lines in file %s\n, max length is %d\n", filename, MAX_ELF_FILE_LENGTH);
            
            fclose(fp);
            exit(1);
        }
    }


    fclose(fp);
    assert(line_count == (string2uint((char *)bufaddr)));       // * bufaddr is the start address

    return line_count;
}

static void free_elf(elf_t *elf) {
    // * free elf sht
    assert(elf->sht != NULL);
    assert(elf->symt != NULL);

    free(elf->sht);
    free(elf->symt);
}


void parse_elf(const char *filename, elf_t *elf) {
    int line_count = read_elf(filename, (uint64_t)(&elf->buffer));

    // * print elf buffer
    for (int i = 0; i < line_count; i++) 
        printf("[%d]\t%s\n", i, elf->buffer[i]);

    // * buffer[1] = section table count
    int sh_count = string2uint(elf->buffer[1]);
    elf->sht = malloc(sh_count * sizeof(st_entry_t));
    
    elf->sh_count = sh_count;
    elf->line_count = line_count;


    st_entry_t *sym_sh = NULL;
    for (int i = 0; i < sh_count; i++) {
        parse_sh(elf->buffer[i + 2], &(elf->sht[i]));
        print_sh_entry(&elf->sht[i]);
        
        if (strcmp(elf->sht[i].sh_name, ".symtab") == 0) { 
            sym_sh = &elf->sht[i];     // copy symtab entry
        }
    }

    assert(sym_sh != NULL);

    elf->sym_count = sym_sh->sh_size;   // sh_size is the line count
    elf->symt = malloc(elf->sym_count * sizeof(sym_entry_t));


    for (int i = 0; i < elf->sym_count; i++) {
        parse_symtab(elf->buffer[sym_sh->sh_offset + i], &(elf->symt[i]));
        print_sym_entry(&elf->symt[i]);
    }

    // safety free
    free_elf(elf);
}
