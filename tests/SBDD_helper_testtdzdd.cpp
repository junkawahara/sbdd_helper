/*
One header library for SAPPOROBDD and TdZdd C++ version test code

Copyright (c) 2017 -- 2026 Jun Kawahara

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

/* SBDD_helper_tdzdd.h（SAPPOROBDD と TdZdd の相互変換）のテスト。
   ビルドには TdZdd が必要なため、make all には含めていない。
   tests ディレクトリで make testtdzdd TDZDDDIR=/path/to/TdZdd を実行する。 */

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#define BDD_CPP
#include "bddc.h"
#include "BDD.h"
#include "ZBDD.h"

#include "../devel/SBDD_helper.h"
#include "../SBDD_helper_tdzdd.h"

using namespace sbddh;

#define test(b) testfunc((b), __FILE__, __LINE__)

void testfunc(bool b, const char* filename, int error_line)
{
    if (!b) {
        fprintf(stderr, "not expected value at %s line %d\n",
                filename, error_line);
        exit(1);
    }
}

/* テスト用の ZBDD {{1, 2}, {1, 3}, {2, 3}} を作る */
ZBDD make_test_zbdd_cpp()
{
    return getSingleSet(2, 1, 2) + getSingleSet(2, 1, 3) + getSingleSet(2, 2, 3);
}

/* TdZdd の DD を SAPPOROBDD の BDD/ZBDD に変換する */
void test_tdzdd2sbdd()
{
    const int n = 5;
    tdzdd::DdStructure<2> dd(n); /* n 変数の全ての部分集合 */

    ZBDD f = tdzdd2szbdd(dd);
    test(f.Card() == (1u << n));
    test(f == getPowerSet(n));

    BDD b = tdzdd2sbdd(dd);
    test(b == BDD(1)); /* 恒真関数 */
}

/* SAPPOROBDD の ZBDD を TdZdd の DD に変換して戻すと元に戻る */
void test_szbdd2tdzdd()
{
    ZBDD f = make_test_zbdd_cpp();

    test(tdzdd2szbdd(sbdd2tdzdd(f)) == f);
    test(tdzdd2szbdd(szbdd2tdzdd(f)) == f);

    /* 終端のみの場合 */
    test(tdzdd2szbdd(szbdd2tdzdd(ZBDD(0))) == ZBDD(0));
    test(tdzdd2szbdd(szbdd2tdzdd(ZBDD(1))) == ZBDD(1));

    ZBDD g = getPowerSet(5);
    test(tdzdd2szbdd(szbdd2tdzdd(g)) == g);
}

/* SAPPOROBDD の BDD を TdZdd の DD に変換して戻すと元に戻る */
void test_sbdd2tdzdd()
{
    BDD b = (BDDvar(1) & BDDvar(2)) | BDDvar(3);

    test(tdzdd2sbdd(sbdd2tdzdd(b)) == b);
    test(tdzdd2sbdd(sbdd2tdzdd(BDDvar(1))) == BDDvar(1));
}

int main()
{
    BDD_Init(1000, 10000000);
    SBDDH_NewVar(10);

    test_tdzdd2sbdd();
    test_szbdd2tdzdd();
    test_sbdd2tdzdd();

    printf("test passed!\n");
    return 0;
}
