#include "predictor.h"

bool one_bit_counter = false;

bool two_bit_counter[2] = { false, false };

bool one_bit_last() {
	
	return one_bit_counter;
}

void one_bit_update(bool is_taken) {
	
	one_bit_counter = is_taken;
}

bool two_bit_last() {

	return two_bit_counter[0];
}

void two_bit_update(bool is_taken) {

	two_bit_counter[0] = two_bit_counter[1];

	two_bit_counter[1] = is_taken;
}

