#include "alloc.h"
#include <stdarg.h>

void* malloc_s(size_t size) {
	void* pointer = malloc(size);

	if (pointer == NULL) {
		fprintf(stderr, "MALLOC FAILED");
		exit(EXIT_FAILURE);
	}

	return pointer;
}

int multiple_malloc_s(int count, ...) {
    va_list ap;

	va_start(ap, count); //Requires the last fixed parameter (to get the address)
    
	int malloc_count = 0;

	for (int i = 0; i < count; i++) {
		void** pptr = va_arg(ap, void**);

		size_t size = va_arg(ap, size_t);

		*pptr = malloc_s(size);

		malloc_count++;
	}

    va_end(ap);

	return malloc_count;
}