#ifndef __EXECUTE_H__

#define __EXECUTE_H__

#include "instruction.h"

void execute(INSTRUCTION* instruction, int registers[], int* pc, int memory[], int opcode);

void ADDIU  (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void ADDU   (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void BEQ    (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void BNE    (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void JUMP   (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void JAL    (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void JR     (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void LUI    (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void LW     (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void ORI    (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void SLT    (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void SLTI   (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void SLL    (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void SW     (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);
void SUBU   (INSTRUCTION* instruction, int registers[], int* pc, int memory[]);

#define JUMP_ADDRESS_FLAG ((int)0b11110000000000000000000000000000)
#define ZERO_IMMIDIATE_FLAG 0b1111111111111111

#endif