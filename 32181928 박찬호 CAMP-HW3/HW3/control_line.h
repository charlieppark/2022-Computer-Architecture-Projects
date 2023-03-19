#ifndef __CONTLINE_H__
#define __CONTLINE_H__

// WB
typedef struct {
    bool reg_write;

    bool mem_to_reg;
} WB_control_line;

// M
typedef struct {
    bool branch;

    bool mem_read;

    bool mem_write;

} M_control_line;

// EX
typedef struct {
    bool reg_dst;

    bool alu_src;
} EX_control_line;

#endif // !__CONTLINE_H__
