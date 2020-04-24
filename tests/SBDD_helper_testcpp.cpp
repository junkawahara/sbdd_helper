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

#include <vector>
#include <iostream>
#include <fstream>

#define BDD_CPP
#include "../SBDD/bddc.h"
#include "../SBDD/BDD.h"
#include "../SBDD/ZBDD.h"

#include "SBDD_helper_testc_cpp.h"

std::vector<bddvar> ullint_to_varvec(ullint v)
{
    int count = 1;
    std::vector<bddvar> vec;
    while (v > 0) {
        if ((v & 1llu) != 0) {
            vec.push_back((bddvar)count);
        }
        v >>= 1;
        ++count;
    }
    return vec;
}


void test_at_random_cpp()
{
    const size_t w = 30; // number of variables
    const size_t N = 1000; // number of cardinality of the constructed ZDD
    int i, j;
    bool found;
    ullint w_pow, c;
    ullint* ar;
    size_t sp = 0;
    ZBDD f, g, h;
    std::vector<bddvar> varvec;
    std::ifstream ifs;
    std::ofstream ofs;

    w_pow = (1llu << w);

    ar = (ullint*)malloc(N * sizeof(ullint));
    if (ar == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    // make array whose elements are distinct
    while (sp < N) {
        c = (((ullint)rand() << 32) | ((ullint)rand())) % w_pow;
        if (c == 0) {
            continue;
        }
        for (i = 0; i < (int)sp; ++i) {
            if (ar[i] == c) {
                break;
            }
        }
        if (i < (int)sp) {
            continue;
        }
        ar[sp] = c;
        ++sp;
    }

    f = ZBDD(0);

    for (i = 0; i < (int)N; ++i) {
        g = getSingleSet(ullint_to_varvec(ar[i]));
        f += g;
    }
    test_eq(f.Card(), N);

    for (i = 0; i < (int)N; ++i) {
        test(isMemberZ(f, ullint_to_varvec(ar[i])));
    }

    for (i = 0; i < 2 * (int)N; ++i) {
        c = (ullint)rand() & w_pow;
        found = false;
        for (j = 0; j < (int)N; ++j) {
            if (ar[j] == c) {
                found = true;
                break;
            }
        }
        test(isMemberZ(f, ullint_to_varvec(c)) == found);
    }
    test(!isMemberZ(f, std::vector<bddvar>()));

    f += ZBDD(1);
    test(isMemberZ(f, std::vector<bddvar>()));

    ofs.open(g_filename1);
    if (!ofs) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    writeZBDDToFileKnuth(ofs, f, 0);
    ofs.close();

    ifs.open(g_filename1);
    g = constructZBDDFromFileKnuth(ifs, 0);
    ifs.close();
    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    test(f == g);

    ofs.open(g_filename1, std::ofstream::binary);
    if (!ofs) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    writeZBDDToBinary(ofs, f);
    ofs.close();

    ifs.open(g_filename1, std::ifstream::binary);
    g = constructZBDDFromBinary(ifs);
    ifs.close();
    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    test(f == g);


    /*
    bddNodeIndex* index = bddNodeIndex_makeIndexZ(g);
    test_eq(bddNodeIndex_count(index), bddcard(g));
    bddNodeIndex_destruct(index);

    index = bddNodeIndex_makeRawIndexZ(g);
    test_eq(bddNodeIndex_count(index), bddcard(g));
    test_eq(bddNodeIndex_size(index), bddsize(g));
    bddNodeIndex_destruct(index);
    */

    free(ar);
}


void test_index_cpp()
{
    ZBDD f = ZBDD_ID(make_test_zbdd());
    DDNodeIndex index(f, false);
    test_eq(index.count(), 3);
    test_eq(index.size(), 4);

    DDNodeIndex::DDNodeIterator itor = index.begin();
    int count = 0;
    while (itor != index.end()) {
        ++itor;
        ++count;
    }
    test_eq(count, 4);

    ZBDD g = f.Change(4);

    test(std::find(index.begin(), index.end(), f.GetID()) != index.end());
    test(std::find(index.begin(), index.end(), bddgetchild1z(f.GetID())) != index.end());
    test(std::find(index.begin(), index.end(), bddgetchild0z(f.GetID())) != index.end());
    test(std::find(index.begin(), index.end(),
                   bddgetchild1z(bddgetchild0z(f.GetID()))) != index.end());
    test(std::find(index.begin(), index.end(), g.GetID()) == index.end());

    std::vector<bddvar> vararr;
    for (int i = 0; i < 40; ++i) {
        vararr.push_back(i + 1);
    }
    f = getPowerSet(vararr);
    DDNodeIndex index2(f, false);
    test_eq(index2.count(), 1ll << 40);
    test_eq(index2.size(), 40);

    f = ZBDD_ID(make_test_zbdd());
    DDNodeIndex index3(f, true);
    test_eq(index3.count(), 3);
    test_eq(index3.size(), 4);

    f = getPowerSet(vararr);
    DDNodeIndex index4(f, false);
    test_eq(index4.count(), 1ll << 40);
    test_eq(index4.size(), f.Size());
}

bool test_count_if_size1(std::set<bddvar> s)
{
    return s.size() == 1;
}

bool test_count_if_size2(std::set<bddvar> s)
{
    return s.size() == 2;
}

bool test_count_if_size3(std::set<bddvar> s)
{
    return s.size() == 3;
}

void test_for_each(std::set<bddvar> s)
{
    test_eq(s.size(), 2);
}

void test_elementIterator_cpp()
{
    ZBDD f = ZBDD_ID(make_test_zbdd());
    // f is expected to be {{3, 2}, {3, 1}, {2, 1}}

    std::set<bddvar> s12; // {1, 2}
    s12.insert(1); s12.insert(2);
    std::set<bddvar> s13; // {1, 3}
    s13.insert(1); s13.insert(3);
    std::set<bddvar> s23; // {2, 3}
    s23.insert(2); s23.insert(3);
    std::set<bddvar> s123; // {1, 2, 3}
    s123.insert(1); s123.insert(2); s123.insert(3);

    {
        ElementIteratorMaker eim(f);
        ElementIterator itor = eim.begin();
        test(itor != eim.end());
        test(*itor == s23);

        ++itor;
        test(itor != eim.end());
        test(*itor == s13);

        ++itor;
        test(itor != eim.end());
        test(*itor == s12);

        ++itor;
        test(itor == eim.end());
    } // itor destructed

    {
        ZBDD g = f + ZBDD(1);
        // g is expected to be {{}, {3, 2}, {3, 1}, {2, 1}}
        ElementIteratorMaker eim(g);
        ElementIterator itor = eim.begin();
        test(itor != eim.end());
        test(*itor == s23);

        ++itor;
        test(itor != eim.end());
        test(*itor == s13);

        ++itor;
        test(itor != eim.end());
        test(*itor == s12);

        ++itor;
        test(itor != eim.end());
        test_eq(itor->size(), 0);

        ++itor;
        test(itor == eim.end());
    } // itor destructed

    // bddempty test
    {
        ElementIteratorMaker eim(ZBDD(0));
        ElementIterator itor = eim.begin();
        test(itor == eim.end());
    }

    // bddsingle test
    {
        ElementIteratorMaker eim(ZBDD(1));
        ElementIterator itor = eim.begin();
        test(itor != eim.end());
        test_eq(itor->size(), 0);
        ++itor;
        test(itor == eim.end());
    }

    // algorithm start
    {
        ElementIteratorMaker eim(f);
        ElementIterator itor = std::find(eim.begin(), eim.end(), s13);
        test(itor != eim.end()); // found

        itor = std::find(eim.begin(), eim.end(), s123);
        test(itor == eim.end()); // not found

        test_eq(std::count(eim.begin(), eim.end(), s12), 1);
        test_eq(std::count(eim.begin(), eim.end(), s13), 1);
        test_eq(std::count(eim.begin(), eim.end(), s23), 1);
        test_eq(std::count(eim.begin(), eim.end(), s123), 0);

        test_eq(std::count_if(eim.begin(), eim.end(), test_count_if_size1), 0);
        test_eq(std::count_if(eim.begin(), eim.end(), test_count_if_size2), 3);
        test_eq(std::count_if(eim.begin(), eim.end(), test_count_if_size3), 0);

        std::for_each(eim.begin(), eim.end(), test_for_each);
    }
}

void start_test_cpp()
{
    test_at_random_cpp();
    test_index_cpp();
    test_elementIterator_cpp();
}

int main()
{
    start_test();
    start_test_cpp();

    printf("test passed!\n");
    return 0;
}
