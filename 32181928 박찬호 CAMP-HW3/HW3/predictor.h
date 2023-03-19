#ifndef __PREDICTOR_H__
#define __PREDICTOR_H__

#include <stdbool.h>

bool one_bit_last();

void one_bit_update(bool is_taken);

bool two_bit_last();

void two_bit_update(bool is_taken);

#endif // !__PREDICTOR_H__

