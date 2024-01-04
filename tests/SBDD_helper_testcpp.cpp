/*
One header library for SAPPOROBDD C++ version test code

Copyright (c) 2017 -- 2023 Jun Kawahara

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <iostream>
#include <fstream>

#define BDD_CPP
#include "bddc.h"
#include "BDD.h"
#include "ZBDD.h"
#include "BDDCT.h"
#define SBDDH_BDDCT

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

void printSet(const std::set<bddvar>& s)
{
    std::set<bddvar>::const_iterator itor = s.begin();
    for ( ; itor != s.end(); ++itor) {
        std::cerr << *itor << " ";
    }
    std::cerr << std::endl;
}

llint getWeight(const std::set<bddvar>& s,
    const std::vector<llint>& weights)
{
    llint weight = 0;
    std::set<bddvar>::const_iterator itor = s.begin();
    for ( ; itor != s.end(); ++itor) {
        weight += weights[*itor];
    }
    return weight;
}

#ifdef SBDDH_GMP

void test_gmp_conversion()
{
    mpz_class z[17];
    z[0] = mpz_class(0);
    z[1] = mpz_class(1);
    z[2] = mpz_class(123);
    z[3] = mpz_class("2147483647"); // 2^31 - 1
    z[4] = mpz_class("2147483648"); // 2^31
    z[5] = mpz_class("2147483649"); // 2^31 + 1
    z[6] = mpz_class("4294967295"); // 2^32 - 1
    z[7] = mpz_class("4294967296"); // 2^32
    z[8] = mpz_class("4294967297"); // 2^32 + 1
    z[9] = mpz_class("9223372036854775807"); // 2^63 - 1
    z[10] = mpz_class("9223372036854775808"); // 2^63
    z[11] = mpz_class("9223372036854775809"); // 2^63 + 1
    z[12] = mpz_class("18446744073709551615"); // 2^64 - 1
    z[13] = mpz_class("18446744073709551616"); // 2^64
    z[14] = mpz_class("18446744073709551617"); // 2^64 + 1
    // rand number: 738869 * 2^64 + 15614076732533780317
    z[15] = mpz_class("13629742961074435226742621");
    z[16] = mpz_class("340282366920938463463374607431768211455"); // 2^128 - 1

    ullint z_ullint[17] = {
        0, 1, 123, 2147483647, 2147483648, 2147483649,
        4294967295, 4294967296, 4294967297, 9223372036854775807ull,
        9223372036854775808ull, 9223372036854775809ull,
        18446744073709551615ull, 0, 1, 15614076732533780317ull,
        18446744073709551615ull
    };

    for (int i = 0; i < 17; ++i) {
        test(sbddh_mpz_to_ullint(z[i]) == z_ullint[i]);
    }

    for (int i = 0; i < 13; ++i) { /* check only for i < 13 */
        test(z[i] == sbddh_ullint_to_mpz(z_ullint[i]));
    }

    for (int i = 0; i < 10; ++i) { /* check only for i < 10 */
        test(z[i] == sbddh_llint_to_mpz(static_cast<llint>(z_ullint[i])));
    }
}

#endif

