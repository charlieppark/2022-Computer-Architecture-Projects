#ifndef __ALLOC_H__
#define __ALLOC_H__

#include <stdio.h>
#include <stdlib.h>

void* malloc_s(size_t size);

int multiple_malloc_s(int count, ...);

#endif //!__ALLOC_H__
