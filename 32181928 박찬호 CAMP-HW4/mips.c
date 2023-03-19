#include "mips.h"
#include "predictor.h"

int inst_hit = 0;
int data_hit = 0;
int num_lw = 0;
int num_sw = 0;

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

int cache_shift_length[] = {
    13, 6
};

int cache_mask[] = {
    0b11111111111111111110000000000000,
    0b00000000000000000001111111000000,
    0b00000000000000000000000000111111
};

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

int find_opcode_index(int opcode) {
    for (int i = 0; i < NUM_OF_OP; i++) {
        if (op_dict_keys[i] == opcode) return i;
    }

    return -1;
}

char opcode_to_inst_type(int opcode) {
    int index;

    if ((index = find_opcode_index(opcode)) != -1) return op_dict_values[index];
}

int jump_address(int input) {
    return input & (mask[1] + mask[2] + mask[3] + mask[4] + mask[5]);
}

IF_ID* fetch(int* pc_ptr, int add_result, bool* PCSrc, int* memory, int* count, int* btb, CacheLine* cache) {

    if (*pc_ptr == -1) return NULL;

    int pc = mux(*PCSrc, *pc_ptr, add_result);

    if (pc == -1) {
        *pc_ptr = -1;
        return NULL;
    }

    int instruction;

    /*
    * 캐시가 초기화 되었다면 -> valid 0
    * 
    * if invalid -> 읽고 채운다.
    * valid = 1;
    * 
    * 
    * if valid
    * 
    * hit or miss
    * hit
    * 읽고 sca = 1을 수행한다.
    * 
    * miss
    * 만약 sca가 0이라면 > 메모리에서 값을 읽어들이고, 캐시에 업데이트 한다.
    * 만약 sca가 1이라면 > sca를 0으로 바꾸고, 메모리에서 값을 읽어들인다.
    */


    int tag = (pc & cache_mask[0]) >> cache_shift_length[0];
    int index = (pc & cache_mask[1]) >> cache_shift_length[1];
    int offset = pc & cache_mask[2];
    

    if (!cache[index].valid) {
        instruction = read_instruction(pc, (char*)memory);

        for (int offset = 0; offset < 64; offset += 4) {
            int cache_pc = (tag << cache_shift_length[0]) | (index << cache_shift_length[1]) | offset;
            cache[index].data[offset >> 2] = read_instruction(cache_pc, memory);
        }
        cache[index].tag = tag;
        cache[index].valid = 1;
    } else {
        bool hit = (cache[index].tag == tag);

        if (hit) {
            instruction = cache[index].data[offset >> 2];
            cache[index].sca = 1;
            inst_hit++;
        } else {
            instruction = read_instruction(pc, (char*)memory);

            if (cache[index].sca == 0) {
                for (int offset = 0; offset < 64; offset += 4) {
                    int cache_pc = (tag << cache_shift_length[0]) | (index << cache_shift_length[1]) | offset;
                    cache[index].data[offset >> 2] = read_instruction(cache_pc, memory);
                }
                cache[index].tag = tag;
            } else {
                cache[index].sca = 0;
            }
        }
    }

    
    IF_ID* if_id = malloc_s(sizeof(*if_id));


    //bool is_saved_branch = (btb[pc] != -1);

    //int new_pc_plus_four = *pc_ptr = 
        //(is_saved_branch && one_bit_last()) ? btb[pc] : pc + 4;

    int new_pc_plus_four = pc + 4;

    *pc_ptr = new_pc_plus_four;


    if_id->pc = pc;
    if_id->pc_plus_four = new_pc_plus_four;
    if_id->instruction = instruction;
    if_id->valid = true;

    if (instruction == 0) {
        if_id->valid = false;
    }

    if (*PCSrc) *PCSrc = false;

    return if_id;
}

ID_EX* decode(IF_ID* if_id, int* registers) {
    if (if_id == NULL) return NULL;


    if (if_id->valid == false) {
        ID_EX* id_ex = malloc_s(sizeof(*id_ex));

        id_ex->valid = false;

        return id_ex;
    }

    int opcode = find_opcode(if_id->instruction);

    void** control_lines = control_unit(opcode);

    EX_control_line* ex = (EX_control_line*)control_lines[0];

    M_control_line* m = (M_control_line*)control_lines[1];

    WB_control_line* wb = (WB_control_line*)control_lines[2];

    int read_register_1 = (if_id->instruction & mask[1]) >> shift_length[1];
    int read_register_2 = (if_id->instruction & mask[2]) >> shift_length[2];

    int* read_data = operate_registers(registers, false, read_register_1, read_register_2, -1, -1);

    short imm = (short)(if_id->instruction & (mask[3] + mask[4] + mask[5]));

    int sign_ext_imm = sign_extend(imm);

    int zero_ext_imm = ZERO_IMMIDIATE_FLAG & sign_ext_imm;

    if ((opcode == 0xc) || (opcode == 0xd)) sign_ext_imm = zero_ext_imm;
    if (opcode == 0xf) sign_ext_imm = (sign_ext_imm << 16);

    int inst_20_16 = (if_id->instruction & mask[2]) >> shift_length[2];

    int inst_15_11 = (if_id->instruction & mask[3]) >> shift_length[3];

    int funct = sign_ext_imm & mask[5];

    ID_EX* id_ex = malloc_s(sizeof(*id_ex));

    id_ex->wb = wb;
    id_ex->m = m;
    id_ex->ex = ex;
    id_ex->pc_plus_four = if_id->pc_plus_four;
    id_ex->read_data = read_data;
    id_ex->sign_ext_imm = sign_ext_imm;
    id_ex->inst_20_16 = inst_20_16;
    id_ex->inst_15_11 = inst_15_11;
    id_ex->opcode = opcode;
    id_ex->rs = read_register_1;
    id_ex->rt = read_register_2;
    id_ex->instruction = if_id->instruction;
    id_ex->pc = if_id->pc;
    id_ex->funct = funct;

    id_ex->valid = true;

    return id_ex;
}

