//
// One header library for SAPPOROBDD C/C++ version test code
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

#include "SBDD/bddc.h"
#include "SBDD_helper.h"

#define test(b) testfunc((llint)(b), __LINE__)
#define test_eq(v1, v2) testfunc_eq((llint)(v1), (llint)(v2), __LINE__)

const char g_filename1[] = "SBDD_helper_testc_test_tempdata1.txt";
const char g_filename2[] = "SBDD_helper_testc_test_tempdata2.txt";

void testfunc(llint b, int error_line)
{
    if (b == 0) {
        fprintf(stderr, "not expected value at line %d\n", error_line);
        exit(1);
    }
}

void testfunc_eq(llint v1, llint v2, int error_line)
{
    if (v1 != v2) {
        fprintf(stderr, "%lld != %lld at line %d\n", v1, v2, error_line);
        exit(1);
    }
}

int is_expected_str(FILE* fp, const char* str)
{
    long v;
    int b, c, len;
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
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    if (v != len) {
        return 0;
    }
    buf = (char*)malloc(v + 1); // +1 for '\0'
    if (buf == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    c = 0;
    while (c < v && fgets(buf + c, v + 1 - c, fp) != NULL) {
        c += strlen(buf + c);
    }
    if (c == 0) {
        fprintf(stderr, "fgets failed!\n");
        exit(1);
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    b = (strncmp(buf, str, v) == 0 ? 1 : 0);
    if (!b) {
        fprintf(stderr, "unexpected str: %s\n", buf);
    }
    free(buf);
    return b;
}


void initialize()
{
    int i;

    bddinit(1000ll, 10000000ll);

    for (i = 0; i < 100; ++i) {
        bddnewvar();
    }
}

bddp make_test_bdd()
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

void test_MyVector()
{
    int N = 1024 * 1024 + 1024 + 1;
    int i;
    sbddextended_MyVector v0;
    sbddextended_MyVector v;

    sbddextended_MyVector_initialize(&v0);
    test_eq(v0.count, 0);
    sbddextended_MyVector_deinitialize(&v0);


    sbddextended_MyVector_initialize(&v);

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

    sbddextended_MyVector_deinitialize(&v);
}

void test_MyDict()
{
    int N = 1024 + 1;
    llint i;
    llint value;
    sbddextended_MyDict d0;
    sbddextended_MyDict d;

    sbddextended_MyDict_initialize(&d0);
    test_eq(d0.count, 0);
    sbddextended_MyDict_deinitialize(&d0);

    sbddextended_MyDict_initialize(&d);

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

    sbddextended_MyDict_deinitialize(&d);
}

// make zbdd representing {{2}} from file
bddp construct_singleton()
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

void test_bddfunctions()
{
    bddp f, s2;
    int i;

    f = make_test_bdd();

    test(bddisconstant(bddempty));
    test(bddisconstant(bddsingle));
    test(!bddisconstant(f));

    // bddisnegative and takenot will be tested at
    // test_getsingleandpowerset

    test(bddis64bitversion());

    test(bddisterminal(bddempty));
    test(bddisterminal(bddsingle));
    test(!bddisterminal(f));

    test_eq(bddgetvar(bddempty), 0);
    test_eq(bddgetvar(bddsingle), 0);
    test_eq(bddgetvar(f), 3);

    test_eq(bddgetlev(bddempty), 0);
    test_eq(bddgetlev(bddsingle), 0);
    test_eq(bddgetlev(f), 3);

    test(bddgetchild1z(bddgetchild0z(f))
         == bddgetchild0z(bddgetchild1z(f)));

    s2 = construct_singleton();
    test(s2 == bddgetsingleton(2));
    test(bddrshift(s2, 1) == bddgetsingleton(1));

    for (i = 1; i <= 50; ++i) {
        test(bddlshift(s2, i) == bddgetsingleton(2 + i));
    }
}

void test_getsingleandpowerset()
{
    int N = 35;
    int i;
    bddp f, f0, f1;
    bddvar* vararr;
    FILE* fp;

    vararr = (bddvar*)malloc(N * sizeof(vararr));
    if (vararr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    for (i = 0; i < N; ++i) {
        vararr[i] = 2 * i + 1;
    }

    // test getsingleset
    f = bddgetsingleset(vararr, N);

    test_eq(bddsize(f), N);
    test_eq(bddcard(f), 1);

    for (i = N - 1; i >= 0; --i) {
        test_eq(bddgetvar(f), 2 * i + 1);
        test_eq(bddgetlev(f), 2 * i + 1);
        test(bddgetchild0z(f) == bddempty);
        f = bddgetchild1z(f);
    }
    test(f == bddsingle);

    // test getsinglesetv

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

    // test getpowerset
    f = bddgetpowerset(vararr, N);

    test_eq(bddsize(f), N);
    test(N <= 38); // bddcard cannot return a value more than 2^38
    test_eq(bddcard(f), (1llu << N)); // 2^N

    // test also bddisnegative and takenot
    while (f != bddempty) {
        f0 = bddgetchild0zraw(f);
        f1 = bddgetchild1zraw(f);
        test(takenot(f0) == f1);
        test(takenot(f1) == f0);
        test(!bddisnegative(f0));
        test(bddisnegative(f1));
        f = f0;
    }
}

void test_ismemberz()
{
    bddp f;
    bddvar vararr[3];

    f = make_test_bdd();

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

void int_to_vararr(int v, bddvar* vararr, int* num)
{
    int count = 1;
    *num = 0;
    while (v > 0) {
        if ((v & 1) != 0) {
            vararr[*num] = count;
            ++(*num);
        }
        v >>= 1;
        ++count;
    }
}

void test_at_random()
{
    int w = 30;
    int N = 1000;
    int i, j, c, num, found;
    int w_pow;
    int* ar;
    int sp = 0;
    bddp f, g, h;
    bddvar* vararr;
    FILE* fp;

    w_pow = (1llu << w);

    ar = (int*)malloc(N * sizeof(int));
    if (ar == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    vararr = (bddvar*)malloc(w * sizeof(bddvar));
    if (vararr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    // make array whose elements are distinct
    while (sp < N) {
        c = rand() % w_pow;
        if (c == 0) {
            continue;
        }
        for (i = 0; i < sp; ++i) {
            if (ar[i] == c) {
                break;
            }
        }
        if (i < sp) {
            continue;
        }
        ar[sp] = c;
        ++sp;
    }

    f = bddempty;

    for (i = 0; i < N; ++i) {
        int_to_vararr(ar[i], vararr, &num);
        g = bddgetsingleset(vararr, num);
        h = bddunion(f, g);
        bddfree(f);
        bddfree(g);
        f = h;
    }
    test_eq(bddcard(f), N);

    for (i = 0; i < N; ++i) {
        int_to_vararr(ar[i], vararr, &num);
        test(bddismemberz(f, vararr, num));
    }

    for (i = 0; i < 2 * N; ++i) {
        c = rand() & w_pow;
        found = 0;
        for (j = 0; j < N; ++j) {
            if (ar[j] == c) {
                found = 1;
                break;
            }
        }
        int_to_vararr(c, vararr, &num);
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
    bddwritezbddtofileknuth(fp, f, 0);

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed\n");
        exit(1);
    }
    g = bddconstructzbddfromfileknuth(fp, 0, -1);

    test(f == g);

    fclose(fp);

    if (remove(g_filename1) != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }

    free(vararr);
    free(ar);
}

void test_io()
{
    bddp f, g, h;
    FILE* fp1;
    FILE* fp2;
    bddvar vararr[3];
    const char* var_name_map[] = {"dummy", "e", "d", "c", "b", "a"};

    // open as binary because treating '\n' as a normal charactor
    fp1 = fopen(g_filename1, "wb+");
    if (fp1 == NULL) {
        fprintf(stderr, "file cannot be opened\n");
        exit(1);
    }
    f = make_test_bdd();
    bddprintzbddelements(fp1, f, "$", " ");

    test(is_expected_str(fp1, "3 2$3 1$2 1"));

    bddprintzbddelementswithmap(fp1, f, "$", " ", var_name_map);

    test(is_expected_str(fp1, "c d$c e$d e"));

    bddwritezbddtofileknuth(fp1, f, 0);
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

    g = bddconstructzbddfromfileknuth(fp2, 0, -1);

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
}

void start_test()
{
    srand(1);
    
    initialize();

    test_MyVector();
    test_MyDict();
    test_bddfunctions();
    test_getsingleandpowerset();
    test_ismemberz();
    test_io();
    test_at_random();
}
