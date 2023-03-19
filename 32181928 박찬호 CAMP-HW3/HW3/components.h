#ifndef __COMP_H__
#define __COMP_H__

#include <stdbool.h>Z

#include "latch.h"
#include "alu.h"

#include "components.h"
#include "base_header.h"

#define NUM_OF_REG 32

#define LR 31

#define LRvalue 0xFFFFFFFF

#define SP 29

#define SPvalue 0x1000000

int* initialize_btb(void);

unsigned int* initialize_registers(void);

int* initialize_memory(void);

int adder(int a, int b);

int alu(int a, int b, int* zero, int opcode, int funct);

bool and_gate(bool a, bool b);

void** control_unit(int opcode);

int data_memory (int address, int write_data, bool MemWrite, bool MemRead, int* memory);

int mux(bool signal, int if_false, int if_true);

int* operate_registers(int* registers, bool RegWrite, int read_register_1, int read_register_2, int write_register, int write_data);

int read_instruction(int address, int* instruction_memory);

int shitf_left_two(int a);

int sign_extend(short instruction_15_0);

void latch_update(IF_ID* if_id_latch[], ID_EX* id_ex_latch[], EX_MEM* ex_mem_latch[], MEM_WB* mem_wb_latch[]);

#endif // !__COMP_H__