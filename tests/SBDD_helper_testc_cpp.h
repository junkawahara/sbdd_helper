/*
One header library for SAPPOROBDD C/C++ version test code

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

#include "bddc.h"
#include "../devel/SBDD_helper.h"

#ifdef __cplusplus
using namespace sbddh;
#endif

#define test(b) testfunc((llint)(b), __FILE__, __LINE__)
#define test_eq(v1, v2) testfunc_eq((llint)(v1), (llint)(v2), __FILE__, __LINE__)

const char g_filename1[] = "SBDD_helper_testc_test_tempdata1.txt";
const char g_filename2[] = "SBDD_helper_testc_test_tempdata2.txt";
const char g_filename3[] = "SBDD_helper_testc_test_tempdata3.txt";

void testfunc(llint b, const char* filename, int error_line)
{
    if (b == 0) {
        fprintf(stderr, "not expected value at %s line %d\n", filename, error_line);
        exit(1);
    }
}

void testfunc_eq(llint v1, llint v2, const char* filename, int error_line)
{
    if (v1 != v2) {
        fprintf(stderr, "%lld != %lld at %s line %d\n", v1, v2, filename, error_line);
        exit(1);
    }
}

int is_expected_str(FILE* fp, const char* str)
{
    long v;
    int b, c;
    size_t len;
    char* buf;

    len = strlen(str);

    v = ftell(fp);
    if (v < 0) {
        fprintf(stderr, "ftell failed\n");
        exit(1);
    }
    if (v >= 1000000) {
        fprintf(stderr, "Too much temporary file size\n");
        exit(1);
    }
    if (fseek(fp, 0l, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    if (v != (long)len) {
        return 0;
    }
    buf = (char*)malloc((size_t)v + 1); /* +1 for '\0' */
    if (buf == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    c = 0;
    while (c < v && fgets(buf + c, (int)v + 1 - c, fp) != NULL) {
        c += (int)strlen(buf + c);
    }
    if (c == 0) {
        fprintf(stderr, "fgets failed!\n");
        exit(1);
    }
    if (fseek(fp, 0l, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    b = (strncmp(buf, str, (size_t)v) == 0 ? 1 : 0);
    if (!b) {
        fprintf(stderr, "unexpected str: %s\n", buf);
    }
    free(buf);
    return b;
}


void initialize(void)
{
    bddinit(1000ll, 10000000ll);

    bddnewvarn(100);
}

bddp make_test_zbdd(void)
{
    int i;
    bddp f;
    bddp g[7];

    g[0] = bddchange(bddsingle, 1);
    g[1] = bddchange(g[0], 2);

    g[2] = bddchange(bddsingle, 1);
    g[3] = bddchange(g[2], 3);

    g[4] = bddchange(bddsingle, 2);
    g[5] = bddchange(g[4], 3);

    g[6] = bddunion(g[1], g[3]);
    f = bddunion(g[5], g[6]);

    for (i = 0; i < 7; ++i) {
        bddfree(g[i]);
    }
    return f;
}

void test_MyVector(void)
{
    int N = 1024 * 1024 + 1024 + 1;
    int i;
    sbddextended_MyVector v0;
    sbddextended_MyVector v;
    sbddextended_MyVector v1;

    sbddextended_MyVector_initialize(&v0);
    test_eq(v0.count, 0);
    sbddextended_MyVector_deinitialize(&v0);

    sbddextended_MyVector_initialize(&v);
    sbddextended_MyVector_initialize(&v1);

    for (i = 0; i < N; ++i) {
        sbddextended_MyVector_add(&v, (llint)i * 2);
    }
    test_eq(v.count, N);

    for (i = 0; i < N; ++i) {
        test_eq(sbddextended_MyVector_get(&v, (llint)i), (llint)i * 2);
    }

    for (i = 0; i < N; i += 2) {
        sbddextended_MyVector_set(&v, (llint)i, (llint)i * 3);
    }

    for (i = 0; i < N; ++i) {
        if (i % 2 == 0) {
            test_eq(sbddextended_MyVector_get(&v, (llint)i), (llint)i * 3);
        } else {
            test_eq(sbddextended_MyVector_get(&v, (llint)i), (llint)i * 2);
        }
    }

    sbddextended_MyVector_copy(&v1, &v);

    test_eq(v1.count, v.count);

    for (i = 0; i < N; ++i) {
        if (i % 2 == 0) {
            test_eq(sbddextended_MyVector_get(&v1, (llint)i), (llint)i * 3);
        } else {
            test_eq(sbddextended_MyVector_get(&v1, (llint)i), (llint)i * 2);
        }
    }
    sbddextended_MyVector_pop_back(&v1);
    sbddextended_MyVector_pop_back(&v1);
    sbddextended_MyVector_pop_back(&v1);
    test_eq(v.count, v1.count + 3);

    sbddextended_MyVector_deinitialize(&v1);
    sbddextended_MyVector_deinitialize(&v);
}

void test_MyDict(void)
{
    int N = 1024 + 1;
    llint i;
    llint value = 0;
    sbddextended_MyDict d0;
    sbddextended_MyDict d;
    sbddextended_MyDict d1;

    sbddextended_MyDict_initialize(&d0);
    test_eq(d0.count, 0);
    sbddextended_MyDict_deinitialize(&d0);

    sbddextended_MyDict_initialize(&d);
    sbddextended_MyDict_initialize(&d1);

    for (i = 0; i < N; ++i) {
        sbddextended_MyDict_add(&d, i * 2, N - i * 2);
        sbddextended_MyDict_add(&d, 5 * N - i * 2, 3 * N + i);
    }
    test_eq(d.count, N * 2);

    for (i = 0; i < N; ++i) {
        test_eq(sbddextended_MyDict_find(&d, i * 2, &value), 1);
        test_eq(value, N - i * 2);
        test_eq(sbddextended_MyDict_find(&d, 5 * N - i * 2, &value), 1);
        test_eq(value, 3 * N + i);
    }

    for (i = 0; i < N; ++i) {
        test_eq(sbddextended_MyDict_find(&d, i * 2 - 1, &value), 0);
    }

    for (i = 0; i < N; ++i) {
        sbddextended_MyDict_add(&d, i * 2, 16 * N + i);
    }
    test_eq(d.count, N * 2);

    for (i = 0; i < N; ++i) {
        test_eq(sbddextended_MyDict_find(&d, i * 2, &value), 1);
        test_eq(value, 16 * N + i);
    }

    sbddextended_MyDict_copy(&d1, &d);

    test_eq(d1.count, d.count);

    for (i = 0; i < N; ++i) {
        test_eq(sbddextended_MyDict_find(&d1, i * 2, &value), 1);
        test_eq(value, 16 * N + i);
    }

    sbddextended_MyDict_deinitialize(&d1);
    sbddextended_MyDict_deinitialize(&d);
}

void test_MySet(void)
{
    int N = 1024 + 1;
    llint i;
    sbddextended_MySet s0;
    sbddextended_MySet s;
    sbddextended_MySet s1;

    sbddextended_MySet_initialize(&s0);
    test_eq(sbddextended_MySet_count(&s0), 0);
    sbddextended_MySet_deinitialize(&s0);

    sbddextended_MySet_initialize(&s);
    sbddextended_MySet_initialize(&s1);

    for (i = 0; i < N; ++i) {
        sbddextended_MySet_add(&s, i * 2);
        sbddextended_MySet_add(&s, 5 * N - i * 2);
    }
    test_eq(sbddextended_MySet_count(&s), N * 2);

    for (i = 0; i < N; ++i) {
        test_eq(sbddextended_MySet_exists(&s, i * 2), 1);
        test_eq(sbddextended_MySet_exists(&s, 5 * N - i * 2), 1);
    }

    for (i = 0; i < N; ++i) {
        test_eq(sbddextended_MySet_exists(&s, i * 2 - 1), 0);
    }

    for (i = 0; i < N; ++i) {
        sbddextended_MySet_add(&s, i * 2);
    }
    test_eq(sbddextended_MySet_count(&s), N * 2);

    for (i = 0; i < N; ++i) {
        test_eq(sbddextended_MySet_exists(&s, i * 2), 1);
    }

    sbddextended_MySet_copy(&s1, &s);

    test_eq(sbddextended_MySet_count(&s1),
        sbddextended_MySet_count(&s));

    for (i = 0; i < N; ++i) {
        test_eq(sbddextended_MySet_exists(&s1, i * 2), 1);
    }

    sbddextended_MySet_deinitialize(&s1);
    sbddextended_MySet_deinitialize(&s);
}

/* make zbdd representing {{2}} from file */
bddp construct_singleton(void)
{
    bddp f;
    FILE* fp;

    fp = fopen(g_filename1, "w");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened.\n");
        exit(1);
    }
    fputs("_i 2\n_o 1\n_n 1\n0 2 F T\n0\n", fp);
    fclose(fp);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened.\n");
        exit(1);
    }
    bddimportz(fp, &f, 1);
    fclose(fp);

    test_eq(bddcard(f), 1);
    test_eq(bddsize(f), 1);
    test_eq(bddgetvar(f), 2);
    test_eq(bddgetlev(f), 2);
    test(bddgetchild0z(f) == bddempty);
    test(bddgetchild1z(f) == bddsingle);
    return f;
}