EX_MEM* execute(ID_EX* id_ex, ForwardLine* line, ForwardInput* input, int* const btb) {
    if (id_ex == NULL) return NULL;

    if (id_ex->valid == false) {

        EX_MEM* ex_mem = malloc_s(sizeof(*ex_mem));

        ex_mem->valid = false;

        return ex_mem;
    }

    int sign_ext_imm_sl2 = shitf_left_two(id_ex->sign_ext_imm);

    int add_result = adder(id_ex->pc_plus_four, sign_ext_imm_sl2);

    //if (btb[id_ex->pc] == 1)
    //    btb[id_ex->pc] = add_result;

    bool zero = false;

    int funct = id_ex->sign_ext_imm & mask[5];

    int alu_input_a = id_ex->read_data[0];

    int b_mux_input = id_ex->read_data[1];

    int read_data_two = id_ex->read_data[1];
    
    if (line->ForwardA_bit_1 && !line->ForwardA_bit_0) { //10
        alu_input_a = input->pre_alu_result;
    } else if (!line->ForwardA_bit_1 && line->ForwardA_bit_0) { //01
        alu_input_a = input->write_data;
    }

    if (line->ForwardB_bit_1 && !line->ForwardB_bit_0) { //10
        b_mux_input = input->pre_alu_result;
        read_data_two = b_mux_input;
    } else if (!line->ForwardB_bit_1 && line->ForwardB_bit_0) { //01
        b_mux_input = input->write_data;
        read_data_two = b_mux_input;
    }

    if ((id_ex->opcode == 0) && (funct == 0)) {
        int shamt = (id_ex->sign_ext_imm & mask[4]) >> shift_length[4];
        alu_input_a = shamt;
    }

    int alu_input_b = mux(id_ex->ex->alu_src, b_mux_input, id_ex->sign_ext_imm);

    int alu_result = alu(alu_input_a, alu_input_b, &zero, id_ex->opcode, funct);

    int RegDst = mux(id_ex->ex->reg_dst, id_ex->inst_20_16, id_ex->inst_15_11);
    
    if (id_ex->opcode == 3) {
        RegDst = 31;
        alu_result = id_ex->pc_plus_four + 4;
    } else if ((id_ex->opcode == 0) && (funct == 8)) {
        add_result = id_ex->read_data[0];
        zero = true;
        id_ex->m->branch = true;
    }
    
    EX_MEM* ex_mem = malloc_s(sizeof(*ex_mem));

    //if ((id_ex->opcode == 0) && (funct == 8)) {
        //add_result = alu_input_a;
    //}



    ex_mem->wb = id_ex->wb;
    ex_mem->m = id_ex->m;
    ex_mem->add_result = add_result;
    ex_mem->zero = zero;
    ex_mem->alu_result = alu_result;
    ex_mem->read_data_two = read_data_two;
    ex_mem->reg_dst_result = RegDst;
    ex_mem->pc = id_ex->pc;
    ex_mem->valid = true;

    free(id_ex->read_data);

    return ex_mem;
}

