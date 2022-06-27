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

    f = getAllPowerSetsIncluding(3, 2);
    test(f.Card() == 4);
    test(f.OnSet(2).Card() == 4);
    test(f.OffSet(2).Card() == 0);

    std::vector<bddvar> bv;
    bv.push_back(2);
    bv.push_back(3);
    f = getAllPowerSetsIncluding(5, bv);
    test(f.Card() == 8);
    test(f.OnSet(3).Card() == 8);
    test(f.OffSet(3).Card() == 0);
    test(f.OnSet(4).Card() == 4);
    test(f.OffSet(4).Card() == 4);

    std::vector<bddvar> basev;
    basev.push_back(2);
    basev.push_back(4);
    basev.push_back(6);

    f = getAllSetsIncluding(basev, 3);
    test(f.Card() == 8);
    test(f.OnSet(3).Card() == 8);
    test(f.OffSet(3).Card() == 0);
    test(f.OnSet(4).Card() == 4);
    test(f.OffSet(4).Card() == 4);

    f = getAllSetsIncluding(basev, bv);
    test(f.Card() == 4);
    test(f.OnSet(3).Card() == 4);
    test(f.OffSet(3).Card() == 0);
    test(f.OnSet(4).Card() == 2);
    test(f.OffSet(4).Card() == 2);


    f = getAllPowerSetsNotIncluding(3, 2);
    test(f.Card() == 4);
    test(f.OnSet(2).Card() == 0);
    test(f.OffSet(2).Card() == 4);

    f = getAllPowerSetsNotIncluding(5, bv);
    test(f.Card() == 8);
    test(f.OnSet(3).Card() == 0);
    test(f.OffSet(3).Card() == 8);
    test(f.OnSet(4).Card() == 4);
    test(f.OffSet(4).Card() == 4);

    f = getAllSetsWithCard(basev, 0);
    test(f.Card() == 1);
    f = getAllSetsWithCard(basev, 1);
    test(f.Card() == 3);
    f = getAllSetsWithCard(basev, 2);
    test(f.Card() == 3);
    f = getAllSetsWithCard(basev, 3);
    test(f.Card() == 1);

    f = getAllPowerSetsWithCard(5, 2);
    test(f.Card() == 10);

    std::vector<bddvar> sv;
    sv.push_back(1);
    sv.push_back(3);
    sv.push_back(5);
    f = makeDontCare(getSingleSet(basev), sv);
    //printZBDDElements(std::cout, f);
    test(f.Card() == 8);
    test(f.OnSet(3).Card() == 4);
    test(f.OffSet(3).Card() == 4);
    test(f.OnSet(4).Card() == 8);
    test(f.OffSet(4).Card() == 0);

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

void test_io_cpp()
{
    ZBDD f = ZBDD_ID(make_test_zbdd());
    test(ZStr(f) == std::string("{3, 2}, {3, 1}, {2, 1}"));
    test(zstr(f) == std::string("{3, 2}, {3, 1}, {2, 1}"));
    test(ZStr(ZBDD(-1)) == std::string("N"));
    test(ZStr(ZBDD(0)) == std::string("E"));
    test(ZStr(ZBDD(1)) == std::string("{}"));
}

void test_index_cpp()
{
    ZBDD f = ZBDD_ID(make_test_zbdd());
    DDNodeIndex index(f, false);
    test_eq(index.count(), 3);
    test_eq(index.size(), 4);

    test_eq(index.sizeAtLevel(1), 1);
    test_eq(index.sizeAtLevel(2), 2);
    test_eq(index.sizeAtLevel(3), 1);

    std::vector<bddvar> vec;
    index.sizeEachLevel(vec);

    test_eq(vec.size(), 4);
    test_eq(vec[1], 1);
    test_eq(vec[2], 2);
    test_eq(vec[3], 1);

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

    int nn = 40;
    std::vector<bddvar> vararr;
    for (int i = 0; i < nn; ++i) {
        vararr.push_back(i + 1);
    }
    f = getPowerSet(vararr);
    DDNodeIndex index2(f, false);
    test_eq(index2.count(), 1ll << nn);
    test_eq(index2.size(), nn);

    f = ZBDD_ID(make_test_zbdd());
    DDNodeIndex index3(f, true);
    test_eq(index3.count(), 3);
    test_eq(index3.size(), 4);

    f = getPowerSet(vararr);
    DDNodeIndex index4(f, false);
    test_eq(index4.count(), 1ll << nn);
    test_eq(index4.size(), f.Size());

    int mm = 50;
    f = getPowerSet(mm);
    DDNodeIndex index5(f, false);
    test_eq(index5.count(), 1ll << mm);
    test_eq(index5.size(), mm);

    DDNodeIndex index6(ZBDD(0), false);
    test_eq(index6.count(), 0);
    test_eq(index6.size(), 0);

    DDNodeIndex index7(ZBDD(1), false);
    test_eq(index7.count(), 1);
    test_eq(index7.size(), 0);
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
        ElementIteratorHolder eih(f);
        ElementIterator itor = eih.begin();
        test(itor != eih.end());
        test(*itor == s23);

        ++itor;
        test(itor != eih.end());
        test(*itor == s13);

        ++itor;
        test(itor != eih.end());
        test(*itor == s12);

        ++itor;
        test(itor == eih.end());
    } // itor destructed

    {
        ZBDD g = f + ZBDD(1);
        // g is expected to be {{}, {3, 2}, {3, 1}, {2, 1}}
        ElementIteratorHolder eih(g);
        ElementIterator itor = eih.begin();
        test(itor != eih.end());
        test(*itor == s23);

        ++itor;
        test(itor != eih.end());
        test(*itor == s13);

        ++itor;
        test(itor != eih.end());
        test(*itor == s12);

        ++itor;
        test(itor != eih.end());
        test_eq(itor->size(), 0);

        ++itor;
        test(itor == eih.end());
    } // itor destructed

    // bddempty test
    {
        ElementIteratorHolder eih(ZBDD(0));
        ElementIterator itor = eih.begin();
        test(itor == eih.end());
    }

    // bddsingle test
    {
        ElementIteratorHolder eih(ZBDD(1));
        ElementIterator itor = eih.begin();
        test(itor != eih.end());
        test_eq(itor->size(), 0);
        ++itor;
        test(itor == eih.end());
    }

    // algorithm start
    {
        ElementIteratorHolder eih(f);
        ElementIterator itor = std::find(eih.begin(), eih.end(), s13);
        test(itor != eih.end()); // found

        itor = std::find(eih.begin(), eih.end(), s123);
        test(itor == eih.end()); // not found

        test_eq(std::count(eih.begin(), eih.end(), s12), 1);
        test_eq(std::count(eih.begin(), eih.end(), s13), 1);
        test_eq(std::count(eih.begin(), eih.end(), s23), 1);
        test_eq(std::count(eih.begin(), eih.end(), s123), 0);

        test_eq(std::count_if(eih.begin(), eih.end(), test_count_if_size1), 0);
        test_eq(std::count_if(eih.begin(), eih.end(), test_count_if_size2), 3);
        test_eq(std::count_if(eih.begin(), eih.end(), test_count_if_size3), 0);

        std::for_each(eih.begin(), eih.end(), test_for_each);
    }
}

void start_test_cpp()
{
    test_at_random_cpp();
    test_io_cpp();
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
