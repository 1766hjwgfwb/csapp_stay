CC = /usr/bin/gcc

CFLAGS = -Wall -g -O2 -Werror -std=gnu99 -Wno-unused-function
GDB_MES = -O0 -g

EXE = program

SRC = ./src

# CODE = ../src/disk/code.c
# CODE = ../src/memory/myinstruction.c ../src/disk/code.c ../src/memory/dram.c ../src/cpu/mmu.c ../src/temp/main.c
# debug
COMMON = $(SRC)/common/print.c $(SRC)/common/convert.c


#.PHONY: csapp
# main:
#	$(CC) $(GDB_MES) -I$(SRC) $(CODE) -o $(EXE)
#	./$(EXE)


# 分离编译

# handware
CPU = $(SRC)/headware/cpu/mmu.c $(SRC)/headware/cpu/isa.c
MEMORY = $(SRC)/headware/memory/dram.c


# main
MAIN = $(SRC)/main_headware.c



.PHONY:csapp
csapp:
	$(CC) $(CFLAGS) -I$(SRC) $(COMMON) $(CPU) $(MEMORY) $(MAIN) -o $(EXE)
	./$(EXE)


clean:
	rm -f $(EXE)
