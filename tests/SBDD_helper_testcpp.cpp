//
// One header library for SAPPOROBDD C++ version test code
//
// Copyright (c) 2017 Jun Kawahara
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BDD_CPP
#include "../SBDD/bddc.h"
#include "../SBDD/BDD.h"
#include "../SBDD/ZBDD.h"

#include "SBDD_helper_testc_cpp.h"

void test_index_cpp()
{
    ZBDD f = ZBDD_ID(make_test_bdd());
    DDNodeIndex index(f, false);
    test_eq(index.Count(), 3);
    test_eq(index.Size(), 4);

    DDNodeIndex::DDNodeIterator itor = index.begin();
    int count = 0;
    while (itor != index.end()) {
        ++itor;
        ++count;
    }
    test_eq(count, 4);

    std::vector<bddvar> vararr;
    for (int i = 0; i < 40; ++i) {
        vararr.push_back(i + 1);
    }
    f = GetPowerSet(vararr);
    DDNodeIndex index2(f, false);
    test_eq(index2.Count(), 1ll << 40);
    test_eq(index2.Size(), 40);

    f = ZBDD_ID(make_test_bdd());
    DDNodeIndex index3(f, true);
    test_eq(index3.Count(), 3);
    test_eq(index3.Size(), 4);

    f = GetPowerSet(vararr);
    DDNodeIndex index4(f, false);
    test_eq(index4.Count(), 1ll << 40);
    test_eq(index4.Size(), f.Size());
}

void start_test_cpp()
{
    test_index_cpp();
}

int main()
{
    start_test();
    start_test_cpp();

    printf("test passed!\n");
    return 0;
}
