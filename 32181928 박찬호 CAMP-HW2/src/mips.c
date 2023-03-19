#include "mips.h"

int op_dict_keys[] = {
    0x0, 0x2, 0x3, 0x4, 0x5, 0x8, 0x9, 0xa,
    0xb, 0xc, 0xd, 0xf, 0x23, 0x24, 0x25, 0x28,
    0x29, 0x2b, 0x30, 0x38
};

char op_dict_values[] = {
    'R', 'J', 'J', 'I', 'I', 'I', 'I', 'I',
    'I', 'I', 'I', 'I', 'I', 'I', 'I', 'I',
    'I', 'I', 'I', 'I'
};

int funct_keys[] = {
    0x0,    0x2,    0x8,    0x20,   0x21,   0x22,
    0x23,   0x24,   0x25,   0x27,   0x2a,   0x2b
};

int find_opcode_index(int opcode) {
    for (int i = 0; i < NUM_OF_OP; i++) {
        if (op_dict_keys[i] == opcode) return i;
    }

    return -1;
}

int find_funct_index(int funct) {
    for (int i = 0; i < NUM_OF_FUNCT; i++) {
        if (funct_keys[i] == funct) return i;
    }

    return -1;
}

int opcode_to_inst_type(int opcode) {
    int index;

    if ((index = find_opcode_index(opcode)) != -1) return op_dict_values[index];
}

void initialize_registers(int registers[]) {

    for (int i = 0; i < NUM_OF_REG; i++) {
        if (i == LR)
            registers[i] = LRvalue;
        else if (i == SP)
            registers[i] = SPvalue;
        else
            registers[i] = 0;
    }
}