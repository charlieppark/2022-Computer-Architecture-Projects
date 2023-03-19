#include <stdbool.h>

#include "base_header.h"
#include "mips.h"
#include "instruction.h"
#include "components.h"
#include "predictor.h"

#define ALWAYS_NOT_TAKEN    0
#define ALWAYS_TAKEN        1
#define BACK_T_FOR_NT       2
#define ONE_BIT_LAST        3
#define TWO_BITS_LAST       4
#define TWO_LEVEL_GLOBAL    5
#define LOCAL_BRANCH_CORR   6
#define HYBRID_BRANCH       7

static void view_registers(int* registers);

static FILE* initialize_file(char* file_name, int* memory, int* num_of_file_lines);

static void count_opcode_type(int opcode, int* num_I, int* num_J, int* num_R);


int main(int argc, char* argv[]) {

    char* file_name_list[7] = {
        "./test_prog/simple.bin",
        "./test_prog/simple2.bin",
        "./test_prog/simple3.bin",
        "./test_prog/simple4.bin",
        "./test_prog/gcd.bin",
        "./test_prog/fib.bin",
        "./test_prog/input4.bin"
    };

    //int branch_mode = -1;

    //for (int i = 1; i < argc; i++) {
    //    if (strcmp(argv[i], "--branch") == 0) {

    //        if (argc <= i + 1) {
    //            fprintf(stderr, "ARGUMENTS ERROR");
    //            exit(EXIT_FAILURE);
    //        }

    //        i++;

    //        branch_mode = atoi(argv[i]);
    //        continue;
    //    }

    //    if (strcmp(argv[i], "--reg") == 0) {
    //        if (argc <= i + 2) {
    //            fprintf(stderr, "ARGUMENTS ERROR");
    //            exit(EXIT_FAILURE);
    //        }

    //        int A = atoi(argv[i + 1]);

    //        if ((A > 9) || (A < 0)) {
    //            fprintf(stderr, "ARGUMENTS ERROR");
    //            exit(EXIT_FAILURE);
    //        }

    //        registers[A] = atoi(argv[i + 2]);

    //        i += 2;
    //        continue;
    //    }

    //    file_name = argv[i];
    //}

    for (int a = 0; a < 7; a++) {

        char* file_name = file_name_list[a];

        unsigned int* const registers = initialize_registers();

        int num_of_instructions = 0, num_of_file_lines = 0;

        int* const memory = initialize_memory();

        FILE* fp = initialize_file(file_name, memory, &num_of_file_lines);

        //int* const btb = initialize_btb();

        int* checked_pc_list = malloc_s(sizeof(*checked_pc_list) * (num_of_file_lines + 100));

        if (memset(checked_pc_list, 0, (num_of_file_lines + 100) * sizeof(*checked_pc_list)) == NULL) {
            fprintf(stderr, "MEMSET FAILED");
            exit(EXIT_FAILURE);
        }


        int count = 0, pc = 0, temp_pc = 0, add_result = 0,
            num_I = 0, num_R = 0, num_J = 0, num_branch = 0,
            num_register = 0, num_memory = 0;

        bool PCSrc = false;



        ForwardLine* line;

        ForwardInput* f_input;

        IF_ID* initial_if_id;

        ID_EX* initial_id_ex;

        EX_MEM* initial_ex_mem;

        MEM_WB* initial_mem_wb;

        multiple_malloc_s(6,
            &line, sizeof(*line),
            &f_input, sizeof(*f_input),
            &initial_if_id, sizeof(*initial_if_id),
            &initial_id_ex, sizeof(*initial_id_ex),
            &initial_ex_mem, sizeof(*initial_ex_mem),
            &initial_mem_wb, sizeof(*initial_mem_wb));

        initial_if_id->valid =
            initial_id_ex->valid =
            initial_ex_mem->valid =
            initial_mem_wb->valid = false;


        IF_ID* if_id_latch[2] = { NULL, initial_if_id };
        ID_EX* id_ex_latch[2] = { NULL, initial_id_ex };
        EX_MEM* ex_mem_latch[2] = { NULL, initial_ex_mem };
        MEM_WB* mem_wb_latch[2] = { NULL, initial_mem_wb };


        int left_cycle = 0;

        //int branches = 0;
        //int correct_predictions = 0;
        //int wrong_predictions = 0;

        while (1) {
            ForwardParameter forward_param = {
                .id_ex_rs = -1,
                .id_ex_rt = -1,
                .ex_mem_rd = -1,
                .ex_mem_regwrite = false,
                .mem_wb_rd = -1,
                .ex_mem_regwrite = false
            };


            // WRITE BACK
            f_input->write_data = write_back(mem_wb_latch[1], registers, &forward_param);


            //MEM
            mem_wb_latch[0] = mem_write(ex_mem_latch[1], &PCSrc, memory, &add_result, &(f_input->pre_alu_result), &forward_param, NULL);

            // Branch detection
            if (PCSrc) {
                id_ex_latch[1]->valid = false;
                if_id_latch[1]->valid = false;
            }

            //// Branch detection
            //if ((ex_mem_latch[1] != NULL) && (ex_mem_latch[1]->valid)) {
            //    if (ex_mem_latch[1]->m->branch) {
            //        if (btb[ex_mem_latch[1]->pc] == -1) {
            //            btb[ex_mem_latch[1]->pc] = ex_mem_latch[1]->add_result;

            //            if (ex_mem_latch[1]->zero) {
            //                id_ex_latch[1]->valid = false;
            //                if_id_latch[1]->valid = false;
            //            }
            //        }

            //        else {
            //            if (ex_mem_latch[1]->zero != one_bit_last()) {
            //                id_ex_latch[1]->valid = false;
            //                if_id_latch[1]->valid = false;
            //                pc = ex_mem_latch[1]->pc + 4;
            //                wrong_predictions++;
            //            } else {
            //                correct_predictions++;
            //            }
            //        }

            //        two_bit_update(ex_mem_latch[1]->zero);
            //        one_bit_update(ex_mem_latch[1]->zero);
            //        branches++;
            //    }
            //}

            // Forward
            if ((id_ex_latch[1] != NULL) && id_ex_latch[1]->valid) {
                forward_param.id_ex_rs = id_ex_latch[1]->rs;
                forward_param.id_ex_rt = id_ex_latch[1]->rt;
            }

            line = forward(&forward_param, line);

            // EXECUTE
            ex_mem_latch[0] = execute(id_ex_latch[1], line, f_input, NULL);

            //if ((ex_mem_latch[0] != NULL) && (ex_mem_latch[0]->valid)) {
            //    if ((id_ex_latch[1] != NULL) && id_ex_latch[1]->valid) {
            //        if ((id_ex_latch[1]->opcode == 0) && (id_ex_latch[1]->funct == 8)) {
            //            pc = ex_mem_latch[0]->add_result;
            //            PCSrc = false;
            //            
            //            if_id_latch[1]->valid = false;
            //        }
            //    }
            //}


            // DECODE
            id_ex_latch[0] = decode(if_id_latch[1], registers);

            if ((id_ex_latch[0] != NULL) && id_ex_latch[0]->valid) {
                if (checked_pc_list[id_ex_latch[0]->pc] == 0) {
                    checked_pc_list[id_ex_latch[0]->pc] = 1;

                    num_of_instructions++;

                    count_opcode_type(id_ex_latch[0]->opcode, &num_I, &num_J, &num_R);

                    if (id_ex_latch[0]->m->mem_read || id_ex_latch[0]->m->mem_write) {
                        num_memory++;
                    }

                    if (id_ex_latch[0]->m->branch) {
                        num_branch++;
                    }

                    if (id_ex_latch[0]->wb->reg_write) {
                        num_register++;
                    }
                }
            }

            // JUMP & BRANCH
            if ((id_ex_latch[0] != NULL) && id_ex_latch[0]->valid) {

                // EXECUTE JUMP
                if ((id_ex_latch[0]->opcode == 2) || (id_ex_latch[0]->opcode == 3)) {
                    int address = jump_address(if_id_latch[0]->instruction);
                    temp_pc = ((JUMP_ADDRESS_FLAG) & (pc)) + (address << 2);
                    id_ex_latch[0]->valid = false;

                    if (id_ex_latch[0]->opcode == 3) registers[31] = pc + 4;
                }

                //// UPDATE BTB
                //if ((id_ex_latch[0]->opcode == 4) || (id_ex_latch[0]->opcode == 5)) {
                //    branches++;

                //    if (btb[if_id_latch[1]->pc] == -1)
                //        btb[if_id_latch[1]->pc] = 1;
                //}
            }


            // FETCH
            if_id_latch[0] = fetch(&pc, add_result, &PCSrc, memory, &count, NULL);


            if ((left_cycle == 0) && (pc == -1)) {
                left_cycle = 4;
            } else if (pc == -1) {
                left_cycle -= 1;
                if (left_cycle == 0) break;
            }


#ifdef __DEBUG_MODE__
            fprintf(stdout, "\n\npc : %x\n\n", id_ex_latch[1]->pc_plus_four);

            view_registers(registers);

            fprintf(stdout, "\n\n\n");
#endif

            latch_update(if_id_latch, id_ex_latch, ex_mem_latch, mem_wb_latch);

            if (temp_pc != 0) {
                pc = temp_pc;
                temp_pc = 0;
                PCSrc = false;
            }

            count++;
        }

        printf("[%s]\n", file_name);
        printf("\nR2 : %d  |  Cycles : %d\n\nNum of Instructions : %d\n", registers[2], count, num_of_instructions);
        printf("Num of I type instructions     : %d\n", num_I);
        printf("Num of R type instructions     : %d\n", num_R);
        printf("Num of J type instructions     : %d\n", num_J);
        printf("Num of Mem access instructions : %d\n", num_memory);
        printf("Num of Reg write instructions  : %d\n", num_register);
        printf("Num of Branch instructions     : %d\n", num_branch);

        //printf("\nBranches : %d  |  Correct : %d\n\n------------------------\n\n", branches, correct_predictions);

        free(registers);
        free(memory);
        free(line);
        free(f_input);
        free(checked_pc_list);
        //free(btb);

        fclose(fp);
    }

    return 0;
}