void test_BDD_functions()
{
    BDD b1 = BDDvar(1) | BDDvar(2);
    BDD b2 = ~b1;
    ZBDD z1 = ZBDD_ID(make_test_zbdd());
    ZBDD z2 = getPowerSet(2);
    ZBDD z3 = z1 + ZBDD(1);

    test(!isNegative(BDD(0)));
    test(isNegative(BDD(1)));
    test(!isNegative(ZBDD(0)));
    test(isNegative(ZBDD(1)));
    test(!isNegative(b1));
    test(isNegative(b2));
    test(!isNegative(z1));
    test(isNegative(z2));
    test(isNegative(z3));

    test(isConstant(BDD(0)));
    test(isConstant(BDD(1)));
    test(isConstant(ZBDD(0)));
    test(isConstant(ZBDD(1)));
    test(!isConstant(b1));
    test(!isConstant(b1));
    test(!isConstant(z1));
    test(!isConstant(z2));
    test(!isConstant(z3));

    test(isTerminal(BDD(0)));
    test(isTerminal(BDD(1)));
    test(isTerminal(ZBDD(0)));
    test(isTerminal(ZBDD(1)));
    test(!isTerminal(b1));
    test(!isTerminal(b1));
    test(!isTerminal(z1));
    test(!isTerminal(z2));
    test(!isTerminal(z3));

    test(takeNot(b1) == b2);
    test(takeNot(b2) == b1);
    test(takeNot(z1) == z3);
    test(takeNot(z3) == z1);

    test(addNot(b1) == b2);
    test(addNot(b2) == b2);
    test(addNot(b2) != b1);
    test(addNot(z1) == z3);
    test(addNot(z3) == z3);
    test(addNot(z3) != z1);

    test(eraseNot(b2) == b1);
    test(eraseNot(b1) == b1);
    test(eraseNot(b1) != b2);
    test(eraseNot(z3) == z1);
    test(eraseNot(z1) == z1);
    test(eraseNot(z1) != z3);

#ifdef B_64
    test(is64BitVersion());
#else
    test(!is64BitVersion());
#endif

    test(!isEmptyMember(ZBDD(0)));
    test(isEmptyMember(ZBDD(1)));
    test(!isEmptyMember(z1));
    test(isEmptyMember(z2));
    test(isEmptyMember(z3));

    test_eq(getVar(BDD(0)), 0);
    test_eq(getVar(BDD(1)), 0);
    test_eq(getVar(b1), 2);
    test_eq(getVar(b2), 2);
    test_eq(getVar(z1), 3);
    test_eq(getVar(z2), 2);
    test_eq(getVar(z3), 3);

    test_eq(getLev(BDD(0)), 0);
    test_eq(getLev(BDD(1)), 0);
    test_eq(getLev(b1), bddlevofvar(2));
    test_eq(getLev(b2), bddlevofvar(2));
    test_eq(getLev(z1), bddlevofvar(3));
    test_eq(getLev(z2), bddlevofvar(2));
    test_eq(getLev(z3), bddlevofvar(3));

    BDD bs[6];
    bs[0] = makeNode(1, BDD(0), BDD(1));
    bs[1] = makeNode(1, BDD(1), BDD(0));
    bs[2] = makeNode(2, bs[0], bs[1]);
    bs[3] = addNot(bs[2]);
    bs[4] = makeNode(3, bs[2], bs[1]);
    bs[5] = addNot(bs[4]);
    test(getChild0(bs[4]) == bs[2]);
    test(getChild(bs[4], 0) == bs[2]);
    test(getChild1(bs[4]) == bs[1]);
    test(getChild(bs[4], 1) == bs[1]);
    test(getChild0Raw(bs[4]) == bs[2]);
    test(getChildRaw(bs[4], 0) == bs[2]);
    test(getChild1Raw(bs[4]) == bs[1]);
    test(getChildRaw(bs[4], 1) == bs[1]);
    test(getChild0(bs[5]) == bs[3]);
    test(getChild(bs[5], 0) == bs[3]);
    test(getChild1(bs[5]) == bs[0]);
    test(getChild(bs[5], 1) == bs[0]);
    test(getChild0Raw(bs[5]) == bs[2]);
    test(getChildRaw(bs[5], 0) == bs[2]);
    test(getChild1Raw(bs[5]) == bs[1]);
    test(getChildRaw(bs[5], 1) == bs[1]);

    ZBDD zs[6];
    zs[0] = makeNode(1, ZBDD(0), ZBDD(1));
    zs[1] = makeNode(1, ZBDD(1), ZBDD(1));
    zs[2] = makeNode(2, zs[0], zs[1]);
    zs[3] = addNot(zs[2]);
    zs[4] = makeNode(3, zs[2], zs[1]);
    zs[5] = addNot(zs[4]);
    test(getChild0(zs[4]) == zs[2]);
    test(getChild(zs[4], 0) == zs[2]);
    test(getChild1(zs[4]) == zs[1]);
    test(getChild(zs[4], 1) == zs[1]);
    test(getChild0Raw(zs[4]) == zs[2]);
    test(getChildRaw(zs[4], 0) == zs[2]);
    test(getChild1Raw(zs[4]) == zs[1]);
    test(getChildRaw(zs[4], 1) == zs[1]);
    test(getChild0(zs[5]) == zs[3]);
    test(getChild(zs[5], 0) == zs[3]);
    test(getChild1(zs[5]) == zs[1]);
    test(getChild(zs[5], 1) == zs[1]);
    test(getChild0Raw(zs[5]) == zs[2]);
    test(getChildRaw(zs[5], 0) == zs[2]);
    test(getChild1Raw(zs[5]) == zs[1]);
    test(getChildRaw(zs[5], 1) == zs[1]);

    BDD p5 = getPrimeNot(5);
    test(getChild0(p5) == BDD(1));
    test(getChild1(p5) == BDD(0));

    ZBDD s5 = getSingleton(5);
    test(getChild0(s5) == ZBDD(0));
    test(getChild1(s5) == ZBDD(1));
    test_eq(getVar(s5), 5);
    test_eq(getLev(s5), bddlevofvar(5));
    test_eq(s5.Card(), 1);

    ZBDD f1 = getSingleSet(2, 1, 2);
    test_eq(f1.Card(), 1);
    test(getChild1(f1) == getSingleton(1));
    test_eq(getVar(f1), 2);

    std::set<bddvar> se1;
    se1.insert(1);
    se1.insert(2);
    test(getSingleSet(se1) == f1);
    std::vector<bddvar> v1;
    v1.push_back(1);
    v1.push_back(2);
    test(getSingleSet(v1) == f1);

    f1 += getSingleSet(2, 1, 3);
    f1 += getSingleSet(2, 2, 3);

    test_eq(f1.Card(), 3);

    ZBDD g1 = makeNode(1, ZBDD(0), ZBDD(1));
    ZBDD g2 = makeNode(2, ZBDD(0), g1);
    ZBDD g3 = makeNode(2, g1, ZBDD(1));
    ZBDD g4 = makeNode(3, g2, g3);

    test(f1 == g4);

    ZBDD fs[10];
    fs[8] = makeNode(1, ZBDD(0), ZBDD(1));
    fs[9] = makeNode(1, ZBDD(1), ZBDD(1));
    fs[5] = makeNode(2, fs[8], fs[9]);
    fs[6] = makeNode(2, fs[9], ZBDD(1));
    fs[7] = makeNode(2, ZBDD(1), fs[9]);
    fs[3] = makeNode(2, ZBDD(0), ZBDD(1));
    fs[4] = makeNode(2, fs[9], fs[8]);
    fs[1] = makeNode(3, fs[5], fs[6]);
    fs[2] = makeNode(3, fs[6], fs[7]);

    std::vector<ZBDD> bps;
    test_eq(countNodes(bps, false), 0);
    bps.push_back(fs[1]);
    test_eq(countNodes(bps, false), 5);
    bps.push_back(ZBDD(-1));
    test_eq(countNodes(bps, false), 0);
    bps[1] = fs[2];
    test_eq(countNodes(bps, false), 7);
    bps.push_back(fs[3]);
    bps.push_back(fs[4]);
    test_eq(countNodes(bps, false), 9);
    bps.push_back(fs[8]);
    test_eq(countNodes(bps, false), 9);
    bps[2] = ZBDD(-1);
    test_eq(countNodes(bps, false), 0);

#if __cplusplus >= 201103L /* use C++ random class */
    std::mt19937 mt(1);
    for (int i = 0; i < 10; ++i) {
        ZBDD fx = getUniformlyRandomZBDD(i, mt);
        test(fx.Top() <= i);
        test(fx.Card() <= (1u << i)); /* card <= 2^i */
    }

    for (int i = 0; i < 10; ++i) {
        ZBDD fx = getRandomZBDDWithCard(i * 10 + 5, i * 10 + 10, mt);
        test(fx.Card() == static_cast<unsigned int>(i * 10 + 10));
    }
#endif

    ullint rand_state = 31415926535ull;
    for (int i = 0; i < 10; ++i) {
        BDD fx = getUniformlyRandomBDDX(i, &rand_state);
        test(fx.Top() <= i);
        ZBDD fy = getUniformlyRandomZBDDX(i, &rand_state);
        test(fy.Top() <= i);
        test(fy.Card() <= (1u << i)); /* card <= 2^i */
    }
    for (int i = 0; i < 10; ++i) {
        ZBDD fx = getRandomZBDDWithCardX(i * 10 + 5, i * 10 + 10, &rand_state);
        test(fx.Card() == static_cast<unsigned int>(i * 10 + 10));
    }
    {
        BDD fx = exampleBdd();
        ZBDD fy = exampleZbdd();
    }
    for (int i = 0; i < 10; ++i) {
        BDD fx = exampleBdd(i);
        ZBDD fy = exampleZbdd(i);
    }
}

