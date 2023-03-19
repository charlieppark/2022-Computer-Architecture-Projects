#ifndef __OPERAND_H__

#define __OPERAND_H__

#include <stdlib.h>
#include "data.h"

typedef enum {
	NONE = -1,
	REGISTER = 'R',
	HEXA = '0'
} TYPE;

typedef struct {

	TYPE type;
	DATA value;

} OPERAND;

OPERAND* operand_initializer(char* token);
DATA value_by_type(OPERAND* operand, DATA* registers);

#endif // !__OPERAND_H__
