#include "operator.h"
#include "operand.h"

#include <string.h>

const char operators[] = { '+', '-', '*', '/', '%', 'M', 'C', 'J', 'B', 'H', '\0' };

int encode_token(char token) {
	return strchr(operators, token) - operators;
}

void add(OPERAND* operand1, void* operand2, DATA* registers) {
	
	DATA op1 = value_by_type(operand1, registers);
	
	DATA op2 = value_by_type((OPERAND*)operand2, registers);

	registers[0] = op1 + op2;

	return;
}

void sub(OPERAND* operand1, void* operand2, DATA* registers) {

	DATA op1 = value_by_type(operand1, registers);

	DATA op2 = value_by_type((OPERAND*)operand2, registers);

	registers[0] = op1 - op2;	
	
	return;
}

void mul(OPERAND* operand1, void* operand2, DATA* registers) {
	
	DATA op1 = value_by_type(operand1, registers);

	DATA op2 = value_by_type((OPERAND*)operand2, registers);

	registers[0] = op1 * op2;

	return;
}

void quo(OPERAND* operand1, void* operand2, DATA* registers) {

	DATA op1 = value_by_type(operand1, registers);

	DATA op2 = value_by_type((OPERAND*)operand2, registers);

	registers[0] = op1 / op2;
	return;
}

void mod(OPERAND* operand1, void* operand2, DATA* registers) {
	
	DATA op1 = value_by_type(operand1, registers);

	DATA op2 = value_by_type((OPERAND*)operand2, registers);

	registers[0] = op1 % op2;

	return;
}

void mov(OPERAND* op1, void* op2, DATA* registers) {

	DATA ret_op2 = value_by_type((OPERAND*)op2, registers);

	registers[op1->value] = ret_op2;

	return;
}

void cmp(OPERAND* operand1, void* operand2, DATA* registers) {

	DATA op1 = value_by_type(operand1, registers);

	DATA op2 = value_by_type((OPERAND*)operand2, registers);

	registers[0] = op1 < op2 ? 1 : 0;

	return;
}

void jmp(OPERAND* op1, void* pc, DATA* registers) {

	int* program_counter = (int*)pc;
	*program_counter = op1->value;
	(*program_counter)--;
}

void brn(OPERAND* op1, void* pc, DATA* registers) {

	if (registers[0] == 1) {
		int* program_counter = (int*)pc;
		*program_counter = op1->value;
		(*program_counter)--;
	}
}

