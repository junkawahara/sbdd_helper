# SBDD_helper C++言語版リファレンス

# 提供しているクラス

* [DDNodeIndex](reference_cpp_DDNodeIndex.md)
* [ElementIterator と ElementIteratorHolder](reference_cpp_ElementIterator.md)

# 提供している関数

## isNegative

```
bool isNegative(const BDD& f)
bool isNegative(const ZBDD& f)
```
f が否定表現であるかを返す。f が否定表現であるなら true を、否定表現でないなら false を返す。

### 使用例

```
ZBDD f = ...;  // 何らかの方法で f を作成
if (isNegative(f)) {
   // f は否定表現である
   // ... 処理
}
```

## isConstant

```
bool isConstant(const BDD& f)
bool isConstant(const ZBDD& f)
```

f が定数関数（bddfalse, bddtrue, bddempty, bddsingle のいずれか）、すなわちBDD/ZBDDの終端ノードであるかを返す。f が定数関数であるなら true を、定数関数でないなら false を返す。

### 使用例

```
ZBDD f = ...;  // 何らかの方法で f を作成
if (isConstant(f)) {
   // f は定数関数である
   // ... 処理
}
```

## takeNot

```
BDD takeNot(const BDD& f)
ZBDD takeNot(const ZBDD& f)
```

f の否定 BDD を返す。
リファレンスカウンタは変化させない。


## addNot

```
BDD addNot(const BDD& f)
ZBDD addNot(const ZBDD& f)
```

f が否定であるときは f を返し、f が否定でないときは f の否定 BDD を返す。
リファレンスカウンタは変化させない。



## eraseNot

```
BDD eraseNot(const BDD& f)
ZBDD eraseNot(const ZBDD& f)
```

f が否定であるときは f の否定を外した BDD を返し、f が否定でないときは f を返す。
リファレンスカウンタは変化させない。

## is64BitVersion

```
bool is64BitVersion()
```

SAPPOROBDD の64ビット版でコンパイルが行われているかどうか判定する。正確には、B_64 マクロが定義されている場合、true を返し、されていない場合、false を返す。


## isTerminal

```
bool isTerminal(const BDD& f)
bool isTerminal(const ZBDD& f)
```

f が定数関数（bddfalse, bddtrue, bddempty, bddsingle のいずれか）、すなわちBDD/ZBDDの終端ノードであるかを返す。f が定数関数であるなら true を、定数関数でないなら false を返す。SAPPOROBDD は多値終端に対応できるように準備されているが、完全には対応していない。本関数は現在のところ、bddfalse, bddtrue, bddempty, bddsingle に対してのみ true を返す。

## isEmptyMember

```
bool isEmptyMember(const ZBDD& f)
```

f が表す集合族が空集合を含む場合は true を、そうでないなら false を返す。

## getVar

```
bddvar getVar(const BDD& f)
bddvar getVar(const ZBDD& f)
```

f の変数番号を返す。

## getLev

```
bddvar getLev(const BDD& f)
bddvar getLev(const ZBDD& f)
```

f の level を返す。

## getChild0

```
BDD getChild0(const BDD& f)
ZBDD getChild0(const ZBDD& f)
```

f が BDD/ZBDD ノードであるとき、f の 0-枝の子を返す。SAPPOROBDD ではBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の 0-枝の子を返す。

## getChild0Raw

```
BDD getChild0Raw(const BDD& f)
ZBDD getChild0Raw(const ZBDD& f)
```

f が BDD/ZBDDノードであるとき、f の 0-枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の 0-枝の子を返す。

## getChild1

```
BDD getChild1(const BDD& f)
ZBDD getChild1(const ZBDD& f)
```

f が BDD/ZBDDノードであるとき、f の 1-枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の 1-枝の子を返す。

## getChild1Raw

```
BDD getChild1Raw(const BDD& f)
ZBDD getChild1Raw(const ZBDD& f)
```

f が BDD/ZBDDノードであるとき、f の 1-枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の 1-枝の子を返す。


## getChild

```
BDD getChild(const BDD& f, int child)
ZBDD getChild(const ZBDD& f, int child)
```

f が BDD/ZBDDノードであるとき、f の child 引数で指定した枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の child-枝の子を返す。


## getChildRaw

```
BDD getChildRaw(const BDD& f, int child)
ZBDD getChildRaw(const ZBDD& f, int child)
```

f が BDD/ZBDDノードであるとき、f の child 引数で指定した枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の child-枝の子を返す。

## makeNode