void test_bddfunctions(void)
{
    bddp f, s2, g1, g2, g3, g4;
    bddp fs[10];
    bddp bps[10];
    int i;

    f = make_test_zbdd();

    test(bddisconstant(bddempty));
    test(bddisconstant(bddsingle));
    test(!bddisconstant(f));

    /* bddisnegative and bddtakenot will be tested at */
    /* test_getsingleandpowerset */

    test(bddis64bitversion());

    test(bddisterminal(bddempty));
    test(bddisterminal(bddsingle));
    test(!bddisterminal(f));

    test(!bddisemptymember(bddempty));
    test(bddisemptymember(bddsingle));
    test(!bddisemptymember(f));
    test(bddisemptymember(bddunion(f, bddsingle)));

    test_eq(bddgetvar(bddempty), 0);
    test_eq(bddgetvar(bddsingle), 0);
    test_eq(bddgetvar(f), 3);

    test_eq(bddgetlev(bddempty), 0);
    test_eq(bddgetlev(bddsingle), 0);
    test_eq(bddgetlev(f), 3);

    test(bddgetchild1z(bddgetchild0z(f))
            == bddgetchild0z(bddgetchild1z(f)));

    test(bddgetchild1g(bddgetchild0g(f, 1, 0), 1, 0)
            == bddgetchild0g(bddgetchild1g(f, 1, 0), 1, 0));

    test(bddgetchildg(bddgetchildg(f, 0, 1, 0), 1, 1, 0)
            == bddgetchildg(bddgetchildg(f, 1, 1, 0), 0, 1, 0));

    g1 = bddmakenodez(1, bddempty, bddsingle);
    g2 = bddmakenodez(2, bddempty, g1);
    g3 = bddmakenodez(2, g1, bddsingle);
    g4 = bddmakenodez(3, g2, g3);
    test(g4 == f);

    test(bddgetterminal(0, 0) == bddfalse);
    test(bddgetterminal(0, 1) == bddempty);
    test(bddgetterminal(1, 0) == bddtrue);
    test(bddgetterminal(1, 1) == bddsingle);

    s2 = construct_singleton();
    test(s2 == bddgetsingleton(2));
    test(bddrshift(s2, 1) == bddgetsingleton(1));

    for (i = 1; i <= 50; ++i) {
        test(bddlshift(s2, (bddvar)i) == bddgetsingleton((bddvar)(2 + i)));
    }

    /* test bddprimenot */
    f = bddat1(bddxor(bddprime((bddvar)1), bddprime((bddvar)2)),
               (bddvar)2); /* compute (x_1 xor x_2)|_{x_2 = 0}, i.e., bar(x_1) */
    test(f == bddprimenot((bddvar)1));

    for (i = 2; i <= 50; ++i) {
        f = bddat1(bddxor(bddprime((bddvar)i), bddprime((bddvar)1)),
                   (bddvar)1); /* compute (x_1 xor x_i)|_{x_i = 0}, i.e., bar(x_i) */
        test(f == bddprimenot((bddvar)i));
    }

    fs[8] = bddmakenodez(1, bddempty, bddsingle);
    fs[9] = bddmakenodez(1, bddsingle, bddsingle);
    fs[5] = bddmakenodez(2, fs[8], fs[9]);
    fs[6] = bddmakenodez(2, fs[9], bddsingle);
    fs[7] = bddmakenodez(2, bddsingle, fs[9]);
    fs[3] = bddmakenodez(2, bddempty, bddsingle);
    fs[4] = bddmakenodez(2, fs[9], fs[8]);
    fs[1] = bddmakenodez(3, fs[5], fs[6]);
    fs[2] = bddmakenodez(3, fs[6], fs[7]);

    bps[0] = fs[1];
    test_eq(bddcountnodes(NULL, 0, 0), 0);
    test_eq(bddcountnodes(bps, 1, 0), 5);
    bps[1] = bddnull;
    test_eq(bddcountnodes(bps, 2, 0), 0);
    bps[1] = fs[2];
    test_eq(bddcountnodes(bps, 2, 0), 7);
    bps[2] = fs[3];
    bps[3] = fs[4];
    test_eq(bddcountnodes(bps, 4, 0), 9);
    bps[4] = fs[8];
    test_eq(bddcountnodes(bps, 5, 0), 9);
    bps[2] = bddnull;
    test_eq(bddcountnodes(bps, 5, 0), 0);
}

