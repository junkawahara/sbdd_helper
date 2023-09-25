# One header library for SAPPOROBDD

[SAPPOROBDD](https://github.com/Shin-ichi-Minato/SAPPOROBDD) を使用する際に便利な関数を提供しています。
ライブラリは1つのヘッダファイルだけから構成されているため、
インクルードするだけで使用できます。
C/C++ 言語の両方に対応しています。

## 免責事項

本プログラムの使用に関して、作者は一切の責任を負いません。本プログラムは [SAPPOROBDD](https://github.com/Shin-ichi-Minato/SAPPOROBDD) と同様、[MIT ライセンス](https://en.wikipedia.org/wiki/MIT_License)で提供しています。ソースコードの一部に [GMP (GNU Multiple Precision Arithmetic Library)](https://gmplib.org/) を用いています。GMP のライセンスは LGPL v3 です。

## リファレンス

[リファレンス](documents/reference.md)

## 機能の例

* ZDD が表す集合族の集合を1つずつ出力
* DD binary/graphillion/Knuth/ 形式のファイルの読み込みと書き込み
* graphviz や SVG による ZDD の描画
* 様々な BDD/ZDD を構築
    * べき集合（3 要素の例では、{{}, {1}, {2},...,{1,2,3}} の8通り）
    * n 変数で要素の大きさが k である集合全体
    * n 変数で指定した要素を必ず含む/含まない集合全体
    * n 変数の一様ランダムな ZDD
* 様々な便利な関数
    * メンバシップ関数（ある集合が集合族に含まれるかを判定）
    * 子ノードを取得する関数
    * 複数の BDD/ZDD からなるノード数を取得する関数
    * 重み最大化、最小化
    * 辞書順と集合の相互変換
    * [GMP (GNU Multiple Precision Arithmetic Library)](https://gmplib.org/) を用いた、2^64 を超える要素の数え上げ、重み和の計算、ランダムサンプリング


## インストール

[SBDD_helper.h ファイル](SBDD_helper.h) をダウンロードして、任意の場所に配置してください
（C/C++ 両方とも同じファイルです）。

または、SAPPOROBDD と SAPPOROBDD helper（と他のライブラリ）を同時にインストールできる [dd_package](https://github.com/junkawahara/dd_package) を用いるのも便利です。

C 言語（bddc.c のみを使用）の場合は以下のように、bddc.h ヘッダのインクルードの後に SBDD_helper.h をインクルードしてください。
```
#include "bddc.h"
#include "SBDD_helper.h"
```

C++ 言語（BDD, ZBDD クラスを使用）の場合、以下のように、BDD.h や ZBDD.h ヘッダのインクルードの後に SBDD_helper.h をインクルードしてください。
```
#include "ZBDD.h"
#include "SBDD_helper.h"
```

C++ 版では、ライブラリはすべて名前空間 sbddh の中で定義されています。ソースコードの初めに以下を記述するか、関数に `sbddh::` を付けてください。

```
using namespace sbddh;
```

## GMP について

ソースコードの一部で、任意桁精度の整数を扱うためのライブラリ [GMP (GNU Multiple Precision Arithmetic Library)](https://gmplib.org/) を用いています。
GMP を利用した機能を用いる場合、USE_GMP マクロを定義します。例えば、ソースコードで `SBDD_helper.h` をインクルードする前に、以下のようにマクロを定義してください。

```
#define USE_GMP
#include "SBDD_helper.h"
```

または、g++ でコンパイルしている場合、コンパイルオプション -D を用いて定義することもできます。

```
g++ -DUSE_GMP program.cpp
```

# C++ 言語版簡易チュートリアル

以下では簡単な使用例を紹介します。
[dd_package](https://github.com/junkawahara/dd_package) に書かれている通りに
パッケージを導入して、`main.cpp` を開いて、main 関数を以下のものに置き換え、
example1 関数を追加します。

```
#include "ZBDD.h"
#include "SBDD_helper.h"

using namespace sbddh;

// dd_package を用いている場合、↑は main.cpp に既に記述されている

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

int main()
{
    example1();
    return 0;
}
```

実行の出力は以下の通りになります。

```
{}, {3, 2, 1}, {3, 2}, {3, 1}, {3}, {2, 1}, {2}, {1}
{1, 3} は f に含まれます
```

getPowerSet、getChild、zstr、isMember が helper の関数です。
このように、helper は様々な便利な関数を提供します。

## BDD/ZDD 構築

BDD/ZDD の構築関数は以下のものがあります
（以下では引数の const は省略）。
vector、set は std 標準ライブラリです。T はイテレータを持つ任意のコンテナ（vector, set など）です。

ZBDD に関する関数

|関数名|機能|
|------|----|
|`getSingleton(bddvar v)`|{{v}} を表す ZBDD を構築|
|`getSingleSet(T& variables)`|variables からなる集合1つを表す ZBDD を構築|
|`getSingleSet(int n, ...)`|引数で指定した要素からなる集合1つを表す ZBDD を構築|
|`getSingleSet(T& variables)`|variables のべき集合を表す ZBDD を構築|
|`getPowerSet(int n)`| n 変数のべき集合を表す ZBDD を構築|
|`getPowerSetIncluding(略)`|指定したべき集合の中から、指定した要素をすべて含む集合を表す ZBDD を構築|
|`getPowerSetNotIncluding(略)`|指定したべき集合の中から、指定した要素をすべて含む集合を表す ZBDD を構築|
|`getPowerSetWithCard(略)`|指定したべき集合の中から、指定した大きさの集合すべてを表す ZBDD を構築|
|`makeDontCare(ZBDD& f, T& variables)`|f の各要素について、variables の各要素を含む集合と含まない集合の両方からなる ZBDD を構築|
|`getUniformlyRandomZBDD(int level, T& random_engine)`|level 個の変数をもつランダムな ZBDD を構築|

`getPowerSetIncluding(略)` などの詳細は[リファレンス](documents/reference.md) を参照してください。

## 子ノードの取得

BDD/ZDD の根ノードの子ノードを取得する関数は以下のものがあります。

|関数名|機能|
|------|----|
|`getChild0(const BDD& f)`| f の 0-枝側を取得|
|`getChild0(const ZBDD& f)`| f の 0-枝側を取得|
|`getChild0Raw(const BDD& f)`| f の 0-枝側を取得（否定枝表現）|
|`getChild0Raw(const ZBDD& f)`| f の 0-枝側を取得（否定枝表現）|
|`getChild1(const BDD& f)`| f の 1-枝側を取得|
|`getChild1(const ZBDD& f)`| f の 1-枝側を取得|
|`getChild1Raw(const BDD& f)`| f の 1-枝側を取得（否定枝表現）|
|`getChild1Raw(const ZBDD& f)`| f の 1-枝側を取得（否定枝表現）|
|`getChild(const BDD& f, int child)`| f の child-枝側を取得|
|`getChild(const ZBDD& f, int child)`| f の child-枝側を取得|
|`getChildRaw(const BDD& f, int child)`| f の child-枝側を取得（否定枝表現）|
|`getChildRaw(const ZBDD& f, int child)`| f の child-枝側を取得（否定枝表現）|

## 判定関数

bool 値を返す判定関数は以下のものがあります。

|関数名|機能|
|------|----|
|`is64BitVersion()`| |
|`isValid(const BDD& f)`| |
|`isTerminal(const BDD& f)`| |
|`isEmptyMember(const ZBDD& f)`| |
|`isMember(const ZBDD& f, const T& variables)`| |

## 値の取得

値を取得する関数は以下のものがあります。

|関数名|機能|
|------|----|
|`getVar(const BDD& f)`| f の根ノードの変数番号を取得|
|`getLev(const BDD& f)`| f の根ノードのレベルを取得|
|`countNodes(T& dds, bool is_raw)`| 複数の BDD/ZBDD dds からなるノード数を取得|

## インデックス

BDD/ZBDD に値を記憶させることで、各種の計算を効率よく行うことができます。

```
// この関数の前に example1() を呼び出して SAPPOROBDD を初期化する必要がある。

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
```

出力結果は以下の通りです。
```
nodes = 9
elems = 10
6th set = {2 3 4 }
order = 6
ランダムサンプリング成功
max = 9
```

## 入出力

BDD/ZBDD を各種の入出力形式に変換できます。

|関数名|機能|
|------|----|
|`constructZBDDFromElements`| ファイルやストリームから要素の集合を読み取り、ZBDD に変換|
|`printZBDDElements`| ZBDD が表す要素を出力|
|`importBDDAsBinary`| BDD/ZBDD を DD バイナリ形式で読み取り|
|`exportBDDAsBinary`| BDD/ZBDD を DD バイナリ形式で書き込み|
|`importBDDAsGraphillion`| BDD/ZBDD を graphillion 形式で読み取り|
|`exportBDDAsGraphillion`| BDD/ZBDD を graphillion 形式で書き込み|
|`importBDDAsKnuth`| BDD/ZBDD を Knuth 形式で読み取り|
|`exportBDDAsKnuth`| BDD/ZBDD を Knuth 形式で書き込み|
|`exportZBDDAsGraphviz`| ZBDD を graphviz 描画用に出力|
|`exportZBDDAsSvg`| ZBDD を SVG 形式で描画|