static FILE* initialize_file(char* file_name, int* memory, int* num_of_file_lines) {
    FILE* fp = fopen(file_name, "rb");

    if (fp == NULL) {
        fprintf(stderr, "FILE OPEN FAILED");
        exit(EXIT_FAILURE);
    }

    int* mem_cursor = memory;

    while (feof(fp) == 0) {
        int ret;

        char buffer[4];

        for (int i = 0; i < 4; i++) {
            ret = fread(&buffer[3 - i], 1, 1, fp);
        }

        if (ret != 1) {
            break;
        }

        int input = *(int*)buffer;

        *mem_cursor = input;

        mem_cursor++;
        (*num_of_file_lines)++;
    }

    return fp;
}

static void count_opcode_type(int opcode, int* num_I, int* num_J, int* num_R) {
    char type = opcode_to_inst_type(opcode);

    if (type == 'R') {
        (*num_R)++;
        return;
    }

    if (type == 'I') {
        (*num_I)++;
        return;
    }

    if (type == 'J') {
        (*num_J)++;
        return;
    }

    printf("%d %c", opcode, type);
}

#ifdef __DEBUG_MODE__
static void view_registers(int* registers) {

    char* str = "  R   E   G   I   S   T   E   R  ";

    for (int i = 0; i < 32; i++) {

        fprintf(stdout, "  %c \t|  %d[%x] ", str[i], i, registers[i]);

        int temp = registers[i];

        fprintf(stdout, "\n");
    }
}
#endif
