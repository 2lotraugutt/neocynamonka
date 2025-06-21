/*
Copyright (c) 2018 [REDACTED]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h> // malloc
#include "vector.h"

int vector_init(vector* vec) { 
	vec->len = 0;
	vec->size = 1;
	vec->elems = malloc(vec->size*sizeof(void*));
	if(!vec->elems) return -1;
	return 0;
}
int vector_init_sized(size_t size, vector* vec) {
	vec->len = 0;
	vec->size = size;
	vec->elems = malloc(vec->size*sizeof(void*));
	if(!vec->elems) return -1;
	return 0;
}
int vector_append(void* elem, vector* vec) {
	if (++(vec->len) > vec->size) {vec->size *= 2; vec->elems = realloc(vec->elems, vec->size);};
	if(!vec->elems) return -1;
	vec->elems[vec->len-1] = elem;
	return 0;
}

void* vector_get(size_t index, vector* vec) {
	if(index >= vec->len) return 0;
	return vec->elems[index];
}

int vector_len(vector* vec) {
	return vec->len;
}