MEM_WB* mem_write(EX_MEM* ex_mem, bool* PCSrc, int* memory, int* add_result, int* pre_alu_result, ForwardParameter* forward_param, int* btb, CacheLine* cache) {
    if (ex_mem == NULL) return NULL;

    if (ex_mem->valid == false) {
        MEM_WB* mem_wb = malloc_s(sizeof(*mem_wb));

        mem_wb->valid = false;

        return mem_wb;
    }

    *add_result = ex_mem->add_result;
    *pre_alu_result = ex_mem->alu_result;

    forward_param->ex_mem_rd = ex_mem->reg_dst_result;
    forward_param->ex_mem_regwrite = ex_mem->wb->reg_write;

    //bool is_saved_branch = (btb[ex_mem->pc] != -1);

    bool branch = and_gate(ex_mem->m->branch, ex_mem->zero);

    //if (!is_saved_branch) 
    *PCSrc = branch;



    int address = ex_mem->alu_result;

    int tag = (address & cache_mask[0]) >> cache_shift_length[0];
    int index = (address & cache_mask[1]) >> cache_shift_length[1];
    int offset = address & cache_mask[2];


    int read_data = 0;

    bool mem_write = ex_mem->m->mem_write;

    bool mem_read = ex_mem->m->mem_read;

    if (mem_read) {
        num_lw++;
        if (!cache[index].valid) {
            read_data = data_memory(address, ex_mem->read_data_two, mem_write, mem_read, (char*)memory);

            for (int offset = 0; offset < 64; offset += 4) {
                int cache_address = (tag << cache_shift_length[0]) | (index << cache_shift_length[1]) | offset;
                cache[index].data[offset >> 2] = data_memory(cache_address, ex_mem->read_data_two, mem_write, mem_read, (char*)memory);
            }
            cache[index].tag = tag;
            cache[index].valid = 1;
        } else {
            bool hit = (cache[index].tag == tag);

            if (hit) {
                read_data = cache[index].data[offset >> 2];
                cache[index].sca = 1;
                data_hit++;
            } else {
                read_data = data_memory(address, ex_mem->read_data_two, mem_write, mem_read, (char*)memory);

                if (cache[index].sca == 0) {
                    for (int offset = 0; offset < 64; offset += 4) {
                        int cache_address = (tag << cache_shift_length[0]) | (index << cache_shift_length[1]) | offset;
                        cache[index].data[offset >> 2] = data_memory(cache_address, ex_mem->read_data_two, mem_write, mem_read, (char*)memory);
                    }
                } else {
                    cache[index].sca = 0;
                }
            }
        }
    }

    if (mem_write) {
        read_data = data_memory(address, ex_mem->read_data_two, mem_write, mem_read, (char*)memory);
        num_sw++;

        for (int offset = 0; offset < 64; offset += 4) {
            int cache_address = (tag << cache_shift_length[0]) | (index << cache_shift_length[1]) | offset;
            cache[index].data[offset >> 2] = data_memory(cache_address, ex_mem->read_data_two, false, true, (char*)memory);
        }
    }



    MEM_WB* mem_wb = malloc_s(sizeof(*mem_wb));

    mem_wb->wb = ex_mem->wb;
    mem_wb->read_data = read_data;
    mem_wb->alu_result = ex_mem->alu_result;
    mem_wb->reg_dst_result = ex_mem->reg_dst_result;
    
    mem_wb->valid = true;


    return mem_wb;
}
 
int write_back(MEM_WB* mem_wb, int* registers, ForwardParameter* forward_param) {
    if ((mem_wb == NULL) || (mem_wb->valid == false)) return NULL;

    forward_param->mem_wb_rd = mem_wb->reg_dst_result;
    forward_param->mem_wb_regwrite = mem_wb->wb->reg_write;

    int write_data = mux(mem_wb->wb->mem_to_reg, mem_wb->alu_result, mem_wb->read_data);

    int write_register = mem_wb->reg_dst_result;

    operate_registers(registers, mem_wb->wb->reg_write, -1, -1, write_register, write_data);

    return write_data;
}


int find_opcode(int instruction) {
    return ((instruction & mask[0]) >> shift_length[0]) & mask[5];
}

ForwardLine* forward(ForwardParameter* forward_param, ForwardLine* line) {
    line->ForwardA_bit_0 = false;
    line->ForwardA_bit_1 = false;
    line->ForwardB_bit_0 = false;
    line->ForwardB_bit_1 = false;

    if ((forward_param->id_ex_rs == -1) ||
        (forward_param->id_ex_rt == -1) ||
        (forward_param->ex_mem_rd == -1 && forward_param->mem_wb_rd == -1))
        return line;
    
    if ((forward_param->ex_mem_regwrite) &&
        (forward_param->ex_mem_rd != 0)) {
        if (forward_param->ex_mem_rd == forward_param->id_ex_rs) {
            line->ForwardA_bit_1 = true;
            line->ForwardA_bit_0 = false;
        }

        if (forward_param->ex_mem_rd == forward_param->id_ex_rt) {
            line->ForwardB_bit_1 = true;
            line->ForwardB_bit_0 = false;
        }
    }

    if (forward_param->mem_wb_rd == -1) return line;

    if ((forward_param->mem_wb_regwrite) &&
        (forward_param->mem_wb_rd != 0)) {
        if (!(forward_param->ex_mem_regwrite && 
            (forward_param->ex_mem_rd != 0) && 
            (forward_param->ex_mem_rd == forward_param->id_ex_rs)
            ) && (forward_param->mem_wb_rd == forward_param->id_ex_rs)) {
            line->ForwardA_bit_1 = false;
            line->ForwardA_bit_0 = true;
        }

        if (!(forward_param->ex_mem_regwrite &&
            (forward_param->ex_mem_rd != 0) &&
            (forward_param->ex_mem_rd == forward_param->id_ex_rt)
            ) && (forward_param->mem_wb_rd == forward_param->id_ex_rt)) {
            line->ForwardB_bit_1 = false;
            line->ForwardB_bit_0 = true;
        }
    }

    return line;
}