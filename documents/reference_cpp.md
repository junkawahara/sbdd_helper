# SBDD_helper C++言語版リファレンス

Version 0.03 alpha

本プログラムで提供している関数は開発中であるため、予告なく仕様を変更することがある。

C++ 版では、C 版の全ての関数も使用可能である（[C言語版リファレンス](reference_c.md)）。

## 変更点

### Version 0.03

C++ 版の関数名の先頭の大文字を小文字に変更した。（例: IsNegative -> isNegative）

提供している関数

## IsNegative

```
bool IsNegative(const BDD& f)
bool IsNegative(const ZBDD& f)
```
f が否定表現であるかを返す。f が否定表現であるなら true を、否定表現でないなら false を返す。

### 使用例

```
bddp f = ...;  // 何らかの方法で f を作成
if (bddisnegative(f)) {
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
bddp f = ...;  // 何らかの方法で f を作成
if (bddisconstant(f)) {
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
ZBDD getSingleSet(const std::vector<bddvar>& vararr)
```

引数で指定した vector vararr が表す集合だけからなる集合族を表すZBDDを返す。すなわち、vararr が [a1, a2,..., an] なら、{{a1, a2,..., an}} を表すZBDDを返す。vararr は少なくとも大きさが n の配列でなければならない。vararr の各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。vararr はソートされている必要はない。vararr の大きさが0のときは空集合 (bddsingle) を返すが、空集合は bddsingle 定数から得られるため、bddgetsingleset(NULL, 0) を呼び出す実質的な意味は無い。

## getSingleSet

```
ZBDD getSingleSet(int n, ...)
```

引数で指定した要素からなる集合だけからなる集合族を表すZBDDを返す。第1引数の n で要素の数を表す。第2引数以降を a1, a2,..., an とするとき、{{a1, a2,..., an}} を表すZBDDを返す。引数の個数は n + 1 でなければならない。各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。引数の各値 はソートされている必要はない。n = 0 を指定すると、空集合 (bddsingle) を返すが、空集合は bddsingle 定数から得られるため、bddgetsinglesetv(0) を呼び出す実質的な意味は無い。

## getPowerSet

```
ZBDD getPowerSet(const std::vector<bddvar>& vararr)
```

引数で指定した vector vararr（を集合とみなしたとき）のべき集合族を表すZBDDを返す。すなわち、vararr が [a1, a2,..., an] なら、{a1, a2,..., an} のすべての部分集合（空集合、全体集合も含む）からなる集合族を表すZBDDを返す。vararr の各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。vararr はソートされている必要はない。

## isMemberZ

```
bool isMemberZ(const ZBDD& f, const std::vector<bddvar>& vararr)
```

引数で指定した vector vararr が表す集合が、ZBDD f が表す集合族に含まれるかどうかを判定する。含まれるなら 1 を、含まれないなら 0 を返す。vararr の各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。vararr はソートされている必要はない。vararr の大きさが 0 の場合、空集合が f に含まれるかを判定する。


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

## writeZBDDToBinary

```
void writeZBDDToBinary(FILE* fp, const ZBDD& zbdd)
void writeZBDDToBinary(std::ostream& ost, const ZBDD& zbdd)
```

引数 fp または ist で指定した（ファイル等の）ストリームに、
ZBDD を、[BDD バイナリ形式](bdd_binary_format.md) で書き込む。

### 使用例

```
ZBDD f = ...; // f は何らかの方法で作成

std::ofstream ofs("zbdd.dat");
writeZBDDToBinary(ofs, f);
ofs.close();
```
