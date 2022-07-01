# SBDD_helper C++言語版リファレンス

Version 0.04 alpha

本プログラムで提供している関数は開発中であるため、予告なく仕様を変更することがある。

C++ 版では、C 版の全ての関数も使用可能である（[C言語版リファレンス](reference_c.md)）。

## 変更点

### Version 0.04

getSingleSet, getPowerSet, isMemberZ の引数を const std::vector<bddvar>& から、
const T& に変更した。T は、T::const_iterator を持つ任意の型（例えば
std::vector<bddvar> や std::set<bddvar>）をサポート。

### Version 0.03

C++ 版の関数名の先頭の大文字を小文字に変更した。（例: IsNegative -> isNegative）

## 提供しているクラス

* [DDNodeIndex](reference_cpp_DDNodeIndex.md)
* [ElementIterator と ElementIteratorHolder](reference_cpp_ElementIterator.md)

## 提供している関数

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

f が定数関数（bddfalse, bddtrue, bddempty, bddsingle のいずれか）、すなわちBDD/ZBDDの終端ノードであるかを返す。f が定数関数であるなら 1 を、定数関数でないなら 0 を返す。

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

SAPPOROBDD の64ビット版でコンパイルが行われているかどうか判定する。正確には、B_64 マクロが定義されている場合、1 を返し、されていない場合、0 を返す。


## isTerminal

```
bool isTerminal(const BDD& f)
bool isTerminal(const ZBDD& f)
```

f が定数関数（bddfalse, bddtrue, bddempty, bddsingle のいずれか）、すなわちBDD/ZBDDの終端ノードであるかを返す。f が定数関数であるなら 1 を、定数関数でないなら 0 を返す。SAPPOROBDD は多値終端に対応できるように準備されているが、完全には対応していない。本関数は現在のところ、bddfalse, bddtrue, bddempty, bddsingle に対してのみ 1 を返す。


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

## getAllSetsIncluding

```
template<typename T>
getAllSetsIncluding(const T& base_variables, const std::vector<bddvar>& target_variables)
template<typename T>
getAllSetsIncluding(const T& base_variables, const std::set<bddvar>& target_variables)
getAllSetsIncluding(const T& base_variables, int v)
```

base_variables（を集合とみなしたとき）のべき集合族のうち、第2引数で指定した target_variables をすべて含む、または、変数 v を含むZBDDを返す。すなわち、base_variables が [a1, a2,..., an]、target_variables が [b1, b2,..., bm] なら、{a1, a2,..., an} のすべての部分集合（空集合、全体集合も含む）からなる集合族のうち、b1, b2,...,bm をすべて含む集合からなる族を表すZBDDを返す。base_variables  の型 T は std::vector<bddvar> や std::set<bddvar> など、bddvar 型のコンテナ型である。base_variables, target_variables の各要素や v は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。base_variables や target_variables はソートされている必要はない。base_variables は target_variables の要素を含んでいても、含んでいなくても正しく動作する。

### 使用例

```
std::vector<bddvar> base_variables;
base_variables.push_back(2); base_variables.push_back(3);
// f は {{5},{2, 5},{3, 5},{2, 3, 5}} を表す ZBDD
ZBDD f = getAllSetsIncluding(base_variables, 5);
```

## getAllPowerSetsIncluding

```
getAllPowerSetsIncluding(int n, const std::vector<bddvar>& target_variables)
getAllPowerSetsIncluding(int n, const std::set<bddvar>& target_variables)
getAllPowerSetsIncluding(int n, int v)
```

{1,...,n} のべき集合族のうち、第2引数で指定した target_variables をすべて含む、または、変数 v を含むZBDDを返す。すなわち、target_variables が [b1, b2,..., bm] なら、{1,...,n} のすべての部分集合（空集合、全体集合も含む）からなる集合族のうち、b1, b2,...,bm をすべて含む集合からなる族を表すZBDDを返す。target_variables の各要素や v は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。target_variables はソートされている必要はない。

### 使用例

```
// f は {{1},{1, 2},{1, 3},{1, 2, 3}} を表す ZBDD
ZBDD f = getAllPowerSetsIncluding(3, 1);
```

## getAllPowerSetsNotIncluding

```
getAllPowerSetsNotIncluding(int n, const std::vector<bddvar>& target_variables)
getAllPowerSetsNotIncluding(int n, const std::set<bddvar>& target_variables)
getAllPowerSetsNotIncluding(int n, int v)
```

{1,...,n} のべき集合族のうち、第2引数で指定した target_variables をいずれも含まない、または、変数 v を含まないZBDDを返す。すなわち、target_variables が [b1, b2,..., bm] なら、{1,...,n} のすべての部分集合（空集合、全体集合も含む）からなる集合族のうち、b1, b2,...,bm をいずれも含まない集合からなる族を表すZBDDを返す。target_variables の各要素や v は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。target_variables はソートされている必要はない。

### 使用例

```
// f は {{},{2},{3},{2, 3}} を表す ZBDD
ZBDD f = getAllPowerSetsNotIncluding(3, 1);
```

## getAllSetsWithCard

```
getAllSetsWithCard(const T& variables, int k)
```

variables（を集合とみなしたとき）のべき集合族のうち、要素数が k である集合の族を表すZBDDを返す。variables の各要素や v は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。variables はソートされている必要はない。

### 使用例

```
std::vector<bddvar> variables;
variables.push_back(2); variables.push_back(3); variables.push_back(5);
// f は {{2, 3},{2, 5},{3, 5}} を表す ZBDD
ZBDD f = getAllSetsWithCard(variables, 2);
```

## getAllPowerSetsWithCard

```
getAllPowerSetsWithCard(int n, int k)
```

