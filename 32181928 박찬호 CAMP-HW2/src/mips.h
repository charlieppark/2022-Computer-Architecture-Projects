#ifndef __MIPS_H__

#define __MIPS_H__

#include "mode.h"

#define NUM_OF_OP 20

#define NUM_OF_FUNCT 12

#define NUM_OF_REG 32

#define LR 31

#define LRvalue 0xFFFFFFFF

#define SP 29

#define SPvalue 0x1000000

int find_opcode_index (int opcode);

int find_funct_index (int funct);

int opcode_to_inst_type (int opcode);

void initialize_registers(int registers[]);

#endif // !__MIPS_H__