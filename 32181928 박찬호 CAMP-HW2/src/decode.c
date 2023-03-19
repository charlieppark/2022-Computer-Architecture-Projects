#include "decode.h"

#include <stdio.h>
#include <stdlib.h>

#include "mips.h"

int shift_length[] = {
    26, 21, 16, 11, 6
};

int mask[] = {
    0b11111100000000000000000000000000,
    0b00000011111000000000000000000000,
    0b00000000000111110000000000000000,
    0b00000000000000001111100000000000,
    0b00000000000000000000011111000000,
    0b00000000000000000000000000111111
    };

INSTRUCTION* decode(int input, int* opcode_return) {

    INSTRUCTION* instruction = malloc(sizeof(*instruction));

    int opcode = ((input & mask[0]) >> shift_length[0]) & mask[5];

    *opcode_return = opcode;

    int instruction_type = opcode_to_inst_type(opcode);

    switch (instruction_type)
    {
    case 'R':
        instruction->r.opcode   = opcode;
        instruction->r.rs       = (input & mask[1]) >> shift_length[1];
        instruction->r.rt       = (input & mask[2]) >> shift_length[2];
        instruction->r.rd       = (input & mask[3]) >> shift_length[3];
        instruction->r.shamt    = (input & mask[4]) >> shift_length[4];
        instruction->r.funct    = (input & mask[5]);

        #ifdef __DEBUG_MODE__
        printf("%#x %#x %#x %#x %#x %#x\n", instruction->r.opcode, instruction->r.rs, instruction->r.rt, instruction->r.rd, instruction->r.shamt, instruction->r.funct);
        #endif

        break;
    
    case 'I':
        instruction->i.opcode   = opcode;
        instruction->i.rs       = (input & mask[1]) >> shift_length[1];
        instruction->i.rt       = (input & mask[2]) >> shift_length[2];
        instruction->i.imm      = input & (mask[3] + mask[4] + mask[5]);

        #ifdef __DEBUG_MODE__
        printf("%#x %#x %#x %#x\n", instruction->i.opcode, instruction->i.rs, instruction->i.rt, instruction->i.imm);
        #endif

        break;
    
    case 'J':
        instruction->j.opcode   = opcode;
        instruction->j.address  = input & (mask[1] + mask[2] + mask[3] + mask[4] + mask[5]);

        #ifdef __DEBUG_MODE__
        printf("%#x %#x\n", instruction->j.opcode, instruction->j.address);
        #endif

        break;

    default:
        break;
    }

    return instruction;
}