void test_at_random_cpp()
{
    const size_t w = 30; /* number of variables */
    const size_t N = 1000; /* number of cardinality of the constructed ZDD */
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

    /* make array whose elements are distinct */
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
        test(isMember(f, ullint_to_varvec(ar[i])));
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
        test(isMember(f, ullint_to_varvec(c)) == found);
    }
    test(!isMemberZ(f, std::vector<bddvar>()));
    test(!isMember(f, std::vector<bddvar>()));

    f += ZBDD(1);
    test(isMemberZ(f, std::vector<bddvar>()));
    test(isMember(f, std::vector<bddvar>()));

    ofs.open(g_filename1);
    if (!ofs) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    exportZBDDAsKnuth(ofs, f, 0);
    ofs.close();

    ifs.open(g_filename1);
    g = importZBDDAsKnuth(ifs, 0);
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
    exportZBDDAsBinary(ofs, f);
    ofs.close();

    ifs.open(g_filename1, std::ifstream::binary);
    g = importZBDDAsBinary(ifs);
    ifs.close();
    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    test(f == g);

    f = getPowerSetIncluding(3, 2);
    test(f.Card() == 4);
    test(f.OnSet(2).Card() == 4);
    test(f.OffSet(2).Card() == 0);

    std::vector<bddvar> bv;
    bv.push_back(2);
    bv.push_back(3);
    f = getPowerSetIncluding(5, bv);
    test(f.Card() == 8);
    test(f.OnSet(3).Card() == 8);
    test(f.OffSet(3).Card() == 0);
    test(f.OnSet(4).Card() == 4);
    test(f.OffSet(4).Card() == 4);

    std::vector<bddvar> basev;
    basev.push_back(2);
    basev.push_back(4);
    basev.push_back(6);

    f = getPowerSetIncluding(basev, 3);
    test(f.Card() == 8);
    test(f.OnSet(3).Card() == 8);
    test(f.OffSet(3).Card() == 0);
    test(f.OnSet(4).Card() == 4);
    test(f.OffSet(4).Card() == 4);

    f = getPowerSetIncluding(basev, bv);
    test(f.Card() == 4);
    test(f.OnSet(3).Card() == 4);
    test(f.OffSet(3).Card() == 0);
    test(f.OnSet(4).Card() == 2);
    test(f.OffSet(4).Card() == 2);


    f = getPowerSetNotIncluding(3, 2);
    test(f.Card() == 4);
    test(f.OnSet(2).Card() == 0);
    test(f.OffSet(2).Card() == 4);

    f = getPowerSetNotIncluding(5, bv);
    test(f.Card() == 8);
    test(f.OnSet(3).Card() == 0);
    test(f.OffSet(3).Card() == 8);
    test(f.OnSet(4).Card() == 4);
    test(f.OffSet(4).Card() == 4);

    f = getPowerSetWithCard(basev, 0);
    test(f.Card() == 1);
    f = getPowerSetWithCard(basev, 1);
    test(f.Card() == 3);
    f = getPowerSetWithCard(basev, 2);
    test(f.Card() == 3);
    test(f == getSingleSet(2, 2, 4) +
            getSingleSet(2, 2, 6) +
            getSingleSet(2, 4, 6));

    f = getPowerSetWithCard(basev, 3);
    test(f.Card() == 1);

    f = getPowerSetWithCard(5, 2);
    test(f.Card() == 10);

    f = getPowerSetWithCard(3, 2);
    test(f == getSingleSet(2, 1, 2) +
            getSingleSet(2, 1, 3) +
            getSingleSet(2, 2, 3));

    std::vector<bddvar> sv;
    sv.push_back(1);
    sv.push_back(3);
    sv.push_back(5);
    f = makeDontCare(getSingleSet(basev), sv);
    /*printZBDDElements(std::cout, f); */
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

