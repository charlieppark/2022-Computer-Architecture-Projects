#include "alu.h"

int BEQ    (int a, int b, bool* zero) {
    *zero = (a == b);

    return 0;
}

int BNE    (int a, int b, bool* zero) {
    *zero = (a != b);

    return 0;
}

