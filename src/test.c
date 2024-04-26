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

#include "test.h"

#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COMMAND "COMMAND ARG1 ARG2 ARG3"
#define COMMAND_DELIMITERS " \n"

static const char *const strings[] = {
    "foo",
    "bar",
    "foobar",
};

int main(void)
{
	printf("\nVector of Ints\n");
	vectorTestInts(10);
	printf("\nVector of Strings\n");
	vectorTestStrings(strings, sizeof(strings) / sizeof(strings[0]));

	return 0;
}

void vectorTestInts(const size_t size)
{
	srand(time(NULL));
	Vector vector;

	vector = vectorCreate(sizeof(int), NULL, vectorIntStdPrint);

	for (size_t i = 0; i < size; ++i) {
		const int randomValue = rand() % 1000;
		vectorPushBack(&vector, &randomValue);
	}

	printf("\n");

	vectorPrint(stdout, vector);
}

void vectorTestStrings(const char *const *const strings, const size_t stringsN)
{
	Vector vector
	    = vectorCreate(sizeof(char *), free, vectorStringStdPrint);

	for (size_t i = 0; i < stringsN; ++i) {
		const char *const string = strdup(strings[i]);
		vectorPushBack(&vector, &string);
	}

	printf("\n");

	vectorPrint(stdout, vector);

	vectorClear(&vector);

	printf("\nVector From Splitting a string\n");
	Vector command = vectorStringSplit(COMMAND, COMMAND_DELIMITERS);
	vectorPrint(stdout, command);
}
