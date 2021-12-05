# SBDD_helper C言語版リファレンス

Version 0.04 alpha

本プログラムで提供している関数は開発中であるため、予告なく仕様を変更することがある。

## 変更点

### Version 0.04

getSingleSet, getPowerSet, isMemberZ の引数を const std::vector<bddvar>& から、
const T& に変更した。T は、T::const_iterator を持つ任意の型（例えば
std::vector<bddvar> や std::set<bddvar>）をサポート。

### Version 0.03

C++ 版の関数名の先頭の大文字を小文字に変更した。（例: IsNegative -> isNegative）

## 提供している関数

## bddisnegative

```
int bddisnegative(bddp f)
```
f が否定表現であるかを返す。f が否定表現であるなら 1 を、否定表現でないなら 0 を返す。

### 使用例

```
bddp f = ...;  // 何らかの方法で f を作成
if (bddisnegative(f)) {
   // f は否定表現である
   // ... 処理
}
```

## bddisconstant

```
int bddisconstant(bddp f)
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

## bddtakenot

```
bddp bddtakenot(bddp f)
```

f の否定 BDD を返す。
リファレンスカウンタは変化させない。

## bddaddnot

```
bddp bddaddnot(bddp f)
```

f が否定であるときは f を返し、f が否定でないときは f の否定 BDD を返す。
リファレンスカウンタは変化させない。

## bdderasenot

```
bddp bdderasenot(bddp f)
```

f が否定であるときは f の否定を外した BDD を返し、f が否定でないときは f を返す。
リファレンスカウンタは変化させない。

## bddis64bitversion

```
int bddis64bitversion()
```

SAPPOROBDD の64ビット版でコンパイルが行われているかどうか判定する。正確には、B_64 マクロが定義されている場合、1 を返し、されていない場合、0 を返す。

## bddisterminal

```
int bddisterminal(bddp f)
```

f が定数関数（bddfalse, bddtrue, bddempty, bddsingle のいずれか）、すなわちBDD/ZBDDの終端ノードであるかを返す。f が定数関数であるなら 1 を、定数関数でないなら 0 を返す。SAPPOROBDD は多値終端に対応できるように準備されているが、完全には対応していない。本関数は現在のところ、bddfalse, bddtrue, bddempty, bddsingle に対してのみ 1 を返す。

## bddgetvar

```
bddvar bddgetvar(bddp f)
```

f の変数番号を返す。

## bddgetlev

```
bddvar bddgetlev(bddp f)
```

f の level を返す。

## bddgetchild0b

```
bddp bddgetchild0b(bddp f)
```

f が BDDノードであるとき、f の 0-枝の子を返す。SAPPOROBDD ではBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の 0-枝の子を返す。f  がZBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild0z

```
bddp bddgetchild0z(bddp f)
```

f が ZBDDノードであるとき、f の 0-枝の子を返す。SAPPOROBDD ではZBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の 0-枝の子を返す。f  がBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild0

```
bddp bddgetchild0(bddp f)
```

f が BDD/ZBDDノードであるとき、f の 0-枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の 0-枝の子を返す。本関数はBDD/ZBDDともに使用可能。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild0braw

```
bddp bddgetchild0braw(bddp f)
```

f が BDDノードであるとき、f の 0-枝の子を返す。SAPPOROBDD ではBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の 0-枝の子を返す。f がZBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild0zraw

```
bddp bddgetchild0zraw(bddp f)
```

f が ZBDDノードであるとき、f の 0-枝の子を返す。SAPPOROBDD ではZBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の 0-枝の子を返す。f がBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild0raw

```
bddp bddgetchild0raw(bddp f)
```

f が BDD/ZBDDノードであるとき、f の 0-枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の 0-枝の子を返す。本関数はBDD/ZBDDともに使用可能。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild1b

```
bddp bddgetchild1b(bddp f)
```

f が BDDノードであるとき、f の 1-枝の子を返す。SAPPOROBDD ではBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の 1-枝の子を返す。f  がZBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild1z

```
bddp bddgetchild1z(bddp f)
```

f が ZBDDノードであるとき、f の 1-枝の子を返す。SAPPOROBDD ではZBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の 1-枝の子を返す。f  がBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild1

```
bddp bddgetchild1(bddp f)
```

f が BDD/ZBDDノードであるとき、f の 1-枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の 1-枝の子を返す。本関数はBDD/ZBDDともに使用可能。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild1braw

```
bddp bddgetchild1braw(bddp f)
```

f が BDDノードであるとき、f の 1-枝の子を返す。SAPPOROBDD ではBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の 1-枝の子を返す。f がZBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild1zraw

```
bddp bddgetchild1zraw(bddp f)
```

f が ZBDDノードであるとき、f の 1-枝の子を返す。SAPPOROBDD ではZBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の 1-枝の子を返す。f がBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild1raw

```
bddp bddgetchild1raw(bddp f)
```

f が BDD/ZBDDノードであるとき、f の 1-枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の 1-枝の子を返す。本関数はBDD/ZBDDともに使用可能。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchildb

```
bddp bddgetchildb(bddp f, int child)
```

f が BDDノードであるとき、f の child 引数で指定した枝の子を返す。SAPPOROBDD ではBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の child-枝の子を返す。f がZBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchildz

```
bddp bddgetchildz(bddp f, int child)
```

f が ZBDDノードであるとき、f の child 引数で指定した枝の子を返す。SAPPOROBDD ではZBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の child-枝の子を返す。f  がBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchild

```
bddp bddgetchild(bddp f, int child)
```

f が BDD/ZBDDノードであるとき、f の child 引数で指定した枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現が用いられていないとみなしたときの f の child-枝の子を返す。本関数はBDD/ZBDDともに使用可能。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchildbraw

```
bddp bddgetchildbraw(bddp f, int child)
```

f が BDDノードであるとき、f の child 引数で指定した枝の子を返す。SAPPOROBDD ではBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の child-枝の子を返す。f がZBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchildzraw

```
bddp bddgetchildzraw(bddp f, int child)
```

f が ZBDDノードであるとき、f の child 引数で指定した枝の子を返す。SAPPOROBDD ではZBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の child-枝の子を返す。f がBDDのときはエラーとなる。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddgetchildraw

```
bddp bddgetchildraw(bddp f, int child)
```

f が BDD/ZBDDノードであるとき、f の child 引数で指定した枝の子を返す。SAPPOROBDD ではBDD/ZBDDは否定枝表現が用いられているが、本関数は否定枝表現における f の child-枝の子を返す。本関数はBDD/ZBDDともに使用可能。本関数は参照カウンタを増加させないため、本関数で得られる bddp ポインタを開放（bddfree）してはいけない。

## bddprimenot

```
bddp bddprimenot(bddvar v)
```

変数番号 v のリテラルの否定だけからなる論理関数に対応する BDD を返す。
bddprime(v) の否定を取った BDD と同じ結果となる。
変数番号 v は 1 から bddvarused() （現在使用している変数の数）以下である必要がある。

## bddgetsingleton

```
bddp bddgetsingleton(bddvar v)
```

引数で指定した変数番号 v だけを要素とする集合だけからなる集合族（すなわち {{v}}）を表すZBDDを返す。変数番号 v は 1 から bddvarused() （現在使用している変数の数）以下である必要がある。

## bddgetsingleset

```
bddp bddgetsingleset(bddvar* vararr, int n)
```

引数で指定した大きさ n の配列 vararr を要素とする集合だけからなる集合族を表すZBDDを返す。すなわち、vararr が [a1, a2,..., an] なら、{{a1, a2,..., an}} を表すZBDDを返す。vararr は少なくとも大きさが n の配列でなければならない。vararr の各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。vararr はソートされている必要はななく、重複する要素を含んでもよい（1つの要素とみなされる）。n = 0 を指定すると、空集合 (bddsingle) を返すが、空集合は bddsingle 定数から得られるため、bddgetsingleset(NULL, 0) を呼び出す実質的な意味は無い。

### 使用例

```
bddvar vararr[3] = {2, 3, 5};
bddp f = bddgetsingleset(vararr, 3); // f は {{2, 3, 5}} を表す ZBDD
```

## bddgetsinglesetv

```
bddp bddgetsinglesetv(int n, ...)
```

引数で指定した要素からなる集合だけからなる集合族を表すZBDDを返す。第1引数の n で要素の数を表す。第2引数以降を a1, a2,..., an とするとき、{{a1, a2,..., an}} を表すZBDDを返す。引数の個数は n + 1 でなければならない。各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。引数の各値 はソートされている必要はないが、重複する要素を含んではいけない。n = 0 を指定すると、空集合 (bddsingle) を返すが、空集合は bddsingle 定数から得られるため、bddgetsinglesetv(0) を呼び出す実質的な意味は無い。

### 使用例

```
bddp f = bddgetsinglesetv(3, 2, 3, 5); // f は {{2, 3, 5}} を表す ZBDD
```

## bddgetpowerset

```
bddp bddgetpowerset(bddvar* vararr, int n)
```

引数で指定した大きさ n の配列 vararr のべき集合族を表すZBDDを返す。すなわち、vararr が [a1, a2,..., an] なら、{a1, a2,..., an} のすべての部分集合（空集合、全体集合も含む）からなる集合族を表すZBDDを返す。vararr は少なくとも大きさが n の配列でなければならない。vararr の各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。vararr はソートされている必要はない。

### 使用例

```
bddvar vararr[3] = {2, 3, 5};
// f は {{},{2},{3},{5},{2,3},{2,5},{3,5},{2,3,5}} を表す ZBDD
bddp f = bddgetpowerset(vararr, 3);
```

## bddgetpowersetn

```
bddp bddgetpowersetn(int n)
```

引数で指定した n について、{1,...,n} のべき集合族を表すZBDDを返す。すなわち、{1,...,n} のすべての部分集合（空集合、全体集合も含む）からなる集合族を表すZBDDを返す。n は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。

### 使用例

```
// f は {{},{1},{2},{3},{1,2},{1,3},{2,3},{1,2,3}} を表す ZBDD
bddp f = bddgetpowersetn(3);
```

## bddismemberz

```
int bddismemberz(bddp f, bddvar* vararr, int n)
```

引数で指定した大きさ n の配列 vararr の要素からなる集合が、ZBDD f が表す集合族に含まれるかどうかを判定する。含まれるなら 1 を、含まれないなら 0 を返す。vararr は少なくとも大きさが n の配列でなければならない。vararr の各要素は 1 以上、bddvarused() （現在使用している変数の数）以下である必要がある。vararr はソートされている必要はない。n が 0 の場合、空集合が f に含まれるかを判定する。この場合は vararr は NULL でもよい（vararr は参照されない）。

### 使用例

```
bddp f = ...; // f は何らかの方法で作成
bddvar vararr[3] = {2, 3, 5};