void test_io_all_func_cpp()
{
    const llint n = 3;
    const llint n_pow = (1ll << n);
    const llint n_powpow = (1ll << n_pow);

    std::set<bddvar> s;

    /* generate all n variable families. */
    for (int v = 0; v < n_powpow; ++v) {
        ZBDD f(0);
        BDD b(0);
        for (int w = 0; w < n_pow; ++w) {
            if (((v >> w) & 1) != 0) {
                s.clear();
                BDD bx(1);
                for (int x = 0; x < n; ++x) {
                    if (((w >> x) & 1) != 0) {
                        s.insert(x + 1);
                        bx &= BDDvar(x + 1);
                    } else {
                        bx &= ~BDDvar(x + 1);
                    }
                }
                f += getSingleSet(s);
                b |= bx;
            }
        }
        for (int i = 0; i < 4; ++i) {
            std::stringstream ss;
            if (i >= 2) {
                exportZBDDAsBinary(ss, f, i % 2 == 1);
            } else {
                exportBDDAsBinary(ss, b, i % 2 == 1);
            }
            ss.seekg(0);
            if (i >= 2) {
                ZBDD g1 = importZBDDAsBinary(ss);
                test(f == g1);
            } else {
                BDD g1 = importBDDAsBinary(ss);
                test(b == g1);
            }
        }
        for (int i = 0; i < 2; ++i) {
            std::stringstream ss;
            if (i == 1) {
                exportZBDDAsGraphillion(ss, f, n);
            } else {
                exportBDDAsGraphillion(ss, b, n);
            }
            ss.seekg(0);
            if (i == 1) {
                ZBDD g1 = importZBDDAsGraphillion(ss, n);
                test(f == g1);
            } else {
                BDD g1 = importBDDAsGraphillion(ss, n);
                test(b == g1);
            }
        }
        for (int i = 0; i < 4; ++i) {
            std::stringstream ss;
            if (i >= 2) {
                exportZBDDAsKnuth(ss, f, i % 2 == 1);
            } else {
                exportBDDAsKnuth(ss, b, i % 2 == 1);
            }
            ss.seekg(0);
            if (i >= 2) {
                ZBDD g1 = importZBDDAsKnuth(ss, i % 2 == 1);
                test(f == g1);
            } else {
                BDD g1 = importBDDAsKnuth(ss, i % 2 == 1);
                test(b == g1);
            }
        }
    }
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

template <typename InputIterator>
void check_iterator(const InputIterator& first,
                    const InputIterator& last)
{
    std::set<bddvar> s12; /* {1, 2} */
    s12.insert(1); s12.insert(2);
    std::set<bddvar> s13; /* {1, 3} */
    s13.insert(1); s13.insert(3);
    std::set<bddvar> s23; /* {2, 3} */
    s23.insert(2); s23.insert(3);
    std::set<bddvar> s123; /* {1, 2, 3} */
    s123.insert(1); s123.insert(2); s123.insert(3);

    InputIterator itor = std::find(first, last, s13);
    test(itor != last); /* found */

    itor = std::find(first, last, s123);
    test(itor == last); /* not found */

    test_eq(std::count(first, last, s12), 1);
    test_eq(std::count(first, last, s13), 1);
    test_eq(std::count(first, last, s23), 1);
    test_eq(std::count(first, last, s123), 0);

    test_eq(std::count_if(first, last, test_count_if_size1), 0);
    test_eq(std::count_if(first, last, test_count_if_size2), 3);
    test_eq(std::count_if(first, last, test_count_if_size3), 0);

    std::for_each(first, last, test_for_each);
}

enum CheckIteratorAllKind {KIND_MIN, KIND_MAX,
    KIND_RANDOM, KIND_DICT, KIND_RDICT};

template <typename InputIterator>
void check_iterator_all(const ZBDD& f, DDIndex<int>& dd_index,
                        const InputIterator& first,
                        const InputIterator& last,
                        CheckIteratorAllKind kind,
                        const std::vector<llint>& weights =
                            std::vector<llint>())
{
    if (first == last) {
        test(f == ZBDD(0) || f == ZBDD(-1));
        return;
    }
    std::set<bddvar> prev;
    bool is_first_time = true;
    const ullint check_th = 10000;
    bool is_check_elements = (dd_index.count() < check_th);
    ZBDD check_z(0);
    ullint count = 0;
    for (InputIterator itor = first;
            itor != last && count < check_th;
            ++itor, ++count) {
        test(isMember(f, *itor));
        if (is_check_elements) {
            check_z += getSingleSet(*itor);
        }
        if (is_first_time) {
            is_first_time = false;
        } else {
            switch (kind) {
            case KIND_MIN:
                test(getWeight(prev, weights) <= getWeight(*itor, weights));
                break;
            case KIND_MAX:
                test(getWeight(prev, weights) >= getWeight(*itor, weights));
                break;
            case KIND_RANDOM:
                /* nothing */
                break;
            case KIND_DICT:
                test(dd_index.getOrderNumber(prev) + 1 == dd_index.getOrderNumber(*itor));
                break;
            case KIND_RDICT:
                test(dd_index.getOrderNumber(prev) == dd_index.getOrderNumber(*itor) + 1);
                break;
            }
        }
        prev = *itor;
    }
    if (is_check_elements) {
        test_eq(f.Card(), count);
        test(f == check_z);
    }
}

#if __cplusplus >= 201103L

template <typename InputIterator>
void check_iterator_std(const ZBDD& f, DDIndex<int>& dd_index,
                        const InputIterator& first,
                        const InputIterator& last)
{
    if (dd_index.count() < 10000) {
        ullint seed = 1;
        std::set<bddvar> s = dd_index.sampleRandomlyA(&seed);
        InputIterator itor1 = std::find(first, last, s);
        test(itor1 != last); /* found */
        test_eq(std::count(first, last, s), 1);
        s.insert(99999999);
        InputIterator itor2 = std::find(first, last, s);
        test(itor2 == last);
        test_eq(std::count(first, last, s), 0);

        test_eq(std::count_if(first, last,
            [&f](const std::set<bddvar>& ss) {
                return isMember(f, ss);
            }), dd_index.count());

        std::for_each(first, last,
            [&f](const std::set<bddvar>& ss) {
                test(isMember(f, ss));
            });
    }
}

#endif

void test_elementIterator_cpp()
{
    ZBDD f = ZBDD_ID(make_test_zbdd());
    /* f is expected to be {{3, 2}, {3, 1}, {2, 1}} */
    ZBDD g = f + ZBDD(1);
    /* g is expected to be {{}, {3, 2}, {3, 1}, {2, 1}} */

    DDIndex<int> ddindex_f(f);
    DDIndex<int> ddindex_g(g);
    std::vector<llint> weights;
    weights.push_back(0); weights.push_back(-3);
    weights.push_back(2); weights.push_back(4);

    std::set<bddvar> s12; /* {1, 2} */
    s12.insert(1); s12.insert(2);
    std::set<bddvar> s13; /* {1, 3} */
    s13.insert(1); s13.insert(3);
    std::set<bddvar> s23; /* {2, 3} */
    s23.insert(2); s23.insert(3);
    std::set<bddvar> s123; /* {1, 2, 3} */
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

        DDIndex<int>::WeightIterator min_itor =
            ddindex_f.weight_min_begin(weights);
        test(min_itor != ddindex_f.weight_min_end());
        test(*min_itor == s12);

        ++min_itor;
        test(min_itor != ddindex_f.weight_min_end());
        test(*min_itor == s13);

        ++min_itor;
        test(min_itor != ddindex_f.weight_min_end());
        test(*min_itor == s23);

        ++min_itor;
        test(min_itor == ddindex_f.weight_min_end());

        DDIndex<int>::WeightIterator max_itor =
            ddindex_f.weight_max_begin(weights);
        test(max_itor != ddindex_f.weight_max_end());
        test(*max_itor == s23);

        ++max_itor;
        test(max_itor != ddindex_f.weight_max_end());
        test(*max_itor == s13);

        ++max_itor;
        test(max_itor != ddindex_f.weight_max_end());
        test(*max_itor == s12);

        ++max_itor;
        test(max_itor == ddindex_f.weight_max_end());

        DDIndex<int>::RandomIterator random_itor =
            ddindex_f.random_begin();
        ZBDD fa(0);
        test(random_itor != ddindex_f.random_end());
        test(isMember(f, *random_itor));
        fa += getSingleSet(*random_itor);

        ++random_itor;
        test(random_itor != ddindex_f.random_end());
        test(isMember(f, *random_itor));
        fa += getSingleSet(*random_itor);

        ++random_itor;
        test(random_itor != ddindex_f.random_end());
        test(isMember(f, *random_itor));
        fa += getSingleSet(*random_itor);

        ++random_itor;
        test(random_itor == ddindex_f.random_end());
        test(f == fa);

        DDIndex<int>::DictIterator dict_itor =
            ddindex_f.dict_begin();
        test(dict_itor != ddindex_f.dict_end());
        test(*dict_itor == s23);

        ++dict_itor;
        test(dict_itor != ddindex_f.dict_end());
        test(*dict_itor == s13);

        ++dict_itor;
        test(dict_itor != ddindex_f.dict_end());
        test(*dict_itor == s12);

        ++dict_itor;
        test(dict_itor == ddindex_f.dict_end());

        DDIndex<int>::DictIterator dict_ritor =
            ddindex_f.dict_rbegin();
        test(dict_ritor != ddindex_f.dict_rend());
        test(*dict_ritor == s12);

        ++dict_ritor;
        test(dict_ritor != ddindex_f.dict_rend());
        test(*dict_ritor == s13);

        ++dict_ritor;
        test(dict_ritor != ddindex_f.dict_rend());
        test(*dict_ritor == s23);

        ++dict_ritor;
        test(dict_ritor == ddindex_f.dict_rend());
    } /* itor destructed */

    {
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

        DDIndex<int>::WeightIterator min_itor =
            ddindex_g.weight_min_begin(weights);
        test(min_itor != ddindex_g.weight_min_end());
        test(*min_itor == s12);

        ++min_itor;
        test(min_itor != ddindex_g.weight_min_end());
        test_eq((*min_itor).size(), 0);

        ++min_itor;
        test(min_itor != ddindex_g.weight_min_end());
        test(*min_itor == s13);

        ++min_itor;
        test(min_itor != ddindex_g.weight_min_end());
        test(*min_itor == s23);

        ++min_itor;
        test(min_itor == ddindex_g.weight_min_end());

        DDIndex<int>::WeightIterator max_itor =
            ddindex_g.weight_max_begin(weights);
        test(max_itor != ddindex_g.weight_max_end());
        test(*max_itor == s23);

        ++max_itor;
        test(max_itor != ddindex_g.weight_max_end());
        test(*max_itor == s13);

        ++max_itor;
        test(max_itor != ddindex_g.weight_max_end());
        test_eq((*max_itor).size(), 0);

        ++max_itor;
        test(max_itor != ddindex_g.weight_max_end());
        test(*max_itor == s12);

        ++max_itor;
        test(max_itor == ddindex_g.weight_max_end());

        DDIndex<int>::RandomIterator random_itor =
            ddindex_g.random_begin();
        ZBDD ga(0);
        test(random_itor != ddindex_g.random_end());
        test(isMember(g, *random_itor));
        ga += getSingleSet(*random_itor);

        ++random_itor;
        test(random_itor != ddindex_g.random_end());
        test(isMember(g, *random_itor));
        ga += getSingleSet(*random_itor);

        ++random_itor;
        test(random_itor != ddindex_g.random_end());
        test(isMember(g, *random_itor));
        ga += getSingleSet(*random_itor);

        ++random_itor;
        test(random_itor != ddindex_g.random_end());
        test(isMember(g, *random_itor));
        ga += getSingleSet(*random_itor);

        ++random_itor;
        test(random_itor == ddindex_g.random_end());
        test(g == ga);

        DDIndex<int>::DictIterator dict_itor =
            ddindex_g.dict_begin();
        test(dict_itor != ddindex_g.dict_end());
        test_eq((*dict_itor).size(), 0);

        ++dict_itor;
        test(dict_itor != ddindex_g.dict_end());
        test(*dict_itor == s23);

        ++dict_itor;
        test(dict_itor != ddindex_g.dict_end());
        test(*dict_itor == s13);

        ++dict_itor;
        test(dict_itor != ddindex_g.dict_end());
        test(*dict_itor == s12);

        ++dict_itor;
        test(dict_itor == ddindex_g.dict_end());

        DDIndex<int>::DictIterator dict_ritor =
            ddindex_g.dict_rbegin();
        test(dict_ritor != ddindex_g.dict_rend());
        test(*dict_ritor == s12);

        ++dict_ritor;
        test(dict_ritor != ddindex_g.dict_rend());
        test(*dict_ritor == s13);

        ++dict_ritor;
        test(dict_ritor != ddindex_g.dict_rend());
        test(*dict_ritor == s23);

        ++dict_ritor;
        test(dict_ritor != ddindex_g.dict_rend());
        test_eq((*dict_ritor).size(), 0);

        ++dict_ritor;
        test(dict_ritor == ddindex_g.dict_rend());
    } /* itor destructed */

    /* bddempty test */
    {
        DDIndex<int> ddindex_0(ZBDD(0));

        ElementIteratorHolder eih(ZBDD(0));
        ElementIterator itor = eih.begin();
        test(itor == eih.end());

        DDIndex<int>::WeightIterator min_itor =
            ddindex_0.weight_min_begin(weights);
        test(min_itor == ddindex_0.weight_min_end());

        DDIndex<int>::WeightIterator max_itor =
            ddindex_0.weight_max_begin(weights);
        test(max_itor == ddindex_0.weight_max_end());

        DDIndex<int>::RandomIterator random_itor =
            ddindex_0.random_begin();
        test(random_itor == ddindex_0.random_end());

        DDIndex<int>::DictIterator dict_itor =
            ddindex_0.dict_begin();
        test(dict_itor == ddindex_0.dict_end());

        DDIndex<int>::DictIterator dict_ritor =
            ddindex_0.dict_rbegin();
        test(dict_ritor == ddindex_0.dict_rend());
    }

    /* bddsingle test */
    {
        DDIndex<int> ddindex_1(ZBDD(1));

        ElementIteratorHolder eih(ZBDD(1));
        ElementIterator itor = eih.begin();
        test(itor != eih.end());
        test_eq(itor->size(), 0);
        ++itor;
        test(itor == eih.end());

        DDIndex<int>::WeightIterator min_itor =
            ddindex_1.weight_min_begin(weights);
        test(min_itor != ddindex_1.weight_min_end());
        test_eq((*min_itor).size(), 0);
        ++min_itor;
        test(min_itor == ddindex_1.weight_min_end());

        DDIndex<int>::WeightIterator max_itor =
            ddindex_1.weight_max_begin(weights);
        test(max_itor != ddindex_1.weight_max_end());
        test_eq((*max_itor).size(), 0);
        ++max_itor;
        test(max_itor == ddindex_1.weight_max_end());

        DDIndex<int>::RandomIterator random_itor =
            ddindex_1.random_begin();
        test(random_itor != ddindex_1.random_end());
        test_eq((*random_itor).size(), 0);
        ++random_itor;
        test(random_itor == ddindex_1.random_end());

        DDIndex<int>::DictIterator dict_itor =
            ddindex_1.dict_begin();
        test(dict_itor != ddindex_1.dict_end());
        test_eq((*dict_itor).size(), 0);
        ++dict_itor;
        test(dict_itor == ddindex_1.dict_end());

        DDIndex<int>::DictIterator dict_ritor =
            ddindex_1.dict_rbegin();
        test(dict_ritor != ddindex_1.dict_rend());
        test_eq((*dict_ritor).size(), 0);
        ++dict_ritor;
        test(dict_ritor == ddindex_1.dict_rend());
    }

    /* algorithm start */
    {
        ElementIteratorHolder eih(f);
        check_iterator(eih.begin(), eih.end());
        check_iterator(ddindex_f.weight_min_begin(weights),
                        ddindex_f.weight_min_end());
        check_iterator(ddindex_f.weight_max_begin(weights),
                        ddindex_f.weight_max_end());
        check_iterator(ddindex_f.random_begin(),
                        ddindex_f.random_end());
        check_iterator(ddindex_f.dict_begin(),
                        ddindex_f.dict_end());
        check_iterator(ddindex_f.dict_rbegin(),
                        ddindex_f.dict_rend());
    }
}