```
BDD makeNode(bddvar v, const BDD& f0, const BDD& f1)
ZBDD makeNode(bddvar v, const ZBDD& f0, const ZBDD& f1)
```

根ノードの変数番号が v、0-枝側が f0、1-枝側が f1 である BDD/ZBDD を構築し、それを返す。
f0、f1 の根ノードのレベルは、変数番号 v のレベルより小さくなければならない。

## getPrimeNot

```
BDD getPrimeNot(bddvar v)
```

変数番号 v のリテラルの否定だけからなる論理関数に対応する BDD を返す。
bddprime(v) の否定を取った BDD と同じ結果となる。
変数番号 v は 1 から bddvarused() （現在使用している変数の数）以下である必要がある。

## getSingleton

```
ZBDD getSingleton(bddvar v)
```

引数で指定した変数番号 v だけを要素とする集合だけからなる集合族（すなわち {{v}}）を表すZBDDを返す。変数番号 v は 1 から bddvarused() （現在使用している変数の数）以下である必要がある。

## getSingleSet

```
template<typename T>
ZBDD getSingleSet(const T& variables)
```

引数で指定した variables が表す集合だけからなる集合族を表すZBDDを返す。すなわち、variables が [a1, a2,..., an] なら、{{a1, a2,..., an}} を表すZBDDを返す。variables  の型 T は std::vector<bddvar> や std::set<bddvar> など、bddvar 型のコンテナ型である。variables の各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。variables はソートされている必要はなく、重複する要素を含んでもよい（1つの要素とみなされる）。variables の大きさが0のときは空集合 (bddsingle) を返すが、空集合は bddsingle 定数から得られるため、bddgetsingleset(NULL, 0) を呼び出す実質的な意味は無い。

### 使用例

```
std::vector<bddvar> vararr;
vararr.push_back(2); vararr.push_back(3); vararr.push_back(5);
ZBDD f = getSingleSet(vararr); // f は {{2, 3, 5}} を表す ZBDD
```

## getSingleSet

```
ZBDD getSingleSet(int n, ...)
```

引数で指定した要素からなる集合だけからなる集合族を表すZBDDを返す。第1引数の n で要素の数を表す。第2引数以降を a1, a2,..., an とするとき、{{a1, a2,..., an}} を表すZBDDを返す。引数の個数は n + 1 でなければならない。各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。引数の各値 はソートされている必要はないが、重複する要素を含んではいけない。n = 0 を指定すると、空集合 (bddsingle) を返すが、空集合は bddsingle 定数から得られるため、bddgetsinglesetv(0) を呼び出す実質的な意味は無い。

### 使用例

```
ZBDD f = getSingleSet(3, 2, 3, 5); // f は {{2, 3, 5}} を表す ZBDD
```

## getPowerSet

```
template<typename T>
ZBDD getPowerSet(const T& variables)
ZBDD getPowerSet(int n)
```

引数で vector variables を指定した場合、variables（を集合とみなしたとき）のべき集合族を表すZBDDを返す。すなわち、variables が [a1, a2,..., an] なら、{a1, a2,..., an} のすべての部分集合（空集合、全体集合も含む）からなる集合族を表すZBDDを返す。引数で自然数 n を指定した場合、{1,...,n} のべき集合族を表すZBDDを返す。variables  の型 T は std::vector<bddvar> や std::set<bddvar> など、bddvar 型のコンテナ型である。variables の各要素や n は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。variables はソートされている必要はない。

### 使用例

```
std::vector<bddvar> vararr;
vararr.push_back(2); vararr.push_back(3); vararr.push_back(5);
// f は {{},{2},{3},{5},{2,3},{2,5},{3,5},{2,3,5}} を表す ZBDD
ZBDD f = getPowerSet(vararr);
// g は {{},{1},{2},{3},{1,2},{1,3},{2,3},{1,2,3}} を表す ZBDD
ZBDD g = getPowerSet(3);
```

## getPowerSetIncluding

```
template<typename T>
getPowerSetIncluding(const T& base_variables, const std::vector<bddvar>& target_variables)
template<typename T>
getPowerSetIncluding(const T& base_variables, const std::set<bddvar>& target_variables)
getPowerSetIncluding(const T& base_variables, int v)
```