if (bddismemberz(f, vararr, 3)) {
    // {2, 3, 5} が f に含まれるなら実行される
}
```

## bddprintzbddelements

```
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1, const char* delim2)
```

引数 fp で指定されるファイルポインタに、ZBDD f の全要素（集合）を出力する。各集合は文字列 delim1 で区切られ、集合の各要素は delim2 で区切られる。この関数は f が空集合を含むなら最初に空集合（すなわち空の文字列）が出力される。f が bddempty なら "E" を、f が bddnull なら "N" を出力する。

### 使用例

```
bddvar vararr[3] = {2, 3, 5};
bddp f = bddgetpowerset(vararr, 3); // f はべき集合

FILE* fp = fopen("output.txt", "w");
bddprintzbddelements(fp, f, "\n", ",");
fclose(fp);
```

出力結果は
```

5,3,2
5,3
5,2
5
3,2
3
2
```
となる。1行目は空行である。最終行（8行目）の最後には実際は改行は出力されない。

## bddprintzbddelementswithmap

```
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1, const char* delim2,
                                   const char* var_name_map[])
```

引数 fp で指定されるファイルポインタに、ZBDD f の全要素（集合）を出力する。
各要素の名前を var_name_map で指定できる。
var_name_map は文字列（const char*）の配列である。変数番号 i の要素の名前が var_name_map[i] として
出力される。したがって、var_name_map 配列の大きさは変数の個数＋１以上にしなければならない。
var_name_map[0] は使用されない。その他の動作は bddprintzbddelements と同様である。

### 使用例

```
bddvar vararr[3] = {2, 3, 5};
bddp f = bddgetpowerset(vararr, 3); // f はべき集合
const char* var_name_map[] = {"", "apple", "banana", "cherry", "grape", "peach"};

