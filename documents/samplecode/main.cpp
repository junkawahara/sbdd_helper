#include "SAPPOROBDD/include/ZBDD.h"
#include "sbdd_helper/SBDD_helper.h"

// Delete commentout if necessary.
//#include "tdzdd/DdEval.hpp"
#include "tdzdd/DdSpec.hpp"
//#include "tdzdd/DdSpecOp.hpp"
#include "tdzdd/DdStructure.hpp"

// Comment out if your program does not need the following headers.
#include "tdzdd/spec/SizeConstraint.hpp"
#include "tdzdd/eval/ToZBDD.hpp"
#include "tdzdd/spec/SapporoZdd.hpp"

using namespace tdzdd;
using namespace sbddh;

#include <vector>
#include <set>
#include <random>

void example1()
{
    // 初期化
    BDD_Init(1000, 1000);
    for (int i = 0; i < 100; ++i) {
        BDD_NewVar();
    }

    // 4 変数のべき集合を取得
    ZBDD f = getPowerSet(4);

    // f の根ノードの 0-枝側を取得
    ZBDD f0 = getChild(f, 0);

    // f0 の全要素を出力
    std::cout << zstr(f0) << std::endl;

    std::vector<bddvar> v;
    v.push_back(1); v.push_back(3); // v = {1, 3}
    if (isMember(f0, v)) { // {1, 3} が f0 に含まれるか判定
        std::cout << "{1, 3} は f に含まれます" << std::endl;
    }
}

void example2()
{
    ZBDD f = getPowerSetWithCard(5, 3);

    DDIndex<int> index(f);

    // f の（否定枝表現を用いない）ノード数
    std::cout << "nodes = " << index.size() << std::endl;

    // f の要素数
    std::cout << "elems = " << index.count() << std::endl;

    // 辞書順で 6 番目の集合を取得
    std::set<bddvar> varset = index.getSet(6);
    // 集合を出力
    std::set<bddvar>::const_iterator itor = varset.begin();
    std::cout << "6th set = {";
    while (itor != varset.end()) {
        std::cout << *itor << " ";
        ++itor;
    }
    std::cout << "}" << std::endl; // {2, 3, 4}

    // 集合 {2, 3, 4} の辞書順の番号を取得
    std::cout << "order = " << index.getOrderNumber(varset) << std::endl;

    // 一様ランダムサンプリング
    std::mt19937 mt(0);
    std::set<bddvar> varset2 = index.sampleRandomly(mt);
    if (isMember(f, varset2)) {
        std::cout << "ランダムサンプリング成功" << std::endl;
    }

    // llint は 64ビット符号付き整数型
    std::vector<llint> weights;
    for (int w = 0; w <= 4; ++w) {
        weights.push_back(w);
    }
    std::cout << "max = " << index.getMaximum(weights) << std::endl;
}

int main()
{
    example1();
    example2();
    return 0;
}
