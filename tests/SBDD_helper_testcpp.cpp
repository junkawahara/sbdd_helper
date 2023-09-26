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

void test_BDD_functions()
{
    ZBDD f = getSingleSet(2, 1, 2);
    f += getSingleSet(2, 1, 3);
    f += getSingleSet(2, 2, 3);

    ZBDD g1 = makeNode(1, ZBDD(0), ZBDD(1));
    ZBDD g2 = makeNode(2, ZBDD(0), g1);
    ZBDD g3 = makeNode(2, g1, ZBDD(1));
    ZBDD g4 = makeNode(3, g2, g3);

    test(f == g4);

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

#if __cplusplus >= 201103L // use C++ random class
    std::mt19937 mt(1);
    for (int i = 0; i < 10; ++i) {
        ZBDD fx = getUniformlyRandomZBDD(i, mt);
        test(fx.Top() <= i);
        test(fx.Card() <= (1u << i)); // card <= 2^i
    }

    for (int i = 0; i < 10; ++i) {
        ZBDD fx = getRandomZBDDWithCard(i * 10 + 5, i * 10 + 10, mt);
        test(fx.Card() == static_cast<unsigned int>(i * 10 + 10));
    }
#endif
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
    f = getPowerSetWithCard(basev, 3);
    test(f.Card() == 1);

    f = getPowerSetWithCard(5, 2);
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

llint v_to_w(bddvar v)
{
    return (llint)(v * v + 3 * v + 8);
}

void printSet(const std::set<bddvar>& s)
{
    std::set<bddvar>::const_iterator itor = s.begin();
    for ( ; itor != s.end(); ++itor) {
        std::cerr << *itor << " ";
    }
    std::cerr << std::endl;
}

void check_ddindex(const ZBDD& f, DDIndex<int>& s)
{
    ZBDD g(0);
    llint card = s.count();
#ifdef USE_GMP
    test_eq(card, s.countMP().get_si());
#endif
    for (llint i = 0; i < card; ++i) {
        std::set<bddvar> varset = s.getSet(i);
        if (i < card - 1) {
            // check dict order
            test(compare_dict_order(varset, s.getSet(i + 1)) < 0);
        }
        g += getSingleSet(varset);
        test(isMember(f, varset));
        llint order = s.getOrderNumber(varset);
        test_eq(i, order);
#ifdef USE_GMP
        mpz_class i_mp((int)i);
        test(varset == s.getSet(i_mp));
        test_eq(order, (llint)s.getOrderNumberMP(varset).get_si());
#endif
    }
    test(f == g);

    s.terminal(0).value = 0;
    s.terminal(1).value = 1;
    for (int level = 1; level <= s.height(); ++level) {
        for (llint j = 0; j < s.size(level); ++j) {
            auto n = s.getNode(level, j);
            n.value = n.child(0).value + n.child(1).value;
        }
    }
    test(card == s.root().value);

    llint n = s.count();
    llint max_s = -99999999;
    llint min_s = 99999999;
    llint sum_s = 0;
    std::vector<llint> weights;
    weights.push_back(0); // dummy
    for (int v = 1; v <= f.Top(); ++v) {
        weights.push_back(v_to_w(v));
    }
    for (llint i = 0; i < n; ++i) {
        std::set<bddvar> se = s.getSet(i);
        llint weight = 0;
        std::set<bddvar>::const_iterator itor = se.begin();
        for ( ; itor != se.end(); ++itor) {
            weight += v_to_w(*itor);
        }
        if (weight > max_s) {
            max_s = weight;
        }
        if (weight < min_s) {
            min_s = weight;
        }
        sum_s += weight;
    }
    std::set<bddvar> ss;
    test_eq(s.getMaximum(weights, ss), max_s);
    test(isMember(f, ss));
    llint w = 0;
    std::set<bddvar>::const_iterator itor1 = ss.begin();
    for ( ; itor1 != ss.end(); ++itor1) {
        w += v_to_w(*itor1);
    }
    test_eq(w, max_s);

    ss.clear();
    test_eq(s.getMinimum(weights, ss), min_s);
    test(isMember(f, ss));
    w = 0;
    itor1 = ss.begin();
    for ( ; itor1 != ss.end(); ++itor1) {
        w += v_to_w(*itor1);
    }
    test_eq(w, min_s);
    test_eq(sum_s, s.getSum(weights));

#ifdef USE_GMP
    test_eq(sum_s, s.getSumMP(weights).get_si());
#endif

#ifdef USE_GMP // use GMP random
    gmp_randclass random(gmp_randinit_default);
    random.seed(1);
    for (llint i = 0; i < 100; ++i) {
        std::set<bddvar> varset = s.sampleRandomly(random);
        test(isMember(f, varset));
    }
#else

#if __cplusplus >= 201103L // use C++ random class
    std::mt19937 mt(1);
    for (llint i = 0; i < 100; ++i) {
        std::set<bddvar> varset = s.sampleRandomly(mt);
        test(isMember(f, varset));
    }
#else
    for (llint i = 0; i < 100; ++i) {
        std::set<bddvar> varset = s.sampleRandomly();
        test(isMember(f, varset));
    }
#endif

#endif

}

void test_ddindex()
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

    //std::mt19937 mt(0);
    //for (int i = 0; i < 1000; ++i) {
    //    ZBDD f4 = getUniformlyRandomZBDD(8, mt);
    //    DDIndex<int> s4(f4);
    //    check_ddindex(f4, s4);
    //}

#ifdef USE_GMP
    ZBDD fp = getPowerSetIncluding(100, 2);
    DDIndex<int> sp(fp);
    // expect 2^99
    test(sp.countMP().get_str() == "633825300114114700748351602688");
    llint l_sum = 0;
    std::vector<llint> weights;
    weights.push_back(0); // dummy
    for (llint i = 1; i <= 100; ++i) {
        llint w = (i - 10) * (i - 10) + 15;
        weights.push_back(w);
        if (i != 2) {
            l_sum += w;
        }
    }
    mpz_class mp_sum(static_cast<int>(l_sum));
    mp_sum *= mpz_class("316912650057057350374175801344"); // 2^98
    mp_sum += mpz_class((int)(weights[2]))
                * mpz_class("633825300114114700748351602688"); // 2^99
    test(sp.getSumMP(weights) == mp_sum);

    std::set<bddvar> ss;
    ss.insert(1);
    ss.insert(2); // last in dict order
    mpz_class last_value("633825300114114700748351602687");
    test(sp.getOrderNumberMP(ss) == last_value);
    test(ss == sp.getSet(last_value));
    ss.erase(1); // second last
    last_value -= mpz_class(1);
    test(sp.getOrderNumberMP(ss) == last_value);
    test(ss == sp.getSet(last_value));
    ss.insert(1);
    ss.insert(3); // third last
    last_value -= mpz_class(1);
    test(sp.getOrderNumberMP(ss) == last_value);
    test(ss == sp.getSet(last_value));

#endif
}

void start_test_cpp()
{
    test_BDD_functions();
    test_at_random_cpp();
    test_io_cpp();
    test_index_cpp();
    test_elementIterator_cpp();
    test_ddindex();
}

int main()
{
    start_test();
    start_test_cpp();

    printf("test passed!\n");
    return 0;
}