void test_getsingleandpowerset(void)
{
    int N = 35;
    int i;
    bddp f, f0, f1;
    bddvar* vararr;
    FILE* fp;

    vararr = (bddvar*)malloc((size_t)(N + 3) * sizeof(bddvar));
    if (vararr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    for (i = 0; i < N; ++i) {
        vararr[i] = 2 * (bddvar)i + 1;
    }
    /* add duplicated values in purpose */
    vararr[N] = (bddvar)(2 * (135 % N) + 1);
    vararr[N + 1] = (bddvar)(2 * (223 % N) + 1);
    vararr[N + 2] = (bddvar)(2 * (157 % N) + 1);

    /* test getsingleset */
    f = bddgetsingleset(vararr, N + 3);

    test_eq(bddsize(f), N);
    test_eq(bddcard(f), 1);

    for (i = N - 1; i >= 0; --i) {
        test_eq(bddgetvar(f), 2 * i + 1);
        test_eq(bddgetlev(f), 2 * i + 1);
        test(bddgetchild0z(f) == bddempty);
        f = bddgetchild1z(f);
    }
    test(f == bddsingle);

    /* test getsinglesetv */

    f = bddgetsinglesetv(5, 2, 3, 5, 7, 11);

    test_eq(bddsize(f), 5);
    test_eq(bddcard(f), 1);

    fp = fopen(g_filename1, "w+");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    bddprintzbddelements(fp, f, "\n", " ");

    test(is_expected_str(fp, "11 7 5 3 2"));
    fclose(fp);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    test(bddgetsinglesetv(0) == bddsingle);

    /* test getpowerset */
    f = bddgetpowerset(vararr, N);

    test_eq(bddsize(f), N);
    test(N <= 38); /* bddcard cannot return a value more than 2^38 */
    test_eq(bddcard(f), (1llu << N)); /* 2^N */

    /* test also bddisnegative and bddtakenot */
    while (f != bddempty) {
        f0 = bddgetchild0zraw(f);
        f1 = bddgetchild1zraw(f);
        test(bddtakenot(f0) == f1);
        test(bddtakenot(f1) == f0);
        test(bddaddnot(f0) == f1);
        test(bddaddnot(f1) == f1);
        test(bdderasenot(f1) == f0);
        test(bdderasenot(f0) == f0);
        test(!bddisnegative(f0));
        test(bddisnegative(f1));
        f = f0;
    }
}

void test_ismemberz(void)
{
    bddp f;
    bddvar vararr[3];

    f = make_test_zbdd();

    vararr[0] = 1, vararr[1] = 2, vararr[2] = 3;
    test(!bddismemberz(f, vararr, 3));

    vararr[0] = 1, vararr[1] = 2;
    test(bddismemberz(f, vararr, 2));

    vararr[0] = 1, vararr[1] = 3;
    test(bddismemberz(f, vararr, 2));

    vararr[0] = 2, vararr[1] = 3;
    test(bddismemberz(f, vararr, 2));

    vararr[0] = 1;
    test(!bddismemberz(f, vararr, 1));

    vararr[0] = 2;
    test(!bddismemberz(f, vararr, 1));

    vararr[0] = 3;
    test(!bddismemberz(f, vararr, 1));

    test(!bddismemberz(f, vararr, 0));

    vararr[0] = 1, vararr[1] = 2, vararr[2] = 3;
    f = bddgetpowerset(vararr, 3);

    test(bddismemberz(f, vararr, 3));
    test(bddismemberz(f, vararr, 2));
    test(bddismemberz(f, vararr, 1));
    test(bddismemberz(f, vararr, 0));
}

void ullint_to_vararr(ullint v, bddvar* vararr, int* num)
{
    int count = 1;
    *num = 0;
    while (v > 0) {
        if ((v & 1llu) != 0) {
            vararr[*num] = (bddvar)count;
            ++(*num);
        }
        v >>= 1;
        ++count;
    }
}

void test_at_random(void)
{
    const size_t w = 30; /* number of variables */
    const size_t N = 1000; /* number of cardinality of the constructed ZDD */
    int i, j, num, found;
    ullint w_pow, c;
    ullint* ar;
    size_t sp = 0;
    bddp f, g, h;
    bddvar* vararr;
    FILE* fp;
    bddNodeIndex* node_index;

    w_pow = (1llu << w);

    ar = (ullint*)malloc(N * sizeof(ullint));
    if (ar == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    vararr = (bddvar*)malloc(w * sizeof(bddvar));
    if (vararr == NULL) {
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

    f = bddempty;

    for (i = 0; i < (int)N; ++i) {
        ullint_to_vararr(ar[i], vararr, &num);
        g = bddgetsingleset(vararr, num);
        h = bddunion(f, g);
        bddfree(f);
        bddfree(g);
        f = h;
    }
    test_eq(bddcard(f), N);

    for (i = 0; i < (int)N; ++i) {
        ullint_to_vararr(ar[i], vararr, &num);
        test(bddismemberz(f, vararr, num));
    }

    for (i = 0; i < 2 * (int)N; ++i) {
        c = (ullint)rand() & w_pow;
        found = 0;
        for (j = 0; j < (int)N; ++j) {
            if (ar[j] == c) {
                found = 1;
                break;
            }
        }
        ullint_to_vararr(c, vararr, &num);
        test_eq(bddismemberz(f, vararr, num), found);
    }
    test(!bddismemberz(f, NULL, 0));

    g = bddunion(f, bddsingle);
    bddfree(f);
    f = g;
    test(bddismemberz(f, NULL, 0));

    fp = fopen(g_filename1, "w+");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    bddexportzbddasknuth(fp, f, 0, NULL);

    if (fseek(fp, 0l, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    g = bddimportzbddasknuth(fp, 0, -1);

    test(f == g);

    fclose(fp);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    fp = fopen(g_filename1, "wb+");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    bddexportzbddasbinary(fp, f, 1, NULL);

    if (fseek(fp, 0l, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    g = bddimportzbddasbinary(fp, -1);

    test(f == g);

    fclose(fp);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    node_index = bddNodeIndex_makeIndexZ(g);
    test_eq(bddNodeIndex_count(node_index), bddcard(g));
    bddNodeIndex_destruct(node_index);

    node_index = bddNodeIndex_makeRawIndexZ(g);
    test_eq(bddNodeIndex_count(node_index), bddcard(g));
    test_eq(bddNodeIndex_size(node_index), bddsize(g));
    bddNodeIndex_destruct(node_index);

    free(vararr);
    free(ar);
}

void test_io(void)
{
    bddp f, g, h;
    FILE* fp1;
    FILE* fp2;
    bddvar vararr[3];
    const char* var_name_map[] = {"dummy", "e", "d", "c", "b", "a"};
    /*const unsigned char table1[] = {1, 1, 1, 1, 0, 1, 1, 1}; */
    /*const unsigned char table2[] = {0, 0, 1, 0, 0, 1, 0, 1}; */

    /* open as binary because treating '\n' as a normal charactor */
    fp1 = fopen(g_filename1, "wb+");
    if (fp1 == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = make_test_zbdd();
    bddprintzbddelements(fp1, f, "$", " ");

    test(is_expected_str(fp1, "3 2$3 1$2 1"));

    bddprintzbddelementswithmap(fp1, f, "$", " ", var_name_map);

    test(is_expected_str(fp1, "c d$c e$d e"));

    bddexportzbddasknuth(fp1, f, 0, NULL);
    test(is_expected_str(fp1, "#1\n2:3,4\n#2\n3:0,5\n4:5,1\n#3\n5:0,1\n"));

    fp2 = fopen(g_filename2, "w+");
    if (fp2 == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    fputs("#1\n2:3,4\n#2\n3:0,5\n4:5,1\n#3\n5:0,1\n", fp2);
    fclose(fp2);

    fp2 = fopen(g_filename2, "r");
    if (fp2 == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }

    g = bddimportzbddasknuth(fp2, 0, -1);

    fclose(fp2);
    if (remove(g_filename2) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    test(f == g);

    vararr[0] = 2, vararr[1] = 3, vararr[2] = 5;
    h = bddgetpowerset(vararr, 3);

    bddprintzbddelements(fp1, h, "!", " ");

    test(is_expected_str(fp1, "!5 3 2!5 3!5 2!5!3 2!3!2"));

    bddprintzbddelementswithmap(fp1, h, "\n", " ", var_name_map);

    test(is_expected_str(fp1, "\na c d\na c\na d\na\nc d\nc\nd"));

    fclose(fp1);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    /* need test bddoutputbddforgraphviz(stderr, f, NULL); here */

    /*f = bddtruthtabletobdd(table1, vararr, 3); */
    /*g = bddor(bddor(bddprime(2), bddprime(3)), bddnot(bddprime(5))); */
    /*test(f == g); */

    /*f = bddtruthtabletobdd(table2, vararr, 3); */
    /*g = bddor(bddand(bddprime(2), bddprime(5)), */
    /*          bddand(bddnot(bddor(bddprime(2), bddprime(5))), */
    /*                 bddprime(3))); */
    /*test(f == g); */

    /*bddconstructzbddfromelements_inner_getoneset("10 20 30 40 50", 14, " ", 1); */
    /*bddconstructzbddfromelements_inner_getoneset("10,2,30,4,50", 12, ",", 1); */
    /*bddconstructzbddfromelements_inner_getoneset("30!&20!&10", 10, "!&", 2); */
    /*bddconstructzbddfromelements_inner_getoneset("10!!30!!20", 10, "!!", 2); */
    /*bddconstructzbddfromelements_inner_getoneset("1", 1, "!&", 2); */
    /*bddconstructzbddfromelements(stdin, "!", ","); */

    fp1 = fopen(g_filename3, "w+");
    if (fp1 == NULL) {
        fprintf(stderr, "file cannot be opened.\n");
        exit(1);
    }
    fputs("1 2\n1 3\n2 3\n", fp1);

    if (fseek(fp1, 0l, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    f = make_test_zbdd();
    g = bddconstructzbddfromelements(fp1);
    fclose(fp1);
    if (remove(g_filename3) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
    test(f == g);

    fp1 = fopen(g_filename3, "w+");
    if (fp1 == NULL) {
        fprintf(stderr, "file cannot be opened.\n");
        exit(1);
    }
    fputs("\n1 2\n1 3\n2 3\n", fp1);

    if (fseek(fp1, 0l, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    f = make_test_zbdd();
    f = bddunion(f, bddsingle);
    g = bddconstructzbddfromelements(fp1);
    fclose(fp1);
    if (remove(g_filename3) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
    test(f == g);
}

void test_index(void)
{
    int i, count;
    bddp f;
    bddNodeIndex* node_index;
    bddNodeIterator* itor;
    bddvar vararr[40];

    f = make_test_zbdd();
    node_index = bddNodeIndex_makeIndexZ(f);
    test_eq(bddNodeIndex_count(node_index), 3);
    test_eq(bddNodeIndex_size(node_index), 4);

    itor = bddNodeIterator_make(node_index);
    count = 0;
    while (bddNodeIterator_hasNext(itor)) {
        bddNodeIterator_next(itor);
        ++count;
    }
    test_eq(count, 4);
    bddNodeIndex_destruct(node_index);

    for (i = 0; i < 40; ++i) {
        vararr[i] = (bddvar)i + 1;
    }
    f = bddgetpowerset(vararr, 40);
    node_index = bddNodeIndex_makeIndexZ(f);
    test_eq(bddNodeIndex_count(node_index), 1ll << 40);
    test_eq(bddNodeIndex_size(node_index), 40);
    bddNodeIndex_destruct(node_index);

    f = make_test_zbdd();
    node_index = bddNodeIndex_makeRawIndexZ(f);
    test_eq(bddNodeIndex_count(node_index), 3);
    test_eq(bddNodeIndex_size(node_index), 4);
    bddNodeIndex_destruct(node_index);

    f = bddgetpowerset(vararr, 40);
    node_index = bddNodeIndex_makeRawIndexZ(f);
    test_eq(bddNodeIndex_count(node_index), 1ll << 40);
    test_eq(bddNodeIndex_size(node_index), bddsize(f));
    bddNodeIndex_destruct(node_index);
}

void test_index_copy(void)
{
    int i;
    size_t j;
    llint value;
    bddp f, node;
    bddNodeIndex* node_index;
    bddNodeIndex* copied;
    ullint count, size;
    ullint size_at_level[8];

    f = make_test_zbdd();
    node_index = bddNodeIndex_makeIndexZ(f);
    count = bddNodeIndex_count(node_index);
    size = bddNodeIndex_size(node_index);
    test(node_index->height < 8);
    for (i = 1; i <= node_index->height; ++i) {
        size_at_level[i] = bddNodeIndex_sizeAtLevel(node_index, i);
    }

    copied = (bddNodeIndex*)malloc(sizeof(bddNodeIndex));
    if (copied == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    bddNodeIndex_copy(copied, node_index);

    /* destruct the source so that the copy must be independent of it */
    bddNodeIndex_destruct(node_index);
    free(node_index);

    test_eq(bddNodeIndex_count(copied), count);
    test_eq(bddNodeIndex_size(copied), size);
    test_eq(copied->is_zbdd, 1);
    test_eq(copied->is_raw, 0);
    for (i = 1; i <= copied->height; ++i) {
        test_eq(bddNodeIndex_sizeAtLevel(copied, i), size_at_level[i]);
        /* every node in level_vec_arr must be registered in node_dict_arr */
        /* with its position in the level as the value */
        for (j = 0; j < copied->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&copied->level_vec_arr[i],
                                                    (llint)j);
            test_eq(sbddextended_MyDict_find(&copied->node_dict_arr[i],
                                                (llint)node, &value), 1);
            test_eq(value, (llint)j);
        }
    }
    bddNodeIndex_destruct(copied);
    free(copied);

    /* the case that f is a terminal, where all the arrays are NULL */
    node_index = bddNodeIndex_makeIndexZ(bddsingle);
    copied = (bddNodeIndex*)malloc(sizeof(bddNodeIndex));
    if (copied == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    bddNodeIndex_copy(copied, node_index);
    bddNodeIndex_destruct(node_index);
    free(node_index);

    test_eq(bddNodeIndex_count(copied), 1);
    test_eq(bddNodeIndex_size(copied), 0);
    test_eq(copied->is_zbdd, 1);
    test(copied->node_dict_arr == NULL);
    test(copied->level_vec_arr == NULL);
    test(copied->offset_arr == NULL);
    test(copied->count_arr == NULL);
    bddNodeIndex_destruct(copied);
    free(copied);
}

void test_elementIterator(void)
{
    bddp f;
    bddvar* arr;
    bddElementIterator* itor;
    bddp g;

    f = make_test_zbdd();
    /* f is expected to be {{3, 2}, {3, 1}, {2, 1}} */
    arr = (bddvar*)malloc(bddgetlev(f) * sizeof(bddvar));

    itor = bddElementIterator_make(f);
    test(bddElementIterator_hasNext(itor) != 0);
    bddElementIterator_next(itor, arr);
    test_eq(arr[0], 3);
    test_eq(arr[1], 2);
    test_eq(arr[2], (bddvar)-1);

    test(bddElementIterator_hasNext(itor) != 0);
    bddElementIterator_next(itor, arr);
    test_eq(arr[0], 3);
    test_eq(arr[1], 1);
    test_eq(arr[2], (bddvar)-1);

    test(bddElementIterator_hasNext(itor) != 0);
    bddElementIterator_next(itor, arr);
    test_eq(arr[0], 2);
    test_eq(arr[1], 1);
    test_eq(arr[2], (bddvar)-1);

    test(bddElementIterator_hasNext(itor) == 0);

    bddElementIterator_destruct(itor);

    g = bddunion(f, bddsingle);
    /* g is expected to be {{}, {3, 2}, {3, 1}, {2, 1}} */

    itor = bddElementIterator_make(g);
    test(bddElementIterator_hasNext(itor) != 0);
    bddElementIterator_next(itor, arr);
    test_eq(arr[0], 3);
    test_eq(arr[1], 2);
    test_eq(arr[2], (bddvar)-1);

    test(bddElementIterator_hasNext(itor) != 0);
    bddElementIterator_next(itor, arr);
    test(bddElementIterator_hasNext(itor) != 0);
    bddElementIterator_next(itor, arr);
    test(bddElementIterator_hasNext(itor) != 0);
    bddElementIterator_next(itor, arr);
    test_eq(arr[0], (bddvar)-1);
    test(bddElementIterator_hasNext(itor) == 0);

    bddElementIterator_destruct(itor);

    /* bddempty test */
    itor = bddElementIterator_make(bddempty);
    test(bddElementIterator_hasNext(itor) == 0);
    bddElementIterator_destruct(itor);

    /* bddsingle test */
    itor = bddElementIterator_make(bddsingle);
    test(bddElementIterator_hasNext(itor) != 0);
    bddElementIterator_next(itor, arr);
    test_eq(arr[0], (bddvar)-1);
    test(bddElementIterator_hasNext(itor) == 0);
    bddElementIterator_destruct(itor);

    free(arr);
}

void test_bddbinaryformat_f(bddp f)
{
    bddp g;
    FILE* fp;

    fp = fopen(g_filename1, "wb+");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    bddexportzbddasbinary(fp, f, 1, NULL);

    if (fseek(fp, 0l, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    g = bddimportzbddasbinary(fp, -1);

    fclose(fp);
    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    test(f == g);
}

/* buf の先頭 len バイトだけからなるファイルを読み込ませ、
   bddnull が返ることを確認する */
void test_bddbinaryformat_truncated_len(const unsigned char* buf, long len)
{
    bddp g;
    FILE* fp;

    fp = fopen(g_filename1, "wb");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    if (len > 0) {
        if (fwrite(buf, (size_t)1, (size_t)len, fp) != (size_t)len) {
            fprintf(stderr, "fwrite failed\n");
            exit(1);
        }
    }
    fclose(fp);

    fp = fopen(g_filename1, "rb");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    g = bddimportzbddasbinary(fp, -1);
    fclose(fp);

    test(g == bddnull);
}

/* 切り詰められたバイナリを読み込んでも、未初期化の値を使わずに
   bddnull を返すことを確認する */
void test_bddbinaryformat_truncated(void)
{
    bddp f;
    FILE* fp;
    long file_size, len;
    unsigned char* buf;

    f = make_test_zbdd();

    fp = fopen(g_filename1, "wb+");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    bddexportzbddasbinary(fp, f, 1, NULL);
    file_size = ftell(fp);
    test(file_size > 0);

    if (fseek(fp, 0l, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    buf = (unsigned char*)malloc((size_t)file_size);
    if (buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    if (fread(buf, (size_t)1, (size_t)file_size, fp) != (size_t)file_size) {
        fprintf(stderr, "fread failed\n");
        exit(1);
    }
    fclose(fp);

    fprintf(stderr, "(the following \"Unexpected end\" messages are expected)\n");

    /* ヘッダ部・レベルごとのノード数・根 ID・ノード列の各所で切り詰める */
    for (len = 0; len < file_size; len = len * 2 + 1) {
        test_bddbinaryformat_truncated_len(buf, len);
    }
    test_bddbinaryformat_truncated_len(buf, file_size / 4);
    test_bddbinaryformat_truncated_len(buf, file_size / 2);
    test_bddbinaryformat_truncated_len(buf, file_size * 3 / 4);
    test_bddbinaryformat_truncated_len(buf, file_size - 1);

    fprintf(stderr, "(end of the expected messages)\n");

    free(buf);
    bddfree(f);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

/* buf の offset バイト目の uint64 を value に書き換えたファイルを
   読み込ませ、bddnull が返ることを確認する */
void test_bddbinaryformat_corrupted_at(const unsigned char* buf, long file_size,
                                       long offset, ullint value)
{
    bddp g;
    FILE* fp;
    unsigned char* buf2;

    buf2 = (unsigned char*)malloc((size_t)file_size);
    if (buf2 == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    memcpy(buf2, buf, (size_t)file_size);
    memcpy(buf2 + offset, &value, sizeof(ullint));

    fp = fopen(g_filename1, "wb");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    if (fwrite(buf2, (size_t)1, (size_t)file_size, fp) != (size_t)file_size) {
        fprintf(stderr, "fwrite failed\n");
        exit(1);
    }
    fclose(fp);
    free(buf2);

    fp = fopen(g_filename1, "rb");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    g = bddimportzbddasbinary(fp, -1);
    fclose(fp);

    test(g == bddnull);
}

/* 範囲外のノード ID を含むバイナリを読み込んでも、未初期化の bddp を
   使ったり領域外を読んだりせずに bddnull を返すことを確認する */
void test_bddbinaryformat_corrupted(void)
{
    bddp f;
    FILE* fp;
    long file_size, header_size, root_id_offset, node_offset;
    ullint max_level;
    unsigned char* buf;

    /* ヘッダのサイズ（'BDD' + version + type + number_of_arcs
       + number_of_terminals + number_of_bits_for_level
       + number_of_bits_for_id + use_negative_arcs + max_level
       + number_of_roots + reserved）*/
    header_size = 3 + 1 + 1 + 2 + 4 + 1 + 1 + 1 + 8 + 8 + 64;

    f = make_test_zbdd();

    fp = fopen(g_filename1, "wb+");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    bddexportzbddasbinary(fp, f, 1, NULL);
    file_size = ftell(fp);
    test(file_size > header_size);

    if (fseek(fp, 0l, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    buf = (unsigned char*)malloc((size_t)file_size);
    if (buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    if (fread(buf, (size_t)1, (size_t)file_size, fp) != (size_t)file_size) {
        fprintf(stderr, "fread failed\n");
        exit(1);
    }
    fclose(fp);

    /* max_level はヘッダの 14 バイト目から 8 バイト */
    memcpy(&max_level, buf + 14, sizeof(ullint));
    root_id_offset = header_size + (long)max_level * 8;
    node_offset = root_id_offset + 8;
    /* 1ノードは 0-child と 1-child の ID（各 8 バイト）からなる */
    test_eq((file_size - node_offset) % 16, 0);
    test(file_size > node_offset);

    fprintf(stderr, "(the following \"out of range\" messages are expected)\n");

    /* 根の ID が範囲外 */
    test_bddbinaryformat_corrupted_at(buf, file_size, root_id_offset,
                                      0xfffffffffffffffeull);
    /* 先頭ノードの 0-child / 1-child が範囲外 */
    test_bddbinaryformat_corrupted_at(buf, file_size, node_offset,
                                      0xfffffffffffffffeull);
    test_bddbinaryformat_corrupted_at(buf, file_size, node_offset + 8,
                                      0xfffffffffffffffeull);
    /* 先頭ノードの 0-child / 1-child がまだ読んでいないノード（自分自身）を指す。
       否定枝を使うので ID は 2 倍されている */
    test_bddbinaryformat_corrupted_at(buf, file_size, node_offset, 2ull * 2ull);
    test_bddbinaryformat_corrupted_at(buf, file_size, node_offset + 8, 2ull * 2ull);

    fprintf(stderr, "(end of the expected messages)\n");

    free(buf);
    bddfree(f);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

void test_bddbinaryformat(void)
{
    bddp f;

    test_bddbinaryformat_f(bddempty);
    test_bddbinaryformat_f(bddsingle);

    f = bddgetsingleton(1);
    test_bddbinaryformat_f(f);
    bddfree(f);

    f = make_test_zbdd();
    test_bddbinaryformat_f(f);
    bddfree(f);

    test_bddbinaryformat_truncated();
    test_bddbinaryformat_corrupted();
}

/* 空のファイルを読み込んでも、未初期化の buf を参照せずに
   bddnull を返すことを確認する */
void test_graphillionformat_empty(void)
{
    bddp f;
    FILE* fp;

    fp = fopen(g_filename1, "w");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    fclose(fp);

    fprintf(stderr, "(the following \"Unexpected end\" messages are expected)\n");

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportbddasgraphillion(fp, -1);
    fclose(fp);
    test(f == bddnull);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportzbddasgraphillion(fp, -1);
    fclose(fp);
    test(f == bddnull);

    fprintf(stderr, "(end of the expected messages)\n");

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

/* ファイル中の最大レベルより大きい root_level を指定しても、必要な数の
   変数が作られ、指定した根のレベルで復元されることを確認する */
void test_graphillionformat_root_level(void)
{
    const int root_level = 120;
    bddp f, g;
    FILE* fp;

    /* ノードは 1 つだけ（graphillion のレベル 1 = 根のレベル）。
       ファイル中の最大レベルは 1 で、root_level より小さい */
    fp = fopen(g_filename1, "w");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    fprintf(fp, "0 1 B T\n.\n");
    fclose(fp);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportzbddasgraphillion(fp, root_level);
    fclose(fp);

    test(f != bddnull);
    test((int)bddvarused() >= root_level);
    g = bddgetsingleton(bddvaroflev((bddvar)root_level));
    test(f == g);
    bddfree(f);
    bddfree(g);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportbddasgraphillion(fp, root_level);
    fclose(fp);

    test(f != bddnull);
    g = bddprime(bddvaroflev((bddvar)root_level));
    test(f == g);
    bddfree(f);
    bddfree(g);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

/* content の内容のファイルを graphillion 形式として読み込ませ、
   bddnull が返ることを確認する */
void test_graphillionformat_corrupted_content(const char* content)
{
    bddp f;
    FILE* fp;

    fp = fopen(g_filename1, "w");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    fputs(content, fp);
    fclose(fp);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportzbddasgraphillion(fp, -1);
    fclose(fp);
    test(f == bddnull);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportbddasgraphillion(fp, -1);
    fclose(fp);
    test(f == bddnull);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

/* 未登録の子ノード ID を含むファイルを読み込んでも、未初期化値を bddp として
   使わずに bddnull を返すことを確認する */
void test_graphillionformat_corrupted(void)
{
    fprintf(stderr, "(the following \"is not found\" messages are expected)\n");

    /* 0-child (9) が定義されていない */
    test_graphillionformat_corrupted_content("0 1 9 T\n.\n");
    /* 1-child (9) が定義されていない */
    test_graphillionformat_corrupted_content("0 1 B 9\n.\n");
    /* 子ノード (1) が自分より後に定義されている（前方参照） */
    test_graphillionformat_corrupted_content("0 1 1 T\n1 2 B T\n.\n");

    fprintf(stderr, "(end of the expected messages)\n");
}

/* content の内容のファイルを Knuth 形式として読み込ませ、
   bddnull が返ることを確認する */
void test_knuthformat_empty_content(const char* content)
{
    bddp f;
    FILE* fp;

    fp = fopen(g_filename1, "w");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    fputs(content, fp);
    fclose(fp);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportbddasknuth(fp, 0, -1);
    fclose(fp);
    test(f == bddnull);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportzbddasknuth(fp, 0, -1);
    fclose(fp);
    test(f == bddnull);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

/* ノードが 1 つも含まれないファイルを読み込んでも、bddnode_buf の
   領域外を読まずに bddnull を返すことを確認する */
void test_knuthformat_empty(void)
{
    fprintf(stderr, "(the following \"Unexpected end\" messages are expected)\n");

    /* 空のファイル */
    test_knuthformat_empty_content("");
    /* レベルのヘッダ行のみ */
    test_knuthformat_empty_content("#1\n");
    /* ノード行のないヘッダ行の並び */
    test_knuthformat_empty_content("#1\n#2\n");

    fprintf(stderr, "(end of the expected messages)\n");
}

/* content の内容のファイルを Knuth 形式として読み込ませ、
   bddnull が返ることを確認する */
void test_knuthformat_corrupted_content(const char* content, int is_hex)
{
    bddp f;
    FILE* fp;

    fp = fopen(g_filename1, "w");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    fputs(content, fp);
    fclose(fp);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportbddasknuth(fp, is_hex, -1);
    fclose(fp);
    test(f == bddnull);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportzbddasknuth(fp, is_hex, -1);
    fclose(fp);
    test(f == bddnull);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

/* 範囲外の子ノード ID を含むファイルを読み込んでも、bddnode_buf の
   領域外を読まずに bddnull を返すことを確認する */
void test_knuthformat_corrupted(void)
{
    fprintf(stderr, "(the following \"out of range\" messages are expected)\n");

    /* 0-child (9) がノード数を超えている */
    test_knuthformat_corrupted_content("#1\n2:9,1\n", 0);
    /* 1-child (9) がノード数を超えている */
    test_knuthformat_corrupted_content("#1\n2:0,9\n", 0);
    /* 0-child (2) が自分自身 */
    test_knuthformat_corrupted_content("#1\n2:2,1\n", 0);
    /* 0-child (2) が自分 (3) より小さい（まだ構築されていないノード） */
    test_knuthformat_corrupted_content("#1\n2:0,1\n#2\n3:2,0\n", 0);
    /* ノード 3 の構築後にノード 2 でエラーになる（構築済みノードの解放） */
    test_knuthformat_corrupted_content("#1\n2:9,1\n#2\n3:0,1\n", 0);
    /* 16 進表記で、符号付きに変換すると負になる 0-child */
    test_knuthformat_corrupted_content("#1\n2:ffffffffffffffff,1\n", 1);

    fprintf(stderr, "(end of the expected messages)\n");
}

/* content の内容のファイルを要素形式として読み込ませ、
   bddnull が返ることを確認する */
void test_elementsformat_out_of_range_content(const char* content)
{
    bddp f;
    FILE* fp;

    fp = fopen(g_filename1, "w");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    fputs(content, fp);
    fclose(fp);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddconstructzbddfromelements(fp);
    fclose(fp);
    test(f == bddnull);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

/* 使用中の変数の個数を超える変数番号を含むファイルを読み込んでも、
   異常終了せずに bddnull を返すことを確認する
   （変数番号 999999 は bddvarmax を超えるので、常に範囲外である） */
void test_elementsformat_out_of_range(void)
{
    fprintf(stderr, "(the following \"out of range\" messages are expected)\n");

    /* 変数番号 0 は存在しない */
    test_elementsformat_out_of_range_content("0\n");
    /* 変数番号が使用中の変数の個数を超えている */
    test_elementsformat_out_of_range_content("999999\n");
    /* 2 行目でエラーになる（構築済みの ZBDD の解放） */
    test_elementsformat_out_of_range_content("1 2\n1 999999\n");
    /* 最終行に改行がない場合 */
    test_elementsformat_out_of_range_content("1 2\n1 999999");
    /* int に収まらない変数番号（桁あふれ） */
    test_elementsformat_out_of_range_content("99999999999999999999\n");
    test_elementsformat_out_of_range_content("1 2\n1 99999999999999999999\n");

    fprintf(stderr, "(end of the expected messages)\n");
}

/* 最終行に改行のないファイルを、行長超過と誤判定せずに読み込めることを
   確認する（C 版の readLine） */
void test_readline_no_newline_at_end(void)
{
    bddp f, g;
    FILE* fp;

    /* Knuth 形式。レベル 1 のノードが 1 つだけで、最終行に改行がない */
    fp = fopen(g_filename1, "w");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    fputs("#1\n2:0,1", fp);
    fclose(fp);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportzbddasknuth(fp, 0, -1);
    fclose(fp);
    g = bddgetsingleton(bddvaroflev(1));
    test(f == g);
    bddfree(f);
    bddfree(g);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportbddasknuth(fp, 0, -1);
    fclose(fp);
    g = bddprime(bddvaroflev(1));
    test(f == g);
    bddfree(f);
    bddfree(g);

    /* graphillion 形式。ノードが 1 つだけで、最終行に改行がない */
    fp = fopen(g_filename1, "w");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    fputs("0 1 B T", fp);
    fclose(fp);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportzbddasgraphillion(fp, -1);
    fclose(fp);
    g = bddgetsingleton(bddvaroflev(1));
    test(f == g);
    bddfree(f);
    bddfree(g);

    fp = fopen(g_filename1, "r");
    if (fp == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = bddimportbddasgraphillion(fp, -1);
    fclose(fp);
    g = bddprime(bddvaroflev(1));
    test(f == g);
    bddfree(f);
    bddfree(g);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

void start_test(void)
{
    srand(1);

    initialize();

    test_MyVector();
    test_MyDict();
    test_MySet();
    test_bddfunctions();
    test_getsingleandpowerset();
    test_ismemberz();
    test_io();
    test_at_random();
    test_index();
    test_index_copy();
    test_elementIterator();
    test_bddbinaryformat();
    test_graphillionformat_empty();
    test_graphillionformat_root_level();
    test_graphillionformat_corrupted();
    test_knuthformat_empty();
    test_knuthformat_corrupted();
    test_elementsformat_out_of_range();
    test_readline_no_newline_at_end();
}