int compare_dict_order(const std::set<bddvar>& s1,
                        const std::set<bddvar>& s2)
{
    std::vector<bddvar> v1;
    std::vector<bddvar> v2;

    std::set<bddvar>::const_iterator itor1 = s1.begin();
    for ( ; itor1 != s1.end(); ++itor1) {
        v1.push_back(*itor1);
    }
    std::set<bddvar>::const_iterator itor2 = s2.begin();
    for ( ; itor2 != s2.end(); ++itor2) {
        v2.push_back(*itor2);
    }
    std::sort(v1.rbegin(), v1.rend());
    std::sort(v2.rbegin(), v2.rend());

    size_t n = std::min(v1.size(), v2.size());
    for (size_t i = 0; i < n; ++i) {
        if (v1[i] > v2[i]) {
            return -1;
        } else if (v1[i] < v2[i]) {
            return 1;
        }
    }
    if (v1.size() < v2.size()) {
        return -1;
    } else if (v1.size() > v2.size()) {
        return 1;
    } else {
        return 0;
    }
}

void check_k_lightest(const ZBDD& f, DDIndex<int>& dd_index,
    const std::vector<llint>& weights)
{
    if (f != ZBDD(0)) {
        ullint card = dd_index.count();
        ullint step = card / 16 + 1;
        for (ullint k = 1; k <= card; k += step) {
            for (int strict = -1; strict <= 1; ++strict) {
                ZBDD f_k = dd_index.getKLightestZBDD(k, weights, strict);
                if (strict == -1) {
                    test(f_k.Card() <= k);
                } else if (strict == 0) {
                    test(f_k.Card() == k);
                } else {
                    test(f_k.Card() >= k);
                }
                ZBDD f_r = f - f_k;
                if (f_k != ZBDD(0) && f_r != ZBDD(0)) {
                    DDIndex<int> dd_index_k(f_k);
                    DDIndex<int> dd_index_r(f_r);
                    test(dd_index_k.getMaximum(weights)
                        <= dd_index_r.getMinimum(weights));
                }
                ZBDD f_h = dd_index.getKHeaviestZBDD(k, weights, strict);
                if (strict == -1) {
                    test(f_h.Card() <= k);
                } else if (strict == 0) {
                    test(f_h.Card() == k);
                } else {
                    test(f_h.Card() >= k);
                }
                ZBDD f_r2 = f - f_h;
                if (f_h != ZBDD(0) && f_r2 != ZBDD(0)) {
                    DDIndex<int> dd_index_h(f_h);
                    DDIndex<int> dd_index_r2(f_r2);
                    test(dd_index_h.getMinimum(weights)
                        >= dd_index_r2.getMaximum(weights));
                }
            }
        }
    }
}


