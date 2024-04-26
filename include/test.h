#ifndef VECTOR_TEST_H
#define VECTOR_TEST_H

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

void vectorTestInts(const size_t size);
void vectorTestStrings(const char *const *const strings, const size_t stringsN);

#endif // !VECTOR_TEST_H
