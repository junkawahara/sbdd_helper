/*
One header library for SAPPOROBDD C/C++ version
version 1.2.0

Copyright (c) 2017 -- 2024 Jun Kawahara

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef SBDD_HELPER_H
#define SBDD_HELPER_H

#ifdef __cplusplus

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <climits>
#include <cmath>
#include <cassert>
#include <cstdarg>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#if __cplusplus >= 201103L /* use rand() function */
#include <random>
#endif

#ifdef SBDDH_GMP
#include <gmp.h>
#include <gmpxx.h>
#endif

#else /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#endif /* __cplusplus */

#ifdef __cplusplus
namespace sbddh {
#endif

typedef long long int llint;
typedef unsigned long long int ullint;

#define unused(a) (void)(a)

/* inline function qualifier for gcc */
/* if a compile error occurs, change the qualifier */
#define sbddextended_INLINE_FUNC static inline

#define sbddextended_BDDNODE_START 2
#define sbddextended_NUMBER_OF_CHILDREN 2

#define sbddextended_BUFSIZE 1024

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef SBDDH_GMP
    const mpz_class sbddextended_VALUE_ZERO(0);
    const mpz_class sbddextended_VALUE_ONE(1);
#else
    const llint sbddextended_VALUE_ZERO = 0;
    const llint sbddextended_VALUE_ONE = 1;
#endif

/* for compatibility */
#define getAllSetsIncluding getPowerSetIncluding
#define getAllPowerSetsIncluding getPowerSetIncluding
#define getAllPowerSetsNotIncluding getPowerSetNotIncluding
#define getAllSetsWithCard getPowerSetWithCard
#define getAllPowerSetsWithCard getPowerSetWithCard

#define bddconstructzbddfrombinary bddimportzbddasbinary
#define constructZBDDFromBinary importZBDDAsBinary

#define bddwritezbddtobinary bddexportzbddasbinary
#define writeZBDDToBinary exportZBDDAsBinary

#define constructZBDDFromGraphillion importZBDDAsGraphillion
#define bddwritebddforgraphillion bddexportbddasgraphillion
#define writeZBDDForGraphillion exportZBDDAsGraphillion

#define bddconstructbddfromfileknuth bddimportbddasknuth
#define bddconstructzbddfromfileknuth bddimportzbddasknuth
#define constructBDDFromFileKnuth importBDDAsKnuth
#define constructZBDDFromFileKnuth importZBDDAsKnuth

#define bddwritezbddtofileknuth bddexportzbddasknuth
#define writeZBDDToFileKnuth exportZBDDAsKnuth

#define bddwritebddforgraphviz bddexportbddasgraphviz
#define writeBDDForGraphviz exportBDDAsGraphviz
#define writeZBDDForGraphviz exportZBDDAsGraphviz


#include "utility.h"
#include "MyVector.h"
#include "MyDict.h"
#include "MySet.h"
#include "readLine.h"
#include "writeLine.h"
#include "globalFunction.h"
#include "NodeIndex.h"
#include "NodeIterator.h"
#include "ElementIterator.h"
#include "io_elements.h"
#include "io_binary.h"
#include "io_graphillion.h"
#include "io_knuth.h"
#include "io_graphviz.h"
#include "io_svg.h"

#ifdef __cplusplus
} /* end of namespace sbddh */
#endif

#endif /* SBDD_HELPER_H */
