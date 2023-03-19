#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "operation.h"
#include "memory.h"
#include "color.h"
#include "exception.h"
#include "stdbool.h"

#define MAX_NUM_TOKEN 3

#define NOARG 1

extern char operators[];

static INSTRUCTION* to_instruction(char* buffer);

static void print_instruction(int line, int opcode, bool line_on, char t1, DATA v1, char t2, DATA v2);

MEMORY* initialize_memory(int memsize) {

	MEMORY* memory = malloc(sizeof(*memory));

	if (memory == NULL) {
		fprintf(stderr, "MALLOC FAILED");
		exit(EXIT_FAILURE);
	}

	memory->mem_size = memsize;

	memory->used_size = 0;
	memory->insts = malloc(sizeof(*(memory->insts)) * memory->mem_size);

	return memory;
}

MEMORY* load_file(MEMORY* memory, char* file_name) {

	if (memory == NULL) {
		print_exception("Wrong Memory Error!");
		return memory;
	}

	if (memory->used_size == memory->mem_size) {
		print_exception("Memory Is Full!");
		return memory;
	}

	if (file_name == NULL || strlen(file_name) <= strlen(".camp")) {
		print_exception("Wrong File Name!");
		return memory;
	}

	FILE* fp = fopen(file_name, "r");

	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN FAILED");
		exit(EXIT_FAILURE);
	}

	char* buffer = calloc(BUFLEN, sizeof(*buffer));

	int memory_begin = memory->used_size;

	while (feof(fp) == 0)
	{
		if (buffer != fgets(buffer, BUFLEN, fp)) {
			print_exception("File Read Error!");
			memset(memory->insts + memory_begin, 0, memory->mem_size - memory_begin);
			memory->used_size = memory_begin;
		}
		
		buffer[strcspn(buffer, "\r\n")] = 0;

		if (memory->used_size + 1 > memory->mem_size) {
			print_exception("Allocation Exceeds Memory Size!");
			memset(memory->insts + memory_begin, 0, memory->mem_size - memory_begin);
			memory->used_size = memory_begin;
			break;
		}

		memory->insts[memory->used_size] = to_instruction(buffer);
		memory->used_size += 1;
	}

	fclose(fp);

	return memory;
}

static INSTRUCTION* to_instruction(char* buffer) {

	INSTRUCTION* instruction = malloc(sizeof(*instruction));

	if (instruction == NULL) {
		fprintf(stderr, "MALLOC FAILED");
		exit(EXIT_FAILURE);
	}

	char** str_token = malloc(sizeof(*str_token) * MAX_NUM_TOKEN);

	if (str_token == NULL) {
		fprintf(stderr, "MALLOC FAILED");
		free(instruction);
		exit(EXIT_FAILURE);
	}

	for (int i = 1; i < 3; i++) str_token[i] = NULL;

	char* ptr;

	int num_of_tokens;

	for (int i = 0, ptr = strtok(buffer, " "); ptr != NULL; num_of_tokens = i++, ptr = strtok(NULL, " ")) str_token[i] = ptr;

	instruction->opcode = encode_token(*str_token[0]);

	OPERAND* operands[2];

	for (int i = 1; i <= 2; i++) {

		if (str_token[i] == NULL) {
			operands[i - 1] = NULL;
			break;
		}

		operands[i - 1] = malloc(sizeof(*operands[i - 1]));
		operands[i - 1] = operand_initializer(str_token[i]);
	}

	instruction->op1 = operands[0];
	instruction->op2 = operands[1];

	return instruction;
}

void visualize_memory(MEMORY* memory, INSTRUCTION* ip) {
	
	int memory_size = memory->mem_size;
	
	int used_size = memory->used_size;

	for (int i = 0; i < used_size; i++) {

		int opcode = memory->insts[i]->opcode;

		bool line_on = memory->insts[i] == ip;

		char t1, t2;
		DATA v1, v2;

		if (memory->insts[i]->op1 == NULL) {
			t1 = NOARG;
			v1 = NOARG;
			t2 = NOARG;
			v1 = NOARG;
		} else {
			t1 = memory->insts[i]->op1->type;
			v1 = memory->insts[i]->op1->value;

			if (memory->insts[i]->op2 == NULL) {
				t2 = NOARG;
				v2 = NOARG;
			} else {
				t2 = memory->insts[i]->op2->type;
				v2 = memory->insts[i]->op2->value;
			}
		}

		if (t1 == '0') t1 = '\0';
		if (t2 == '0') t2 = '\0';
		
		print_instruction(i, opcode, line_on, t1, v1, t2, v2);
	}
}

static void print_instruction(int line, int opcode, bool line_on, char t1, DATA v1, char t2, DATA v2) {
	
	char cursor;
	
	if (line_on) {
		cursor = '>';
		fprintf(stdout, RED "");
	} else {
		cursor = ' ';
		fprintf(stdout, GREEN "");
	}

	fprintf(stdout, "%c %#x\t| %c ", cursor, line, operators[opcode]);
	
	if (t1 == 'R') fprintf(stdout, " %c%d ", t1, v1);
	else if (t1 != NOARG) fprintf(stdout, " %c%#x", t1, v1);
	
	if (t2 == 'R') fprintf(stdout, " %c%d", t2, v2);
	else if (t2 != NOARG) fprintf(stdout, " %c%#x", t1, v1);

	fprintf(stdout, "\n");

}