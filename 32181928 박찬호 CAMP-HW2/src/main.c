#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decode.h"

#include "mode.h"

#include "mips.h"

#include "execute.h"

#ifdef __DEBUG_MODE__
#include "color.h"

static void view_registers(int* registers);
static void clrscr(void);
#endif

int main(int argc, char* argv[]) {

    char* file_name = argv[1];

    FILE* fp = fopen(file_name, "rb");

    if (fp == NULL) {
        fprintf(stderr, "FILE OPEN FAILED");
        exit(EXIT_FAILURE);
    }

    unsigned int registers[32];

    initialize_registers(registers);

    int* memory = malloc(100000000 * sizeof(*memory));

    if (memory == NULL) exit(EXIT_FAILURE);

    memset(memory, -1, 100000000);

    int* mem_cursor = memory;

    while (feof(fp) == 0) {
        int ret;

        char buffer[4];

        for (int i = 0; i < 4; i++) {
            ret = fread(&buffer[3 - i], 1, 1, fp);
        }

        if (ret != 1) {

#ifdef __DEBUG_MODE__
            printf("FILE END");
#endif
            break;
        }

        int input = *(int*)buffer;

        *mem_cursor = input;

#ifdef __DEBUG_MODE__
        printf("%#X\n", *mem_cursor);
#endif

        mem_cursor++;
    }

    int pc = 0;

    char* const pc_cursor = (char*) memory;

    int count = 0;

    while (pc != -1) {

#ifdef __DEBUG_MODE__
        clrscr();

        view_registers(registers);

        fprintf(stdout, "\n\n\n");
#endif
        //FETCH

        int temp_pc = pc;

        int inst = *(int*)(pc_cursor + pc);

        if (inst == -1) break;

#ifdef __DEBUG_MODE__
        printf("\n%#X\n\n", inst);
#endif

        if (inst == 0) {
            count++;
            pc += 4;
            continue;
        }

        int opcode;

        // DECODE

        INSTRUCTION* instruction = decode(inst, &opcode);

        // EXECUTE

        execute(instruction, registers, &pc, memory, opcode);

        free(instruction);

        if (temp_pc == pc) pc += 4;

        count++;

#ifdef __DEBUG_MODE__
        fprintf(stdout, NC "\n\n TO CONTINUE, PRESS ENTER");

		while (1) {
			char c = getch();

			if (c == 10) break;
		}
#endif
    }

    printf("\nR2 : %d  |  Count : %d\n", registers[2], count);

    free(memory);

    fclose(fp);

    return 0;
}

#ifdef __DEBUG_MODE__
static void view_registers(int* registers) {

   char* str = "  R   E   G   I   S   T   E   R  ";

   for (int i = 0; i < 32; i++) {

       fprintf(stdout, BLUE "⁕ %c \t|  %d[%d] ", str[i], i, registers[i]);

       int temp = registers[i];

       fprintf(stdout, NC "\n");
   }
}

static void clrscr(void) {
   printf("%c[2J%c[1;1H", 27, 27);
}
#endif