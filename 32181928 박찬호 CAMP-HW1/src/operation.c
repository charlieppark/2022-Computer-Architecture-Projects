#include "operation.h"

static void clrscr(void) {
	printf("%c[2J%c[1;1H", 27, 27);
}

static void view_registers(DATA* registers) {

	char* str = " REGISTER ";

	for (int i = 0; i < 10; i++) {

		fprintf(stdout, BLUE "⁕ %c \t|  %d[%d] ", str[i], i, registers[i]);

		DATA temp = registers[i];
	
		fprintf(stdout, NC "\n");
	}
}

void operation(MEMORY* memory, DATA* registers)
{
	void (*fp[9])(OPERAND*, void*, DATA*);

	fp[0] = &add;
	fp[1] = &sub;
	fp[2] = &mul;
	fp[3] = &quo;
	fp[4] = &mod;
	fp[5] = &mov;
	fp[6] = &cmp;
	fp[7] = &jmp;
	fp[8] = &brn;

	int size = memory->used_size;

	for (int pc = 0; pc < size; pc++) {
		
		clrscr();

		view_registers(registers);

		fprintf(stdout, "\n\n\n");

		visualize_memory(memory, memory->insts[pc]);

		int opcode = memory->insts[pc]->opcode;

		if (opcode == 9) break;

		OPERAND* operand1 = memory->insts[pc]->op1;

		void* operand2 = opcode < 7 ? memory->insts[pc]->op2 : &pc;

		fp[opcode](operand1, operand2, registers);

		fprintf(stdout, NC "\n\n TO CONTINUE, PRESS ENTER");

		while (1) {
			char c = getch();

			if (c == 10) break;
		}
	}
}