base_variables（を集合とみなしたとき）のべき集合族のうち、第2引数で指定した target_variables をすべて含む、または、変数 v を含むZBDDを返す。すなわち、base_variables が [a1, a2,..., an]、target_variables が [b1, b2,..., bm] なら、{a1, a2,..., an} のすべての部分集合（空集合、全体集合も含む）からなる集合族のうち、b1, b2,...,bm をすべて含む集合からなる族を表すZBDDを返す。base_variables  の型 T は std::vector<bddvar> や std::set<bddvar> など、bddvar 型のコンテナ型である。base_variables, target_variables の各要素や v は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。base_variables や target_variables はソートされている必要はない。base_variables は target_variables の要素を含んでいても、含んでいなくても正しく動作する。

### 使用例

```
std::vector<bddvar> base_variables;
base_variables.push_back(2); base_variables.push_back(3);
// f は {{5},{2, 5},{3, 5},{2, 3, 5}} を表す ZBDD
ZBDD f = getAllSetsIncluding(base_variables, 5);
```

## getPowerSetIncluding

```
getPowerSetIncluding(int n, const std::vector<bddvar>& target_variables)
getPowerSetIncluding(int n, const std::set<bddvar>& target_variables)
getPowerSetIncluding(int n, int v)
```

{1,...,n} のべき集合族のうち、第2引数で指定した target_variables をすべて含む、または、変数 v を含むZBDDを返す。すなわち、target_variables が [b1, b2,..., bm] なら、{1,...,n} のすべての部分集合（空集合、全体集合も含む）からなる集合族のうち、b1, b2,...,bm をすべて含む集合からなる族を表すZBDDを返す。target_variables の各要素や v は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。target_variables はソートされている必要はない。

### 使用例

```
// f は {{1},{1, 2},{1, 3},{1, 2, 3}} を表す ZBDD
ZBDD f = getAllPowerSetsIncluding(3, 1);
```

## getPowerSetNotIncluding

```
getPowerSetNotIncluding(int n, const std::vector<bddvar>& target_variables)
getPowerSetNotIncluding(int n, const std::set<bddvar>& target_variables)
getPowerSetNotIncluding(int n, int v)
```

{1,...,n} のべき集合族のうち、第2引数で指定した target_variables をいずれも含まない、または、変数 v を含まないZBDDを返す。すなわち、target_variables が [b1, b2,..., bm] なら、{1,...,n} のすべての部分集合（空集合、全体集合も含む）からなる集合族のうち、b1, b2,...,bm をいずれも含まない集合からなる族を表すZBDDを返す。target_variables の各要素や v は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。target_variables はソートされている必要はない。

### 使用例

```
// f は {{},{2},{3},{2, 3}} を表す ZBDD
ZBDD f = getPowerSetNotIncluding(3, 1);
```

## getPowerSetWithCard

```
getPowerSetWithCard(const T& variables, int k)
```

variables（を集合とみなしたとき）のべき集合族のうち、要素数が k である集合の族を表すZBDDを返す。variables の各要素や v は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。variables はソートされている必要はない。

### 使用例

```
std::vector<bddvar> variables;
variables.push_back(2); variables.push_back(3); variables.push_back(5);
// f は {{2, 3},{2, 5},{3, 5}} を表す ZBDD
ZBDD f = getPowerSetWithCard(variables, 2);
```

## getPowerSetWithCard

```
getPowerSetWithCard(int n, int k)
```

{1,...,n} のべき集合族のうち、要素数が k である集合の族を表すZBDDを返す。

### 使用例

```
// f は {{1, 2},{1, 3},{2, 3}} を表す ZBDD
ZBDD f = getPowerSetWithCard(3, 2);
```

## makeDontCare

```
makeDontCare(const ZBDD& f, const T& variables)
```

f の各要素について、variables の各要素を含む集合と含まない集合の両方からなる
集合族を表す ZBDD を返す。


### 使用例

```
// f は {{1}, {2, 3}} を表す ZBDD
ZBDD f = ...;
std::vector<bddvar> variables;
variables.push_back(4); variables.push_back(5);
// g は {{1}, {1, 4}, {1, 5}, {1, 4, 5}, {2, 3}, {2, 3, 4}, {2, 3, 5}, {2, 3, 4, 5}} を
// 表す ZBDD
ZBDD g = makeDontCare(f, variables);
```

## isMember

```
template<typename T>
bool isMember(const ZBDD& f, const T& variables)
```

引数で指定した vector variables が表す集合が、ZBDD f が表す集合族に含まれるかどうかを判定する。含まれるなら 1 を、含まれないなら 0 を返す。variables の型 T は std::vector<bddvar> や std::set<bddvar> など、bddvar 型のコンテナ型である。variables の各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。variables はソートされている必要はない。variables の大きさが 0 の場合、空集合が f に含まれるかを判定する。

### 使用例

