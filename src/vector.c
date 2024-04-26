/*******************************************************************************
 * Project                                                  ____ _
 *                                              __   _____ / ___| |_ ___  _ __
 *                                              \ \ / / _ \ |   | __/ _ \| '__|
 *                                               \ V /  __/ |___| || (_) | |
 *                                                \_/ \___|\____|\__\___/|_|
 *
 *
 * Author: Luís Góis
 *
 * This software is licensed as described in the file LICENSE, which
 * you should have received as part of this distribution.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the LICENSE file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vector vectorCreate(const size_t elementSize,
                    FreeElementFunction freeElementFunction,
                    PrintElementFunction printElementFunction)
{
	Vector vector = {
	    .elementSize = elementSize,
	    .capacity    = 0,
	    .size        = 0,
	    .data        = NULL,

	    .freeElement  = freeElementFunction,
	    .printElement = printElementFunction,
	};

	return vector;
}

void *vectorPushBack(Vector *vector, const void *const element)
{
	if (vector->data == NULL
	    && (vector->size != 0 || vector->capacity != 0)) {
		vectorClear(vector);
	}

	if (vector->data != NULL && vector->size == 0) {
		vectorClear(vector);
	}

	if (vector->size + 1 > vector->capacity) {
		vector->capacity
		    = vector->capacity == 0 ? 1 : vector->capacity * 2;
		vector->data = realloc(vector->data,
		                       vector->capacity * vector->elementSize);
		if (vector->data == NULL) {
			return NULL;
		}
	}

	return memcpy(vectorGet(vector, vector->size++),
	              element,
	              vector->elementSize);
}

void *vectorGet(const Vector *const vector, const size_t index)
{
	return index >= vector->size
	           ? NULL
	           : vector->data + index * vector->elementSize;
}

void vectorClear(Vector *vector)
{
	if (vector == NULL) {
		return;
	}

	if (vector->data != NULL) {
		if (vector->freeElement != NULL) {
			for (size_t i = 0; i < vector->size; ++i) {
				void **element = vectorGet(vector, i);
				vector->freeElement(*element);
			}
		}

		free(vector->data);
	}

	vector->size     = 0;
	vector->capacity = 0;
	vector->data     = NULL;
}

Vector vectorStringSplit(const char *const string, const char *const delimiters)
{
	Vector vector
	    = vectorCreate(sizeof(char *), free, vectorStringStdPrint);

	char *stringClone = strdup(string);
	if (stringClone == NULL) {
		return vector;
	}

	for (char *token = strtok(stringClone, delimiters); token != NULL;
	     token       = strtok(NULL, delimiters)) {
		const void *const t = strdup(token);
		vectorPushBack(&vector, &t);
	}

	free(stringClone);

	return vector;
}

void vectorPrint(FILE *file, const Vector vector)
{
	fprintf(file, "[");
	if (vector.data != NULL) {
		for (size_t i = 0; i < vector.size; ++i) {
			if (i > 0) {
				fprintf(file, ", ");
			}

			const void *const element = vectorGet(&vector, i);
			if (vector.printElement == NULL) {
				vectorMemoryStdPrint(file,
				                     element,
				                     vector.elementSize);
			} else {
				vector.printElement(file,
				                    element,
				                    vector.elementSize);
			}
		}
	}
	fprintf(file, "]\n");
}

void vectorMemoryStdPrint(FILE *file,
                          const void *const element,
                          const size_t elementSize)
{
	const unsigned char *const ptr = element;

	fprintf(file, "0x");
	for (int i = elementSize - 1; i >= 0; --i) {
		if (ptr[i] == 0) {
			continue;
		}

		fprintf(file, "%X", ptr[i]);
	}
}

#define WRAPPER(TYPE, FUNCTION, FORMAT)                   \
	void FUNCTION(FILE *file,                         \
	              const void *const element,          \
	              const size_t elementSize)           \
	{                                                 \
		(void) elementSize;                       \
		fprintf(file, FORMAT, *(TYPE *) element); \
	}
PRIMITIVE_TYPES
#undef WRAPPER
