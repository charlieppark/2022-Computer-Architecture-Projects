#ifndef __MEMORY_H__

#define __MEMORY_H__

#include "data.h"

#define dMEMSIZE 30

#define BUFLEN 20

typedef struct _instruction {
	
	int opcode;

	OPERAND* op1;

	OPERAND* op2;

} INSTRUCTION;

typedef struct _memory {

	size_t mem_size;

	size_t used_size;

	INSTRUCTION** insts;

} MEMORY;

MEMORY* initialize_memory(int memsize);

MEMORY* load_file(MEMORY* memory, char* file_name);

void visualize_memory(MEMORY* memory, INSTRUCTION* ip);

#endif // !__MEMORY_H__
