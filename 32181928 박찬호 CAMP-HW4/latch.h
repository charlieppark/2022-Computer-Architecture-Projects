#ifndef __LATCH_H__
#define __LATCH_H__

#include "control_line.h"

// IF/ID
typedef struct {
    int pc;

    int pc_plus_four;

    int instruction;

    bool valid;
} IF_ID;


// ID/EX
typedef struct {
    WB_control_line* wb;

    M_control_line* m;

    EX_control_line* ex;

    int pc_plus_four;

    int* read_data;

    int sign_ext_imm;

    int inst_20_16;

    int inst_15_11;

    int opcode;

    int rs;

    int rt;

    int instruction;

    int pc;

    int funct;

    bool valid;
} ID_EX;



// EX/MEM
typedef struct {
    WB_control_line* wb;

    M_control_line* m;

    int add_result;

    bool zero;

    int alu_result;

    int read_data_two;

    int reg_dst_result;

    int pc;

    bool valid;
} EX_MEM;



// MEM/WB
typedef struct {
    WB_control_line* wb;

    int read_data;

    int alu_result;

    int reg_dst_result;

    bool valid;
} MEM_WB;

#endif // !__LATCH_H__