```
std::vector<bddvar> variables;
variables.push_back(2); variables.push_back(3); variables.push_back(5);

ZBDD f = getPowerSet(3);

if (isMember(f, variables)) { // {2, 3, 5} は f に含まれないので false
    std::cout << "variables in f" << std::endl;
} else {
    std::cout << "variables not in f" << std::endl;
}
```

## isMemberZ

```
template<typename T>
bool isMemberZ(const ZBDD& f, const T& variables)
```

isMember の旧名である。

## countNodes

```
llint countNodes(const std::vector<bddp>& dds, bool is_raw = false)
llint countNodes(const std::set<bddp>& dds, bool is_raw = false)
llint countNodes(const std::vector<BDD>& dds, bool is_raw = false)
llint countNodes(const std::set<BDD>& dds, bool is_raw = false)
llint countNodes(const std::vector<ZBDD>& dds, bool is_raw = false)
llint countNodes(const std::set<ZBDD>& dds, bool is_raw = false)
```

引数で指定した大きさ n の vector/set dds が表す BDD/ZBDD 全体のノード数を返す。ノードは共有されている可能性があり、共有されているノードは 1 つと数える。is_raw は true なら否定枝表現を用いた場合のノード数を、false なら否定枝表現を用いない場合のノード数を返す。dds に bddnull が 1 つでも含まれている場合は 0 を返す。dds が BDD か ZBDD かは自動判定される。dds が BDD と ZBDD の両方を含む場合はエラーメッセージを出力して終了する。

### 使用例

```
std::vector<ZBDD> dds;
// ZBDD f0, f1, f2 は何らかの方法で作成
dds.push_back(f0), dds.push_back(f1), dds.push_back(f2);
printf("%d", countNodes(dds, false));
```

## getUniformlyRandomZBDD

```
template <typename T>
ZBDD getUniformlyRandomZBDD(int level, T& random_engine)
```

ノードレベルが最大 level である（すなわち根ノードのレベルが level 以下である）ZBDD を一様ランダムに構築して返す。
一様ランダムとは、ノードレベルが最大 level である ZBDD が表すことのできる 2^(2^(level)) 個の集合族から一様ランダムに1つ選択し、その集合族を表す ZBDD を構築して返すという意味である。
この関数を呼び出す前に、BDD_NewVar を少なくとも level 回呼び出して、変数を level 個用意する必要がある。
この関数は 2^(level) より大きな計算時間がかかる（level が大きすぎると計算が終わらない）。
引数の T& random_engine は、乱数生成エンジンを指定する。<random> ヘッダのインクルードが必要である。この関数は C++ 版にのみ存在し、C++11 以降でコンパイルした場合のみ使用可能である。

### 使用例

```
std::mt19937 mt(0);
ZBDD f = getUniformlyRandomZBDD(10, mt);
```

## getUniformlyRandomBDDX

```
BDD getUniformlyRandomBDDX(int level, ullint* rand_state)
```

ノードレベルが最大 level である（すなわち根ノードのレベルが level 以下である）ZBDD を一様ランダムに構築して返す。
一様ランダムとは、ノードレベルが最大 level である ZBDD が表すことのできる 2^(2^(level)) 個の集合族から一様ランダムに1つ選択し、その集合族を表す ZBDD を構築して返すという意味である。
この関数を呼び出す前に、BDD_NewVar を少なくとも level 回呼び出して、変数を level 個用意する必要がある。
この関数は 2^(level) より大きな計算時間がかかる（level が大きすぎると計算が終わらない）。
rand_state は乱数エンジンの内部状態を指し、ullint へのポインタを指定する。

この関数は乱数生成として、本ライブラリ内部で実装している Xorshift を用いている。そのため、C++ 言語処理系に依存しない結果が必要な場合に用いることができる。

### 使用例

```
ullint rand_state = 31415926535ull;
BDD f1 = getUniformlyRandomBDDX(10, &rand_state);
BDD f2 = getUniformlyRandomBDDX(10, &rand_state);
```

## getUniformlyRandomZBDDX

```
ZBDD getUniformlyRandomZBDDX(int level, ullint* rand_state)
```

ノードレベルが最大 level である（すなわち根ノードのレベルが level 以下である）ZBDD を一様ランダムに構築して返す。
一様ランダムとは、ノードレベルが最大 level である ZBDD が表すことのできる 2^(2^(level)) 個の集合族から一様ランダムに1つ選択し、その集合族を表す ZBDD を構築して返すという意味である。
この関数を呼び出す前に、BDD_NewVar を少なくとも level 回呼び出して、変数を level 個用意する必要がある。
この関数は 2^(level) より大きな計算時間がかかる（level が大きすぎると計算が終わらない）。
rand_state は乱数エンジンの内部状態を指し、ullint へのポインタを指定する。

