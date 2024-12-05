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



int read_elf(const char *filename, uint64_t bufaddr) {
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

        // * len[len - 1] = '\0' , so we can access line[len - 1 - 1], because we need to i < len - 1
        for (int i = 0; i < len - 1; i++)
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
                if ((line[i] == '\n') || (line[i] == '\r') || ((i + 1 < len) && (i + 1 < MAX_ELF_FILE_WIDTH) && (line[i] == '/') && (line[i+1] == '/')))
                    break;

                // copy char to linebuffer
                linebuf[i] = line[i];
                i++;
            }
            
            // * add null terminator
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