#ifndef __DECODE_H__

#define __DECODE_H__

#include "instruction.h"

#include "mode.h"

INSTRUCTION* decode(int input, int* opcode_return);

#endif // !__DECODE_H__