この関数は乱数生成として、本ライブラリ内部で実装している Xorshift を用いている。そのため、C++ 言語処理系に依存しない結果が必要な場合に用いることができる。

### 使用例

```
ullint rand_state = 31415926535ull;
ZBDD f1 = getUniformlyRandomZBDDX(10, &rand_state);
ZBDD f2 = getUniformlyRandomZBDDX(10, &rand_state);
```

## getRandomZBDDWithCard

```
template <typename T>
ZBDD getRandomZBDDWithCard(int level, llint card, T& random_engine)
```

ノードレベルが最大 level である（すなわち根ノードのレベルが level 以下である）ZBDD のうち、要素数が card 個であるものをランダムに作成して返す。
この関数を呼び出す前に、BDD_NewVar を少なくとも level 回呼び出して、変数を level 個用意する必要がある。
この関数は、card に依存する計算時間がかかる（card が大きすぎると計算が終わらない）。
引数の T& random_engine は、乱数生成エンジンを指定する。<random> ヘッダのインクルードが必要である。この関数は C++ 版にのみ存在し、C++11 以降でコンパイルした場合のみ使用可能である。

### 使用例

```
std::mt19937 mt(0);
ZBDD f = getUniformlyRandomZBDD(10, mt);
```

## exampleBdd

```
BDD exampleBdd(ullint kind = 0ull)
```

3変数以上8変数以下の BDD の例を返す。kind には 0 以上の数を指定し、同じ数を指定した場合は同じ BDD を返す。
この関数は使用している変数が足りない場合、自動的に `BDD_NewVar()` 関数を呼び出す。
本関数はマニュアルや学習用としての使用を想定している。

## exampleZbdd

```
ZBDD exampleZbdd(ullint kind = 0ull)
```

3変数以上8変数以下の ZBDD の例を返す。kind には 0 以上の数を指定し、同じ数を指定した場合は同じ ZBDD を返す。
この関数は使用している変数が足りない場合、自動的に `BDD_NewVar()` 関数を呼び出す。
本関数はマニュアルや学習用としての使用を想定している。

## constructZBDDFromElements

```
ZBDD constructZBDDFromElements(FILE* fp)
ZBDD constructZBDDFromElements(std::istream& ist)
```

要素を並べたファイルを、引数 fp または ist で指定した（ファイル等の）ストリーム fp または ist から読み込み、ZBDD を構築して返す。
ファイルは、1行に1つの集合の変数番号をスペース区切りで並べた形式である。
最後の行の末尾には改行があってもなくてもよい。何も書かれていない行は空集合に対応する。
空集合に対応する行は、ファイルの末尾に置いてはいけない。
過去には引数に large_sep、small_sep を指定できたが、Ver 0.9.0 廃止された。

### 使用例

ファイルの例

```
1 2

2 3
1 2 4
```

```
std::ifstream ifs("input.txt");
ZBDD f = constructZBDDFromElements(ifs);
ifs.close();
```

## printZBDDElements

```
void printZBDDElements(FILE* fp, const ZBDD& zbdd)
void printZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
void printZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd)
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
```

ZBDD zbdd が表す集合族を指定したストリームに出力する。
例えば、"{4,2,1},{3,1},{1},{}" のような出力が得られる。
zbdd が bddnull のときは "N" と出力し、bddempty のときは
"E" と出力する。

引数の delim1 は、集合族の要素（集合）の区切りである。delim2 は集合族の要素（集合）の要素の区切りである。例えば、`delim1 == "};{"`, `delim2 == ","` なら、"4,2,1};{3,1};{1};{" のような出力が得られる（この場合、先頭と末尾の "{", "}" は別途出力しなければならない）。delim1 と delim2 を指定しない場合（`void printZBDDElements(std::ostream& ost, const ZBDD& zbdd)`）、例えば、"{4,2,1},{3,1},{1},{}" のように出力される。

ver_name_map は変数番号をインデックスとする、任意の文字列の配列であり、
変数番号の代わりに、変数番号に対応する文字列を取得する。

## printZBDDElementsAsValueList

```
void printZBDDElementsAsValueList(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, int num_of_variables)
void printZBDDElementsAsValueList(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, int num_of_variables)
```