FILE* fp = fopen("output.txt", "w");
fprintf(fp, "{");
bddprintzbddelements_with_map(fp, f, "},{", ",", var_name_map2);
fprintf(fp, "}");
fclose(fp);
```

出力結果

```
{},{peach,cherry,banana},{peach,cherry},{peach,banana},{peach},{cherry,banana},{cherry},{banana}
```

## bddconstructzbddfromfileknuth

```
bddp bddconstructzbddfromfileknuth(FILE* fp, int is_hex, int root_level)
```

Knuth 形式のファイルを、引数で指定したファイルポインタ fp から読み込み、ZBDDを構築して返す。
is_hex 引数が 0 の場合、ノード番号が10進数で表現され、1 の場合、ノード番号が16進数で表現される。
root_level 引数によって、根ノードが SAPPOROBDD のどのノードレベルに取り込まれるかを
指定することができる。-1 を指定すると、構築されたZBDDの高さと同じになる。

使用例

input.txt の中身
```
#1
2:3,4
#2
3:0,5
4:5,1
#3
5:0,1
```

```
FILE* fp = fopen("input.txt", "r");
bddp f = bddconstructzbddfromfileknuth(fp, 0, -1);
fclose(fp);
bddprintzbddelements(stdout, f, "#", " "); // 出力は 3 2#3 1#2 1
```

## bddwritezbddtofileknuth

```
void bddwritezbddtofileknuth(FILE* fp, bddp f, int is_hex)
```

ZBDD f を Knuth 形式で、引数で指定したファイルポインタ fp に書き込む。
is_hex 引数が 0 の場合、ノード番号が10進数で表現され、1 の場合、ノード番号が16進数で表現される。

使用例
```
// 以下では BDD ノードの使用後開放処理を行っていないが、
// 本来は行う必要がある。
bddp f = bddunion(bddgetsingleton(1), bddgetsingleton(2));
f = bddunion(f, takenot(bddgetsingleton(3)));
FILE* fp = fopen("output.txt", "w");
bddwritezbddtofileknuth(fp, f, 0);
fclose(fp);
```

出力結果
```
#1
2:3,1
#2
3:4,1
#3
4:1,1
```