llint v_to_w(bddvar v)
{
    return (llint)(v * v + 3 * v + 8);
}

void check_ddindex(const ZBDD& f, DDIndex<int>& dd_index)
{
    ZBDD g(0);
    test(f == dd_index.getZBDD());
    ullint card = dd_index.count();
#ifdef SBDDH_GMP
    test_eq(card, dd_index.countMP().get_si());
#endif
    for (ullint i = 0; i < card; ++i) {
        std::set<bddvar> varset = dd_index.getSet(i);
        if (i + 1 < card) {
            /* check dict order */
            test(compare_dict_order(varset, dd_index.getSet(i + 1)) < 0);
        }
        g += getSingleSet(varset);
        test(isMember(f, varset));
        llint order = dd_index.getOrderNumber(varset);
        test_eq(static_cast<llint>(i), order);
#ifdef SBDDH_GMP
        mpz_class i_mp((int)i);
        test(varset == dd_index.getSet(i_mp));
        test_eq(order, (llint)dd_index.getOrderNumberMP(varset).get_si());
#endif
    }
    test(f == g);

    dd_index.terminal(0).value = 0;
    dd_index.terminal(1).value = 1;
    std::set<bddvar> varused;
    for (int level = 1; level <= dd_index.height(); ++level) {
        for (ullint j = 0; j < dd_index.size(level); ++j) {
            DDNode<int> n = dd_index.getNode(level, j);
            varused.insert(bddtop(n.getBddp()));
            n.value = n.child(0).value + n.child(1).value;
        }
    }
    test_eq(static_cast<int>(card), dd_index.root().value);
    test(dd_index.usedVar() == varused);

    llint max_s = -99999999;
    llint min_s = 99999999;
    llint sum_s = 0;
    std::vector<llint> weights;
    weights.push_back(0); /* dummy */
    for (int v = 1; v <= f.Top(); ++v) {
        weights.push_back(v_to_w(v));
    }
    const int bound = 100, lower_bound = 50, upper_bound = 110;
    ZBDD f_le(0), f_lt(0), f_ge(0), f_gt(0), f_eq(0), f_ne(0), f_range(0);
    for (ullint i = 0; i < card; ++i) {
        std::set<bddvar> se = dd_index.getSet(static_cast<llint>(i));
        llint weight = getWeight(se, weights);
        if (weight > max_s) {
            max_s = weight;
        }
        if (weight < min_s) {
            min_s = weight;
        }
        sum_s += weight;
        if (weight <= bound) {
            f_le += getSingleSet(se);
        }
        if (weight < bound) {
            f_lt += getSingleSet(se);
        }
        if (weight >= bound) {
            f_ge += getSingleSet(se);
        }
        if (weight > bound) {
            f_gt += getSingleSet(se);
        }
        if (weight == bound) {
            f_eq += getSingleSet(se);
        } else {
            f_ne += getSingleSet(se);
        }
        if (lower_bound <= weight && weight <= upper_bound) {
            f_range += getSingleSet(se);
        }
    }
    std::set<bddvar> ss;
    test_eq(dd_index.getMaximum(weights, ss), max_s);
    test(isMember(f, ss));
    llint w = 0;
    std::set<bddvar>::const_iterator itor1 = ss.begin();
    for ( ; itor1 != ss.end(); ++itor1) {
        w += v_to_w(*itor1);
    }
    test_eq(w, max_s);

    ss.clear();
    test_eq(dd_index.getMinimum(weights, ss), min_s);
    test(isMember(f, ss));
    w = 0;
    itor1 = ss.begin();
    for ( ; itor1 != ss.end(); ++itor1) {
        w += v_to_w(*itor1);
    }
    test_eq(w, min_s);
    test_eq(sum_s, dd_index.getSum(weights));

#ifdef SBDDH_GMP
    test_eq(sum_s, dd_index.getSumMP(weights).get_si());
#endif

    /* std::cerr << f_le.Card() << ": " << f_lt.Card() << ": "
        << f_ge.Card() << ": "
        << f_gt.Card() << ": "
        << f_eq.Card() << ": "
        << f_ne.Card() << ": "
        << f_range.Card() << ": "
        << f.Card() << std::endl; */
    test(weightLE(f, bound, weights) == f_le);
    test(weightLT(f, bound, weights) == f_lt);
    test(weightGE(f, bound, weights) == f_ge);
    test(weightGT(f, bound, weights) == f_gt);
    test(weightEQ(f, bound, weights) == f_eq);
    test(weightNE(f, bound, weights) == f_ne);
    test(weightRange(f, lower_bound, upper_bound, weights) == f_range);

    ZBDD f_ks(0);
    for (llint i = -1; i <= static_cast<llint>(dd_index.count()) + 1; ++i) {
        if (i >= 1 && i <= static_cast<llint>(dd_index.count())) {
            f_ks += getSingleSet(dd_index.getSet(i - 1));
        }
        ZBDD ksets = dd_index.getKSetsZBDD((i >= 0 ? i : 0));
        test(f_ks == ksets);
        test(ksets - f == ZBDD(0)); /* ksets is included in f */
#ifdef SBDDH_GMP
        ZBDD ksets_mpz = dd_index.getKSetsZBDD(sbddh_llint_to_mpz(i));
        test(ksets_mpz == ksets);
#endif
    }

    check_k_lightest(f, dd_index, weights);

    /* check iterators */
    check_iterator_all(f, dd_index,
        dd_index.weight_min_begin(weights),
        dd_index.weight_min_end(),
        KIND_MIN,
        weights);

    check_iterator_all(f, dd_index,
        dd_index.weight_max_begin(weights),
        dd_index.weight_max_end(),
        KIND_MAX,
        weights);

    check_iterator_all(f, dd_index,
        dd_index.random_begin(),
        dd_index.random_end(),
        KIND_RANDOM);

    check_iterator_all(f, dd_index,
        dd_index.dict_begin(),
        dd_index.dict_end(),
        KIND_DICT);

    check_iterator_all(f, dd_index,
        dd_index.dict_rbegin(),
        dd_index.dict_rend(),
        KIND_RDICT);

#if __cplusplus >= 201103L

    check_iterator_std(f, dd_index,
        dd_index.weight_min_begin(weights),
        dd_index.weight_min_end());

    check_iterator_std(f, dd_index,
        dd_index.weight_max_begin(weights),
        dd_index.weight_max_end());

    check_iterator_std(f, dd_index,
        dd_index.random_begin(),
        dd_index.random_end());

    check_iterator_std(f, dd_index,
        dd_index.dict_begin(),
        dd_index.dict_end());

    check_iterator_std(f, dd_index,
        dd_index.dict_rbegin(),
        dd_index.dict_rend());

#endif

#ifdef SBDDH_GMP /* use GMP random */
    gmp_randclass random(gmp_randinit_default);
    random.seed(1);
    for (llint i = 0; i < 100; ++i) {
        std::set<bddvar> varset = dd_index.sampleRandomly(random);
        test(isMember(f, varset));
    }
#else

#if __cplusplus >= 201103L /* use C++ random class */
    std::mt19937 mt(1);
    for (llint i = 0; i < 100; ++i) {
        std::set<bddvar> varset = dd_index.sampleRandomly(mt);
        test(isMember(f, varset));
    }
#else
    for (llint i = 0; i < 100; ++i) {
        std::set<bddvar> varset = dd_index.sampleRandomly();
        test(isMember(f, varset));
    }
#endif

    ullint state = 1;
    for (llint i = 0; i < 100; ++i) {
        std::set<bddvar> varset = dd_index.sampleRandomlyA(&state);
        test(isMember(f, varset));
    }

#endif

}