{1,...,n} のべき集合族のうち、要素数が k である集合の族を表すZBDDを返す。

### 使用例

```
// f は {{1, 2},{1, 3},{2, 3}} を表す ZBDD
ZBDD f = getAllPowerSetsWithCard(3, 2);
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

## isMemberZ

```
template<typename T>
bool isMemberZ(const ZBDD& f, const T& variables)
```

引数で指定した vector variables が表す集合が、ZBDD f が表す集合族に含まれるかどうかを判定する。含まれるなら 1 を、含まれないなら 0 を返す。variables  の型 T は std::vector<bddvar> や std::set<bddvar> など、bddvar 型のコンテナ型である。variables の各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。variables はソートされている必要はない。variables の大きさが 0 の場合、空集合が f に含まれるかを判定する。

### 使用例

```
std::vector<bddvar> variables;
variables.push_back(2); variables.push_back(3); variables.push_back(5);

ZBDD f = getPowerSet(3);

if (isMemberZ(f, variables)) { // {2, 3, 5} は f に含まれないので false
    std::cout << "variables in f" << std::endl;
} else {
    std::cout << "variables not in f" << std::endl;
}
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

## constructBDDFromFileKnuth

```
BDD constructBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
BDD constructBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
```

引数 fp または ist で指定した（ファイル等の）ストリームから、
Knuth 形式を読み込み、BDD を構築して返す。
root_level に正の数を指定すると、根のレベルが root_level になる。
is_hex を true にすると、値を16進数として、false にすると、値を10進数として
読み込む。

## constructZBDDFromFileKnuth

```
ZBDD constructZBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
ZBDD constructZBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
```

引数 fp または ist で指定した（ファイル等の）ストリームから、
Knuth 形式を読み込み、ZBDD を構築して返す。
root_level に正の数を指定すると、根のレベルが root_level になる。
is_hex を true にすると、値を16進数として、false にすると、値を10進数として
読み込む。


## constructZBDDFromElements

```
ZBDD constructZBDDFromElements(FILE* fp, const char* large_sep, const char* small_sep)
ZBDD constructZBDDFromElements(std::istream& ist, const char* large_sep, const char* small_sep)
```

要素の集合の集合から ZBDD を作成する。

## constructZBDDFromBinary

```
ZBDD constructZBDDFromBinary(FILE* fp, int root_level = -1)
ZBDD constructZBDDFromBinary(std::istream& ist, int root_level = -1)
```

引数 fp または ist で指定した（ファイル等の）ストリームから、
[BDD バイナリ形式](bdd_binary_format.md) を読み込み、ZBDD を構築して返す。
root_level に正の数を指定すると、根のレベルが root_level になる。

### 使用例

```
std::ifstream ifs("zbdd.dat");
ZBDD f = constructZBDDFromBinary(ifs);
ifs.close();
```

## writeZBDDToFileKnuth

```
void writeZBDDToFileKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex)
void writeZBDDToFileKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex)
```

引数 fp または ost で指定した（ファイル等の）ストリームに、
ZBDD を、Knuth 形式 で書き込む。
is_hex を true にすると16進数を、false にすると10進数を書き込む。

## writeZBDDToBinary

```
void writeZBDDToBinary(FILE* fp, const ZBDD& zbdd)
void writeZBDDToBinary(std::ostream& ost, const ZBDD& zbdd)
```

引数 fp または ost で指定した（ファイル等の）ストリームに、
ZBDD を、[BDD バイナリ形式](bdd_binary_format.md) で書き込む。

### 使用例

```
ZBDD f = ...; // f は何らかの方法で作成

std::ofstream ofs("zbdd.dat");
writeZBDDToBinary(ofs, f);
ofs.close();
```


## writeZBDDForGraphillion

```
void writeZBDDForGraphillion(FILE* fp, const ZBDD& zbdd, DDNodeIndex* index = NULL)
void writeZBDDForGraphillion(std::ostream& ost, const ZBDD& zbdd, DDNodeIndex* index = NULL)
```

引数 fp または ost で指定した（ファイル等の）ストリームに、
ZBDD を、[graphillion 形式](graphillion_format.md)で書き込む。
事前に DDNodeIndex を構築している場合は引数に指定できる。
構築していない場合は index を NULL にすることで、関数内部で自動的に構築される
（関数終了後、自動的に破棄される）。

### 使用例

```
ZBDD f = ...; // f は何らかの方法で作成

std::ofstream ofs("zbdd_for_graphillion.dat");
writeZBDDForGraphillion(ofs, f);
ofs.close();
```


## writeZBDDForGraphviz

```
void writeZBDDForGraphviz(FILE* fp, const ZBDD& zbdd, DDNodeIndex* index = NULL)
void writeZBDDForGraphviz(std::ostream& ost, const ZBDD& zbdd, DDNodeIndex* index = NULL)
```

引数 fp または ost で指定した（ファイル等の）ストリームに、
ZBDD を、[Graphviz](https://www.graphviz.org/) の
[dot 形式](https://www.graphviz.org/doc/info/lang.html)で書き込む。
事前に DDNodeIndex を構築している場合は引数に指定できる。
構築していない場合は index を NULL にすることで、関数内部で自動的に構築される
（関数終了後、自動的に破棄される）。

### 使用例

```
ZBDD f = ...; // f は何らかの方法で作成

std::ofstream ofs("zbdd_for_graphviz.dat");
writeZBDDForGraphviz(ofs, f);
ofs.close();
```

Graphviz の dot を以下のコマンドで呼び出すことで、ZBDD が描画される。

```
dot -Tpng -o zbdd.png < zbdd_for_graphviz.dat
```