ZBDD zbdd が表す集合族を value list 形式で指定したストリームに出力する。
value list 形式では、1行で1つの集合を表す。1行は 0/1 の列からなり、
i 個目（i = 1,...,number_of_variables）の数が 1 であるときに
i 番目の変数が集合に含まれ、0 であるときには含まれない。
zbdd が bddnull のときは "N" と出力し、bddempty のときは
"E" と出力する。

## ZStr

```
std::string ZStr(const ZBDD& zbdd)
```

ZBDD zbdd が表す集合族を文字列として返す。例えば、"{4, 2, 1},{3, 1},{1},{}" のような文字列が返される。
zbdd が 0 終端のときは "E" を、1 終端のときは "{}" を出力する。

### 使用例

```
// f は {3, 2}, {3, 1}, {2, 1} を表す ZBDD
ZBDD f = getAllPowerSetsWithCard(3, 2);
// "{3, 2}, {3, 1}, {2, 1}" が出力される
std::cout << ZStr(f) << std::endl;
```

## zstr

```
std::string zstr(const ZBDD& zbdd)
```

ZStr 関数の別名である。

## importBDDAsBinary

```
BDD importBDDAsBinary(FILE* fp, int root_level = -1)
BDD importBDDAsBinary(std::istream& ist, int root_level = -1)
ZBDD importZBDDAsBinary(FILE* fp, int root_level = -1)
ZBDD importZBDDAsBinary(std::istream& ist, int root_level = -1)
```

