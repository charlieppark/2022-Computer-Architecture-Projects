#include "components.h"

// adder
int adder(int a, int b) { return a + b; }

// alu (w/t control signal)
int alu(int a, int b, int* zero, int opcode, int funct) {
    int alu_result = 0;

    switch (opcode) {
        case 0:
            switch(funct) {
                case 0x20:
                case 0x21:
                    alu_result = ADD(a, b, zero);
                    break;
                case 0x24:
                    alu_result = AND(a, b, zero);
                    break;
                case 0x27:
                    alu_result = OR(a, b, zero);
                    break;
                case 0x25:
                    alu_result = !OR(a, b, zero);
                    break;
                case 0x2a:
                case 0x2b:
                    alu_result = SLT(a, b, zero);
                    break;
                case 0x00:
                    alu_result = SLL(a, b, zero);
                    break;
                case 0x22:
                case 0x23:
                    alu_result = SUB(a, b, zero);
                    break;
                case 0x8:
                    break;
                default:
#ifdef __DEBUG_MODE__
                    printf("unavailable funct : %x", funct);
                    exit(EXIT_FAILURE);
#endif // __DEBUG_MODE__
                    break;
            }
            break;
        case 0x08:
        case 0x09:
        case 0x23:
        case 0x2b:
        case 0xf:
            alu_result = ADD(a, b, zero);
            break;
        case 0x0c:
            alu_result = AND(a, b, zero);
            break;
        case 0x04:
            alu_result = BEQ(a, b, zero);
            break;
        case 0x05:
            alu_result = BNE(a, b, zero);
            break;
        case 0x03:
            alu_result = 31;
            break;
        case 0x0d:
            alu_result = OR(a, b, zero);
            break;
        case 0x0a:
        case 0x0b:
            alu_result = SLT(a, b, zero);
            break;
        case 0x2:
            break;
        default:
#ifdef __DEBUG_MODE__
            printf("unavailable opcode : %x", opcode);
            exit(EXIT_FAILURE);
#endif // __DEBUG_MODE__
            break;
    }

    return alu_result;
}

// and
bool and_gate(bool a, bool b) {
    return a && b;
}

void set_control_line(void** ret_array, bool reg_dst, bool alu_src, bool branch, bool mem_read, bool mem_write, bool reg_write, bool mem_to_reg) {
    ((EX_control_line*)(ret_array[0]))->reg_dst = reg_dst;
    ((EX_control_line*)(ret_array[0]))->alu_src = alu_src;

    ((M_control_line*)(ret_array[1]))->branch = branch;
    ((M_control_line*)(ret_array[1]))->mem_read = mem_read;
    ((M_control_line*)(ret_array[1]))->mem_write = mem_write;

    ((WB_control_line*)(ret_array[2]))->reg_write = reg_write;
    ((WB_control_line*)(ret_array[2]))->mem_to_reg = mem_to_reg;
}

void** control_unit(int opcode) {
    WB_control_line* wb = malloc_s(sizeof(*wb));

    M_control_line* m = malloc_s(sizeof(*m));

    EX_control_line* ex = malloc_s(sizeof(*ex));

    void* ret_array[3] = { (void*)ex, (void*)m, (void*)wb };
    
    switch (opcode) {
    case 0:
        set_control_line(ret_array, true, false, false, false, false, true, false);
        break;
    case 0x23:
        set_control_line(ret_array, false, true, false, true, false, true, true);
        break;
    case 0x2b:
        set_control_line(ret_array, false, true, false, false, true, false, false);
        break;
    case 0x4:
    case 0x5:
        set_control_line(ret_array, false, false, true, false, false, false, false);
        break;
    case 0x2:
        set_control_line(ret_array, false, false, false, false, false, false, false);
        break;
    case 0x3:
        set_control_line(ret_array, false, false, false, false, false, true, false);
        break;
    case 0xa:
    case 0xb:
        set_control_line(ret_array, false, true, false, false, false, true, false);
        break;
    default:
        set_control_line(ret_array, false, true, false, false, false, true, false);
        break;
    }

    return ret_array;
}

// data memory
int data_memory (int address, int write_data, bool MemWrite, bool MemRead, char* memory) {
    int read_data = 0;

    int* target = (int*)(memory + address);

    if (MemWrite) {
        *target = write_data;
    } else if (MemRead) {
        read_data = *target;
    }

    return read_data;
}

int* initialize_btb(void) {
    int* const btb = malloc_s(100000 * sizeof(*btb));

    if (memset(btb, -1, 100000 * sizeof(*btb)) == NULL) {
        fprintf(stderr, "MEMSET FAILED");
        exit(EXIT_FAILURE);
    }

    return btb;
}

int* initialize_memory(void) {
    int* const memory = malloc_s(100000000 * sizeof(*memory));

    if (memset(memory, -1, 100000000 * sizeof(*memory)) == NULL) {
        fprintf(stderr, "MEMSET FAILED");
        exit(EXIT_FAILURE);
    }

    return memory;
}

unsigned int* initialize_registers(void) {

    unsigned int* const registers = malloc_s(sizeof(*registers) * NUM_OF_REG);

    for (int i = 0; i < NUM_OF_REG; i++) {
        if (i == LR)
            registers[i] = LRvalue;
        else if (i == SP)
            registers[i] = SPvalue;
        else
            registers[i] = 0;
    }

    return registers;
}

void latch_update(IF_ID* if_id_latch[], ID_EX* id_ex_latch[], EX_MEM* ex_mem_latch[], MEM_WB* mem_wb_latch[]) {
    if (if_id_latch[0] != NULL) {
        free(if_id_latch[1]);
        if_id_latch[1] = if_id_latch[0];
    }

    if (id_ex_latch[0] != NULL) {
        if (id_ex_latch[1]->valid == true) free(id_ex_latch[1]->ex);
        free(id_ex_latch[1]);
        id_ex_latch[1] = id_ex_latch[0];
    }

    if (ex_mem_latch[0] != NULL) {
        if (ex_mem_latch[1]->valid == true) free(ex_mem_latch[1]->m);
        free(ex_mem_latch[1]);
        ex_mem_latch[1] = ex_mem_latch[0];
    }

    if (mem_wb_latch[0] != NULL) {
        if (mem_wb_latch[1]->valid == true) free(mem_wb_latch[1]->wb);
        free(mem_wb_latch[1]);
        mem_wb_latch[1] = mem_wb_latch[0];
    }

    return;
}

// mux
int mux(bool signal, int if_false, int if_true) {
    int return_value = signal ? if_true : if_false;

    return return_value;
}

// registers
int* operate_registers(int* registers, bool RegWrite, int read_register_1, int read_register_2, int write_register, int write_data) {
    if (RegWrite) {

        registers[write_register] = write_data;
        
        return NULL;
    } else {
    
        int* read_data = malloc_s(sizeof(*read_data) * 2);

        read_data[0] = registers[read_register_1];
        read_data[1] = registers[read_register_2];

        return read_data;
    }
}

// instruction memory
int read_instruction(int address, char* instruction_memory) {
    int* target = (int*)(instruction_memory + address);

    return *target;
}

// shift-left-2
int shitf_left_two(int a) {
    return a << 2;
}

int sign_extend(short instruction_15_0) {
    return (int)instruction_15_0;
}