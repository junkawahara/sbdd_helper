//
// One header library for SAPPOROBDD C/C++ version
// version 0.05 alpha
//
// Copyright (c) 2017 -- 2022 Jun Kawahara
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SBDD_HELPER_H
#define SBDD_HELPER_H

#ifdef __cplusplus

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstdarg>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#endif

#ifdef __cplusplus
namespace sbddh {
#endif

typedef long long int llint;
typedef unsigned long long int ullint;

#define unused(a) (void)(a)

// inline function qualifier for gcc
// if a compile error occurs, change the qualifier
#define sbddextended_INLINE_FUNC static inline

#define sbddextended_MAX_LINE 256
#define sbddextended_BDDNODE_START 2
#define sbddextended_NUMBER_OF_CHILDREN 2

#define sbddextended_TEMP_BUFSIZE 1024

#include "utility.h"
#include "MyVector.h"
#include "MyDict.h"
#include "readLine.h"
#include "writeLine.h"
#include "globalFunction.h"
#include "NodeIndex.h"
#include "NodeIterator.h"
#include "ElementIterator.h"
#include "io.h"
#include "bddbinaryformat.h"

#ifdef __cplusplus
} // end of namespace sbddh
#endif

#endif // SBDD_HELPER_H
