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

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const Vector INVALID_VECTOR = {0};

static char *trim(char *string);

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

char *vectorString(const Vector vector)
{
	if (memcmp(&vector, &INVALID_VECTOR, sizeof(Vector)) == 0) {
		return NULL;
	}

#ifdef VECTOR_STRING_FILE_IMPLEMENTATION
	FILE *tempFile = fopen(VECTOR_TEMP_FILE, "wb");
	if (tempFile == NULL) {
		fprintf(stderr,
		        "Failed to open file: \"%s\"\n",
		        VECTOR_TEMP_FILE);
		return NULL;
	}

	vectorPrint(tempFile, vector);
	fclose(tempFile);

	tempFile = fopen(VECTOR_TEMP_FILE, "rb");
	if (tempFile == NULL) {
		fprintf(stderr,
		        "Failed to open file: \"%s\"\n",
		        VECTOR_TEMP_FILE);
		return NULL;
	}

	fseek(tempFile, 0, SEEK_END);
	const unsigned long fileSize = ftell(tempFile);
	rewind(tempFile);

	char *const buffer = malloc(fileSize + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Error allocating memory\n");
		return NULL;
	}

	if (fread(buffer, 1, fileSize, tempFile) != fileSize) {
		fprintf(stderr,
		        "Error reading file: \"%s\"\n",
		        VECTOR_TEMP_FILE);
		free(buffer);
		return NULL;
	}

	buffer[fileSize] = '\0';

	char *const string = strdup(trim(buffer));
	free(buffer);

	return string;
#else
	#error "VectorString using Strings not yet implemented"
	assert(0 && "TODO: VectorString using Strings not yet implemented!");
#endif
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

static char *trim(char *string)
{
	size_t length = 0;
	char *frontP  = string;
	char *endP    = NULL;

	if (string == NULL) {
		return NULL;
	}
	if (string[0] == '\0') {
		return string;
	}

	length = strlen(string);
	endP   = string + length;

	/* Move the front and back pointers to address the first non-whitespace
   * characters from each end.
   */
	while (isspace((unsigned char) *frontP)) {
		++frontP;
	}
	if (endP != frontP) {
		while (isspace((unsigned char) *(--endP)) && endP != frontP) {}
	}

	if (frontP != string && endP == frontP) *string = '\0';
	else if (string + length - 1 != endP) *(endP + 1) = '\0';

	/* Shift the string so that it starts at str so that if it's dynamically
   * allocated, we can still free it on the returned pointer.  Note the reuse
   * of endp to mean the front of the string buffer now.
   */
	endP = string;
	if (frontP != string) {
		while (*frontP) {
			*endP++ = *frontP++;
		}
		*endP = '\0';
	}

	return string;
}