引数 fp または ist で指定した（ファイル等の）ストリームから、
[BDD バイナリ形式](https://github.com/junkawahara/dd_documents/blob/main/formats/bdd_binary_format.md) を読み込み、BDD/ZBDD を構築して返す。
root_level に正の数を指定すると、根のレベルが root_level になる。

Ver 1.0.0 で BDD 版が実装された。また、否定枝を使わない場合に対応した。

### 使用例

```
std::ifstream ifs("zbdd.dat");
ZBDD f = importZBDDAsBinary(ifs);
ifs.close();
```

## exportBDDAsBinary

```
template <typename T>
void exportBDDAsBinary(FILE* fp, const BDD& bdd, bool use_negative_arcs, DDIndex<T>* index)void exportBDDAsBinary(FILE* fp, const BDD& bdd, bool use_negative_arcs = true)
void exportBDDAsBinary(std::ostream& ost, const BDD& bdd, bool use_negative_arcs, DDIndex<T>* index)
void exportBDDAsBinary(std::ostream& ost, const BDD& bdd, bool use_negative_arcs = true)
void exportZBDDAsBinary(FILE* fp, const ZBDD& zbdd, bool use_negative_arcs, DDIndex<T>* index)
void exportZBDDAsBinary(FILE* fp, const ZBDD& zbdd, bool use_negative_arcs = true)
void exportZBDDAsBinary(std::ostream& ost, const ZBDD& zbdd, bool use_negative_arcs, DDIndex<T>* index)
void exportZBDDAsBinary(std::ostream& ost, const ZBDD& zbdd, bool use_negative_arcs = true)
```

引数 fp または ost で指定した（ファイル等の）ストリームに、
BDD/ZBDD を、[BDD バイナリ形式](https://github.com/junkawahara/dd_documents/blob/main/formats/bdd_binary_format.md) で書き込む。
use_negative_arcs は否定枝を使うかどうかを表す bool 値であり、true なら否定枝を使用し、false なら否定枝を使わない。否定枝を用いるとファイルサイズが削減されるので、理由がない場合は否定枝を用いることを推奨する。

事前に DDIndex を構築している場合は引数に指定できる。
構築していない場合は index を NULL にすることで、関数内部で自動的に構築される
（関数終了後、自動的に破棄される）。
use_negative_arcs が true のときは、DDIndex は raw モードで作成されていなければならない。
use_negative_arcs が false のときは、DDIndex は通常モード（raw モードではないモード）で作成されていなければならない。


Ver 1.0.0 で BDD 版が実装された。また、引数 use_negative_arcs と DDIndex に対応した。

### 使用例

```
ZBDD f = ...; // f は何らかの方法で作成

std::ofstream ofs("zbdd.dat");
exportZBDDAsBinary(ofs, f);
ofs.close(); // バイナリ形式ファイル zbdd.dat が作成される
```

## importBDDAsGraphillion

```
BDD importBDDAsGraphillion(FILE* fp, int root_level = -1)
BDD importBDDAsGraphillion(std::istream& ist, int root_level = -1)
ZBDD importZBDDAsGraphillion(FILE* fp, int root_level = -1)
ZBDD importZBDDAsGraphillion(std::istream& ist, int root_level = -1)
```

引数 fp または ist で指定した（ファイル等の）ストリームから、
[graphillion 形式](https://github.com/junkawahara/dd_documents/blob/main/formats/graphillion_format.md) を読み込み、BDD/ZBDD を構築して返す。
root_level には、graphillion の変数番号 1（根に近い番号）に対応する SAPPOROBDD の level を指定する。graphillion で universe を n 個設定する場合は、root_level に n を与えればよい。root_level が -1 のときは、SAPPOROBDD の根ノードの level が、graphillion の変数番号 1（根に近い番号）に対応するように自動設定される。

## exportBDDAsGraphillion

```
template <typename T>
void exportBDDAsGraphillion(FILE* fp, const BDD& bdd, int root_level, DDIndex<T>* index)
void exportBDDAsGraphillion(FILE* fp, const BDD& bdd, int root_level = -1)
void exportBDDAsGraphillion(std::ostream& ost, const BDD& bdd, int root_level, DDIndex<T>* index)
void exportBDDAsGraphillion(std::ostream& ost, const BDD& bdd, int root_level = -1)
void exportZBDDAsGraphillion(FILE* fp, const ZBDD& zbdd, int root_level, DDIndex<T>* index)void exportZBDDAsGraphillion(FILE* fp, const ZBDD& zbdd, int root_level = -1)
void exportZBDDAsGraphillion(std::ostream& ost, const ZBDD& zbdd, int root_level, DDIndex<T>* index)
void exportZBDDAsGraphillion(std::ostream& ost, const ZBDD& zbdd, int root_level = -1)
```

引数 fp または ost で指定した（ファイル等の）ストリームに、
BDD/ZBDD を、[graphillion 形式](https://github.com/junkawahara/dd_documents/blob/main/formats/graphillion_format.md) で書き込む。
root_level には、graphillion の変数番号 1（根に近い番号）に対応する SAPPOROBDD の level を指定する。graphillion で universe を n 個設定した場合は、root_level に n を与えればよい。root_level が -1 のときは、graphillion 形式に現れる最も大きな変数番号（すなわち終端に近い）が、SAPPOROBDD の level 1 に対応するように自動設定される。

事前に DDIndex を構築している場合は引数に指定できる。
構築していない場合は index を NULL にすることで、関数内部で自動的に構築される
（関数終了後、自動的に破棄される）。
DDIndex は通常モード（raw モードではないモード）で作成されていなければならない。

Ver 1.0.0 から root_level 引数が追加され、DDNodeIndex から DDIndex<T> に変更された。

### 使用例

```
ZBDD f = ...; // f は何らかの方法で作成

std::ofstream ofs("zbdd_for_graphillion.dat");
exportZBDDAsGraphillion(ofs, f);
ofs.close();
```

## importBDDAsKnuth

```
BDD importBDDAsKnuth(FILE* fp, bool is_hex, int root_level = -1)
BDD importBDDAsKnuth(std::istream& ist, bool is_hex, int root_level = -1)
ZBDD importZBDDAsKnuth(FILE* fp, bool is_hex, int root_level = -1)
ZBDD importZBDDAsKnuth(std::istream& ist, bool is_hex, int root_level = -1)
```

引数 fp または ist で指定した（ファイル等の）ストリームから、
Knuth 形式を読み込み、BDD/ZBDD を構築して返す。
root_level に正の数を指定すると、根のレベルが root_level になる。
is_hex を true にすると、値を16進数として、false にすると、値を10進数として
読み込む。

## exportBDDAsKnuth

```
template <typename T>
void exportBDDAsKnuth(FILE* fp, const BDD& bdd, bool is_hex, DDIndex<T>* index)
void exportBDDAsKnuth(FILE* fp, const BDD& bdd, bool is_hex = false)
void exportBDDAsKnuth(std::ostream& ost, const BDD& bdd, bool is_hex, DDIndex<T>* index)
void exportBDDAsKnuth(std::ostream& ost, const BDD& bdd, bool is_hex = false)
void exportZBDDAsKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex, DDIndex<T>* index)
void exportZBDDAsKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex = false)
void exportZBDDAsKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex, DDIndex<T>* index)
void exportZBDDAsKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex = false)
```

引数 fp または ost で指定した（ファイル等の）ストリームに、
BDD/ZBDD を、Knuth 形式 で書き込む。
is_hex を true にすると16進数を、false にすると10進数を書き込む。
Knuth 形式の利用は非推奨である。代わりに [graphillion 形式](https://github.com/junkawahara/dd_documents/blob/main/formats/graphillion_format.md) を推奨する。

事前に DDIndex を構築している場合は引数に指定できる。
構築していない場合は index を NULL にすることで、関数内部で自動的に構築される
（関数終了後、自動的に破棄される）。
DDIndex は通常モード（raw モードではないモード）で作成されていなければならない。

Ver 1.0.0 から DDIndex<T> 引数が追加された。

## exportZBDDAsGraphviz

```
template <typename T>
void exportBDDAsGraphviz(FILE* fp, const BDD& bdd, std::map<std::string, std::string>* /*option*/, DDIndex<T>* index)
void exportBDDAsGraphviz(FILE* fp, const BDD& bdd, std::map<std::string, std::string>* option = NULL)
void exportBDDAsGraphviz(std::ostream& ost, const BDD& bdd, std::map<std::string, std::string>* /*option*/, DDIndex<T>* index)
void exportBDDAsGraphviz(std::ostream& ost, const BDD& bdd,std::map<std::string, std::string>* option = NULL)
void exportZBDDAsGraphviz(FILE* fp, const ZBDD& zbdd, std::map<std::string, std::string>* /*option*/, DDIndex<T>* index)
void exportZBDDAsGraphviz(FILE* fp, const ZBDD& zbdd, std::map<std::string, std::string>* option = NULL)
void exportZBDDAsGraphviz(std::ostream& ost, const ZBDD& zbdd, std::map<std::string, std::string>* /*option*/, DDIndex<T>* index)
void exportZBDDAsGraphviz(std::ostream& ost, const ZBDD& zbdd, std::map<std::string, std::string>* option = NULL)
```

引数 fp または ost で指定した（ファイル等の）ストリームに、
ZBDD を、[Graphviz](https://www.graphviz.org/) の
[dot 形式](https://www.graphviz.org/doc/info/lang.html)で書き込む。

option 引数では、現バージョンでは無視される。

事前に DDIndex を構築している場合は引数に指定できる。
構築していない場合は index を NULL にすることで、関数内部で自動的に構築される
（関数終了後、自動的に破棄される）。
DDIndex は通常モード（raw モードではないモード）で作成されていなければならない。

Ver 1.0.0 から BDD に対応した。option 引数が用意された。bddNodeIndex が DDIndex<T> に変更された。

### 使用例

```
ZBDD f = ...; // f は何らかの方法で作成

std::ofstream ofs("zbdd_for_graphviz.dat");
exportZBDDAsGraphviz(ofs, f);
ofs.close();
```

Graphviz の dot を以下のコマンドで呼び出すことで、ZBDD が描画される。

```
dot -Tpng -o zbdd.png < zbdd_for_graphviz.dat
```

## exportZBDDAsSvg

```
template<typename T>
void exportBDDAsSvg(FILE* fp, const BDD& bdd, std::map<std::string, std::string>* /*option*/, DDIndex<T>* index)
void exportBDDAsSvg(FILE* fp, const BDD& bdd, std::map<std::string, std::string>* option = NULL)
void exportBDDAsSvg(std::ostream& ost, const BDD& bdd, std::map<std::string, std::string>* /*option*/, DDIndex<T>* index)
void exportBDDAsSvg(std::ostream& ost, const BDD& bdd, std::map<std::string, std::string>* option = NULL)
void exportZBDDAsSvg(FILE* fp, const ZBDD& zbdd, std::map<std::string, std::string>* /*option*/, DDIndex<T>* index)
void exportZBDDAsSvg(FILE* fp, const ZBDD& zbdd, std::map<std::string, std::string>* option = NULL)
void exportZBDDAsSvg(std::ostream& ost, const ZBDD& zbdd, std::map<std::string, std::string>* /*option*/, DDIndex<T>* index)
void exportZBDDAsSvg(std::ostream& ost, const ZBDD& zbdd, std::map<std::string, std::string>* option = NULL)
```

引数 fp または ost で指定した（ファイル等の）ストリームに、
BDD/ZBDD を SVG 形式の図として書き込む。
（SVG 形式はブラウザ等で閲覧できる。）

事前に DDIndex<T> を構築している場合は引数に指定できる。
構築していない場合は index を NULL にすることで、関数内部で自動的に構築される
（関数終了後、自動的に破棄される）。
DDIndex は通常モード（raw モードではないモード）で作成されていなければならない。

本関数は C++ 版にのみ存在する。
BDD 版は未実装である。
