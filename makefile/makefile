CC = /usr/bin/gcc

CFLAGS = -Wall -g -O2 -Werror -std=gnu99
GDB_MES = -O0 -g

EXE = program

SRC = ../src

# CODE = ../src/disk/code.c
CODE = ../src/memory/myinstruction.c ../src/disk/code.c ../src/memory/dram.c ../src/cpu/mmu.c ../src/temp/main.c

.PHONY: main
main:
	$(CC) $(GDB_MES) -I$(SRC) $(CODE) -o $(EXE)
	./$(EXE)


run:
	./$(EXE)

clean:
	rm -f $(EXE)
