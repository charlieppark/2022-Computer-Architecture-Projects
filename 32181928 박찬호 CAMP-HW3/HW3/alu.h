#ifndef __ALU_H__

#define __ALU_H__

#include <stdbool.h>

#define ADD(a, b, zero) (a + b);
#define AND(a, b, zero) (a & b);
#define SUB(a, b, zero) (a - b);
#define OR(a, b, zero) (a | b);
#define SLT(a, b, zero) ((a < b) ? 1 : 0);
#define SLL(a, b, zero) (b << a);

int BEQ (int a, int b, bool* zero);
int BNE(int a, int b, bool* zero);

#endif