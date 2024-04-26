#ifndef VECTOR_H
#define VECTOR_H

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

#include <stddef.h>
#include <stdio.h>

typedef void (*FreeElementFunction)(void *);
typedef void (*PrintElementFunction)(const void *const);

typedef struct {
	size_t size;
	size_t capacity;
	void *data;
	size_t elementSize;

	FreeElementFunction freeElement;
	PrintElementFunction printElement;
} Vector;


Vector vectorCreate(const size_t elementSize,
                    FreeElementFunction freeElementFunction,
                    PrintElementFunction printElementFunction);

void *vectorPushBack(Vector *vector, const void *const element);
void *vectorGet(const Vector *const vector, const size_t index);
void vectorClear(Vector *vector);
Vector vectorStringSplit(const char *const string,
                         const char *const delimiters);
void vectorPrint(FILE *file, const Vector vector);

#define PRIMITIVE_TYPES                                 \
	WRAPPER(char *, vectorStringStdPrint, "\"%s\"") \
	WRAPPER(int, vectorIntStdPrint, "%d")           \
	WRAPPER(float, vectorFloatStdPrint, "%f")       \
	WRAPPER(double, vectorDoubleStdPrint, "%f")

void vectorMemoryStdPrint(const void *const element);

#define WRAPPER(TYPE, FUNCTION, FORMAT) \
	void FUNCTION(const void *const element);
PRIMITIVE_TYPES
#undef WRAPPER

#endif // !VECTOR_H
