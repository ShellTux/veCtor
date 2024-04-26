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

typedef struct {
	size_t size;
	size_t capacity;
	void *data;
	size_t elementSize;

	void (*freeElement)(void *);
	void (*printElement)(void *);
} Vector;


Vector vectorCreate(const size_t elementSize,
                    void (*freeElementFunction)(void *),
                    void (*printElementFunction)(void *));

void *vectorPushBack(Vector *vector, const void *const element);
void *vectorGet(const Vector *const vector, const size_t index);
void vectorClear(Vector *vector);
Vector vectorStringSplit(const char *const string,
                         const char *const delimiters);

#endif // !VECTOR_H
