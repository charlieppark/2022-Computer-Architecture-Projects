#ifndef __OPERATOR_H__

#define __OPERATOR_H__

#include "data.h"
#include "operand.h"

int encode_token(char token);

void add(OPERAND* op1, void* op2, DATA* registers);
void sub(OPERAND* op1, void* op2, DATA* registers);
void mul(OPERAND* op1, void* op2, DATA* registers);
void quo(OPERAND* op1, void* op2, DATA* registers);
void mod(OPERAND* op1, void* op2, DATA* registers);
void mov(OPERAND* op1, void* op2, DATA* registers);
void cmp(OPERAND* op1, void* op2, DATA* registers);
void jmp(OPERAND* op1, void* pc, DATA* registers);
void brn(OPERAND* op1, void* pc, DATA* registers);
DATA value_by_type(OPERAND* operand, DATA* registers);

#endif // !__OPERATOR_H__