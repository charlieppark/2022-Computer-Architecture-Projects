#include "execute.h"

#include <stdio.h>
#include <stdlib.h>

#include "mips.h"

#include "mode.h"

void execute_r(INSTRUCTION* inst, int registers[], int* pc, int memory[]);

void NULLF(INSTRUCTION* inst, int registers[], int* pc, int memory[]);

void (*op_inst[NUM_OF_OP])(INSTRUCTION*, int*, int*, int*) = {
    &execute_r, &JUMP,  &JAL,   &BEQ,   &BNE,   &NULLF, &ADDIU, &SLTI,
    &NULLF,     &NULLF, &ORI,   &LUI,   &LW,    &NULLF, &NULLF, &NULLF,
    &NULLF,     &SW,    &NULLF, &NULLF
};

void (*funct_inst[NUM_OF_FUNCT])(INSTRUCTION*, int*, int*, int*) = {
    &SLL,   &NULLF, &JR,    &NULLF, &ADDU,  &NULLF,
    &SUBU,  &NULLF, &NULLF, &NULLF, &SLT,   &NULLF
};


void execute(INSTRUCTION* inst, int registers[], int* pc, int memory[], int opcode) {

    int key = find_opcode_index(opcode);

    if (key == -1) {
        printf("ERROR with opcode : %#x\n", opcode);
        printf("%d", *pc);
    }

    op_inst[key](inst, registers, pc, memory);
}

void NULLF(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {

#ifdef __DEBUG_MODE__
    printf("\nUNIMPLEMENTED");
    printf("\n%d", inst->i.opcode);
#endif

    exit(1);
}

void execute_r(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    int key = find_funct_index(inst->r.funct);

    if (key == -1) {
        printf("ERROR with funct : %#x\n", inst->r.funct);
        printf("%d", *pc);
    }
    
    funct_inst[key](inst, registers, pc, memory);
}

void ADDIU(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    registers[inst->i.rt] = registers[inst->i.rs] + inst->i.imm;
    return;
}

void ADDU(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    registers[inst->r.rd] = registers[inst->r.rs] + registers[inst->r.rt];
    return;
}

void BEQ(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    if (registers[inst->i.rs] == registers[inst->i.rt]) *pc = *pc + 4 + (inst->i.imm << 2);
    return;
}

void BNE(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    if (registers[inst->i.rs] != registers[inst->i.rt]) *pc = *pc + 4 + (inst->i.imm << 2);
    return;
}

void JUMP(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    int address = ((JUMP_ADDRESS_FLAG) & (*pc + 4)) + (inst->j.address << 2);
    *pc = address;
    return;
}

void JAL(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    int address = ((JUMP_ADDRESS_FLAG) & (*pc + 4)) + (inst->j.address << 2);

    registers[31] = *pc + 8;

    *pc = address;

    return;
}

void JR(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    *pc = registers[inst->r.rs];
    return;
}

void LUI(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    registers[inst->i.rt] = inst->i.imm << 16;
    return;
}

void LW(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    registers[inst->i.rt] = memory[registers[inst->i.rs] + (int)inst->i.imm];
    return;
}

void ORI(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    registers[inst->i.rt] = registers[inst->i.rs] | (inst->i.imm & ZERO_IMMIDIATE_FLAG);
    return;
}

void SLT(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    registers[inst->r.rd] = (registers[inst->r.rs] < registers[inst->r.rt]) ? 1 : 0;
    return;
}

void SLTI(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    registers[inst->i.rt] = (registers[inst->i.rs] < inst->i.imm) ? 1 : 0;
    return;
}

void SLL(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    registers[inst->r.rd] = registers[inst->r.rt] << inst->r.shamt;
    return;
}

void SW(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    memory[registers[inst->i.rs] + (int)inst->i.imm] = registers[inst->i.rt];
    return;
}

void SUBU(INSTRUCTION* inst, int registers[], int* pc, int memory[]) {
    registers[inst->r.rd] = registers[inst->r.rs] - registers[inst->r.rt];
    return;
}