void test_ddindex(bool exhaustive)
{
    ZBDD f1 = getPowerSetWithCard(5, 3);
    DDIndex<int> s1(f1);
    check_ddindex(f1, s1);
    ZBDD f2 = f1 + ZBDD(1) + ZBDD(1).Change(2)
                + ZBDD(1).Change(2).Change(3).Change(4).Change(5);
    DDIndex<int> s2(f2);
    check_ddindex(f2, s2);
    ZBDD f3 = getPowerSet(8);
    DDIndex<int> s3(f3);
    check_ddindex(f3, s3);

    if (exhaustive) {
        ullint seed = 1;
        for (int i = 0; i < 1000; ++i) {
            ZBDD f4 = getUniformlyRandomZBDDX(8, &seed);
            DDIndex<int> s4(f4);
            check_ddindex(f4, s4);
        }
    }

#ifdef SBDDH_GMP
    ZBDD fp = getPowerSetIncluding(100, 2);
    DDIndex<int> sp(fp);
    /* expect 2^99 */
    test(sp.countMP().get_str() == "633825300114114700748351602688");
    llint l_sum = 0;
    std::vector<llint> weights;
    weights.push_back(0); /* dummy */
    for (llint i = 1; i <= 100; ++i) {
        llint w = (i - 10) * (i - 10) + 15;
        weights.push_back(w);
        if (i != 2) {
            l_sum += w;
        }
    }
    mpz_class mp_sum(static_cast<int>(l_sum));
    mp_sum *= mpz_class("316912650057057350374175801344"); /* 2^98 */
    mp_sum += mpz_class((int)(weights[2]))
                * mpz_class("633825300114114700748351602688"); /* 2^99 */
    test(sp.getSumMP(weights) == mp_sum);

    std::set<bddvar> ss;
    ss.insert(1);
    ss.insert(2); /* last in dict order */
    mpz_class last_value("633825300114114700748351602687");
    test(sp.getOrderNumberMP(ss) == last_value);
    test(ss == sp.getSet(last_value));
    ss.erase(1); /* second last */
    last_value -= mpz_class(1);
    test(sp.getOrderNumberMP(ss) == last_value);
    test(ss == sp.getSet(last_value));
    ss.insert(1);
    ss.insert(3); /* third last */
    last_value -= mpz_class(1);
    test(sp.getOrderNumberMP(ss) == last_value);
    test(ss == sp.getSet(last_value));

#endif
}

void start_test_cpp(bool exhaustive)
{
#ifdef SBDDH_GMP
    test_gmp_conversion();
#endif
    test_BDD_functions();
    test_at_random_cpp();
    test_io_cpp();
    test_io_all_func_cpp();
    test_index_cpp();
    test_elementIterator_cpp();
    test_ddindex(exhaustive);
}

int main(int argc, char** argv)
{
    bool exhaustive = false;
    if (argc > 1 && std::string(argv[1]) == std::string("--ex")) {
        exhaustive = true;
    }
    start_test();
    start_test_cpp(exhaustive);

    printf("test passed!\n");
    return 0;
}
