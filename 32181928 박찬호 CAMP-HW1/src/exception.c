#include "exception.h"

void print_exception(char* message) {
    if (message == NULL) {
        fprintf(stderr, RED "[ERROR]"
			NC  ": Improper Exception Message Error! \n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, RED "[ERROR]"
			NC  ": %s \n", message);

    return;
}
