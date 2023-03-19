#ifndef __MIPS_H__

#define __MIPS_H__

#include "base_header.h"

#include "components.h"

#define NUM_OF_OP 20

#define NUM_OF_FUNCT 12

#define JUMP_ADDRESS_FLAG ((int)0b11110000000000000000000000000000)

#define UPPER_IMM_FLAG ((int)0b11111111111111110000000000000000)

#define ZERO_IMMIDIATE_FLAG ((int)0b00000000000000001111111111111111)

int inst_hit;
int data_hit;

typedef struct {
    bool ForwardA_bit_0;
    bool ForwardA_bit_1;
    bool ForwardB_bit_0;
    bool ForwardB_bit_1;
} ForwardLine;

typedef struct {
    int id_ex_rs;

    int id_ex_rt;

    int ex_mem_rd;

    bool ex_mem_regwrite;

    int mem_wb_rd;

    bool mem_wb_regwrite;

} ForwardParameter;

typedef struct {
    int pre_alu_result;
    int write_data;
} ForwardInput;

typedef struct {
    unsigned int tag : 19; // among 32 bit, use only a few bits
    unsigned int sca : 1;
    unsigned int valid : 1;
    unsigned int dirty : 1;
    int data[16];
} CacheLine;

int find_opcode_index(int opcode);

IF_ID* fetch(int* pc_ptr, int add_result, bool* PCSrc, int* memory, int* count, int* btb, CacheLine* cache);

ID_EX* decode(IF_ID* if_id, int* registers);

EX_MEM* execute(ID_EX* id_ex, ForwardLine* line, ForwardInput* input, int* const branch_target_buffer);

MEM_WB* mem_write(
    EX_MEM* ex_mem,
    bool* PCSrc,
    int* memory, 
    int* add_result, 
    int* pre_alu_result, 
    ForwardParameter* forward_param, 
    int* btb,
    CacheLine* cache);

int write_back(MEM_WB* mem_wb, int* registers, ForwardParameter* forward_param);

int find_opcode(int instruction);

char opcode_to_inst_type(int opcode);

ForwardLine* forward(ForwardParameter* forward_param, ForwardLine* line);

#endif // !__MIPS_H__