#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "data.h"
#include "operation.h"
#include "memory.h"
#include "color.h"
#include "keyboard.h"


int main(int argc, char* argv[]) {

	int mem_size = dMEMSIZE;

	char* file_name;

	DATA registers[10] = { 0, };

	for (int i = 1; i < argc; i++) {

		if (strcmp(argv[i], "--mem-size") == 0) {
			
			if (argc <= i + 1) {
				fprintf(stderr, "ARGUMENTS ERROR");
				exit(EXIT_FAILURE);
			}

			i++;
			mem_size = atoi(argv[i]);
			continue;
		}

		if (strcmp(argv[i], "--reg") == 0) {
			if (argc <= i + 2) {
				fprintf(stderr, "ARGUMENTS ERROR");
				exit(EXIT_FAILURE);
			}

			int A = atoi(argv[i + 1]);

			if ((A > 9) || (A < 0)) {
				fprintf(stderr, "ARGUMENTS ERROR");
				exit(EXIT_FAILURE);
			}

			registers[A] = atoi(argv[i + 2]);

			i += 2;
			continue;
		}

		file_name = argv[i];
	}

	MEMORY* memory = initialize_memory(mem_size);

	if (memory != load_file(memory, file_name)) {
		fprintf(stderr, "FILE LOAD ERROR \n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Input File : %s\nRunning Calculator With Input File", file_name);

	fprintf(stdout, "\n\nTO CONTINUE, PRESS ENTER");

	while (1) {
		char c = getch();

		if (c == 10) break;
	}

	operation(memory, registers);

	fprintf(stdout, NC "\nRETURN VALUE : %d\n\n", registers[0]);

	for (int i = 0; i < memory->used_size; i++) {
		free(memory->insts[i]->op1);
		free(memory->insts[i]->op2);
		free(memory->insts[i]);
	}

	free(memory);

	exit(EXIT_SUCCESS);
}