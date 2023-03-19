#include "operand.h"
#include "exception.h"

OPERAND* operand_initializer(char* token) {

	OPERAND* operand = malloc(sizeof(*operand));

	TYPE type = token[0];

	switch (type) {

	case REGISTER:
		operand->type = type;
		operand->value = token[1] - '0';
		break;

	case HEXA:
		operand->type = type;
		operand->value = (int)strtol(token, NULL, 16);
		break;

	case NONE:
	default:
		free(operand);
		operand = NULL;
		break;
	}

	return operand;
}

DATA value_by_type(OPERAND* operand, DATA* registers) {
	
	switch (operand->type) {

	case REGISTER:
		return registers[operand->value];

	case HEXA:
		return operand->value;

	case NONE:
	default:
		print_exception("Wrong Value In Operand!");
		return NULL;
	}
}