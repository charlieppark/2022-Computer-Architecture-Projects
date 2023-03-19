#ifndef __INSTRUCTION_H__

#define __INSTRUCTION_H__

typedef struct {
    unsigned int opcode : 6;
    unsigned int rs     : 5;
    unsigned int rt     : 5;
    unsigned int rd     : 5;
    unsigned int shamt  : 5;
    unsigned int funct  : 6;
} R_INSTRUCTION;

typedef struct {
    unsigned int opcode : 6;
    unsigned int rs     : 5;
    unsigned int rt     : 5;
             int imm    : 16;
} I_INSTRUCTION;

typedef struct {
    unsigned int opcode : 6;
    unsigned int address: 26;
} J_INSTRUCTION;

typedef union {
    R_INSTRUCTION r;
    I_INSTRUCTION i;
    J_INSTRUCTION j;
} INSTRUCTION;

#endif // !__INSTRUCTION_H__