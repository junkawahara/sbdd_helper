# DDIndex クラス

`DDIndex<T>` クラスは、BDD/ZBDD に対して「インデックス」を付加することで、
BDD/ZBDD が表す集合族の要素の個数や、BDD/ZBDD のノードの個数を
求める機能を提供するクラスである。
本クラスは `DDNodeIndex` を改良したクラスである。
`DDNodeIndex` は用いる必要がなく、本クラスを用いればよい。

`DDIndex<T>` はテンプレートクラスである。型 T は、各ノードに情報を保持させる場合に、保持させたいデータの型を指定する。情報を保持させない場合は T には int など任意の型を指定する。

count や sampleRandomly などの一部の関数では、内部に計算用の情報を格納させる。
それらの関数を一度呼び出せば、2回目以降、情報が再利用される。
（例えば、count を呼び出した後に sampleRandomly を呼び出すと、情報が再利用されて高速に計算される。）

## 使用例

```
ZBDD f = ...;  // 何らかの方法で f を作成

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

// 各ノードに値を記憶させて計算を行う例。
// この例では、f に含まれる要素数を計算する。
// 終端に近いノードから順に根に向かって計算する。
// 値は value メンバ変数に格納される。
// value の型は DDIndex<T> の T である。
index.terminal(0).value = 0; // 終端ノードに値を格納
index.terminal(1).value = 1;
// ZBDD の各レベルについて
for (int level = 1; level <= index.height(); ++level) {
    // レベル level の各ノードについて
    for (ullint j = 0; j < index.size(level); ++j) {
        // レベル level の j 番目のノードを取得
        DDNode<int> n = index.getNode(level, j);
        // 子ノードの値から親ノードの値を計算
        n.value = n.child(0).value + n.child(1).value;
        // value はインデックスに格納されるので、
        // DDNode<int> n は開放しても問題ない
    }
}
// 根ノードの値を取得
std::cout << "count = " << index.root().value << std::endl;
```

出力結果は以下の通り。
```
nodes = 9
elems = 10
6th set = {2 3 4 }
order = 6
ランダムサンプリング成功
max = 9
count = 10
```

## コンストラクタ

```
DDIndex(const BDD& f, bool is_raw = true)
DDIndex(const ZBDD& f, bool is_raw = true)
DDIndex(bddp f, bool is_raw = false)
```

インデックスを作成する。is_raw が false の場合、
BDD/ZBDD が否定枝表現されていないとみなしたときの
インデックスを作成する。is_raw が true の場合、
BDD/ZBDD が否定枝表現されているとみなしたときの
インデックスを作成する。

現在のバージョンでは、is_raw が false の場合のみ対応している。

## デストラクタ

```
~DDIndex()
```

注意すべき点は特に無い。

## getRawPointer

```
bddNodeIndex* getRawPointer()
```

内部で管理している bddNodeIndex へのポインタを取得。
このポインタを用いて、内部構造の書き換えを行った場合の
DDIndex クラスの動作は不定である。

## getStorageRef

```
T& getStorageRef(bddp f)
```

ノード f に関連付けられた T 型のデータへの参照を取得する。

## getZBDD()

```
ZBDD getZBDD() const
```

インデックス対象の ZBDD を取得する。

## height

```
int height() const
```

インデックス対象の BDD/ZBDD の根ノードのレベルを取得する。

## size

```
ullint size() const
ullint size(int level) const
```

引数を指定しない場合、インデックス元の BDD/ZBDD のノード数を取得する。
引数 level を指定した場合、インデックス元の BDD/ZBDD のレベルが level のノード数を取得する。
否定枝表現を用いずにインデックスを構築した場合は、
否定枝表現を用いない BDD/ZBDD のノード数を取得する。

Ver 1.1.0 から返り値の型を llint から ullint に変更。

## sizeEachLevel

```
void sizeEachLevel(std::vector<bddvar>& arr) const
```

インデックス元の BDD/ZDD の各レベルのノード数を、引数で指定した arr に
格納する。レベル i のノード数は arr[i] に格納される。arr の size は
BDD/ZDD の高さ（根ノードのレベル）+ 1 となる。
否定枝表現を用いずにインデックスを構築した場合は、
否定枝表現を用いない BDD/ZBDD のノード数を取得する。

## count

```
ullint count()
```

インデックス元の BDD/ZDD が表す集合族の要素の個数（64ビット値）を取得する。
要素の個数が64ビット値を超える場合、2^64 で割った余りを返す。
要素の個数が64ビット値を超える場合は countMP 関数を用いれば、
正確な値が取得できる。
本メンバ関数は内部にカウント用の情報を記憶させる。

Ver 1.1.0 から返り値の型を llint から ullint に変更。

## countMP

```
mpz_class countMP()
```

インデックス元の BDD/ZDD が表す集合族の要素の個数を取得する。
返り値は GMP ライブラリの mpz_class であり、任意桁の整数を表現できる。
この関数を利用する際は、SBDDH_GMP マクロを冒頭で定義する。
本メンバ関数は内部にカウント用の情報を記憶させる。

## getMaximum

```
llint getMaximum(const std::vector<llint>& weights, std::set<bddvar>& s) const
llint getMaximum(const std::vector<llint>& weights) const
```

重み最大の集合とその重みを計算して返す。重み最大の集合は第2引数 s に格納される
（そのため、関数呼び出し前に s は空にしておかなければならない）。
weights は要素の重みであり、BDD/ZBDD の変数番号を添え字とし、値が重みである vector である。
（BDD/ZBDD の変数番号は 1 から n である。weights[i] が変数番号 i の重みを表す。
weights[0] は用いられないので任意の値を格納しておく。）

## getMinimum

```
llint getMinimum(const std::vector<llint>& weights, std::set<bddvar>& s) const
llint getMinimum(const std::vector<llint>& weights) const
```

重み最小の集合とその重みを計算して返す。重み最小の集合は第2引数 s に格納される
（そのため、関数呼び出し前に s は空にしておかなければならない）。
weights は要素の重みであり、BDD/ZBDD の変数番号を添え字とし、値が重みである vector である。
（BDD/ZBDD の変数番号は 1 から n である。weights[i] が変数番号 i の重みを表す。
weights[0] は用いられないので任意の値を格納しておく。）

## getSum

```
llint getSum(const std::vector<llint>& weights)
```

全集合の重みの和を計算して返す。
weights は要素の重みであり、BDD/ZBDD の変数番号を添え字とし、値が重みである vector である。
（BDD/ZBDD の変数番号は 1 から n である。weights[i] が変数番号 i の重みを表す。
weights[0] は用いられないので任意の値を格納しておく。）
本メンバ関数は内部にカウント用の情報を記憶させる。

## getSumMP

```
mpz_class getSumMP(const std::vector<llint>& weights)
```

全集合の重みの和を計算して返す。
weights は要素の重みであり、BDD/ZBDD の変数番号を添え字とし、値が重みである vector である。
（BDD/ZBDD の変数番号は 1 から n である。weights[i] が変数番号 i の重みを表す。
weights[0] は用いられないので任意の値を格納しておく。）
返り値は GMP ライブラリの mpz_class であり、任意桁の整数を表現できる。
この関数を利用する際は、SBDDH_GMP マクロを冒頭で定義する。
本メンバ関数は内部にカウント用の情報を記憶させる。

## getOrderNumber

```
llint getOrderNumber(const std::set<bddvar>& s)
```

引数で指定した集合 s が、辞書順で何番目か（0始まり）を返す。s が含まれていない場合、-1 を返す。
本メンバ関数は内部にカウント用の情報を記憶させる。

辞書順の定義は以下の通りである。ZBDD 変数 a と b について、それぞれのレベルを L(a) と L(b) とすると、
L(a) &gt; L(b) ならば a &lt; b とする。すなわち、a と b のうち、根ノードに近い方の変数を「小さい」と定義する。
この順序の下で、辞書順を考える。すなわち、集合 A と B について、
両者の要素を昇順に並べて、初めて異なる要素を a と b とすると、
a &gt; b であるとき、A < B であるとする。

## getOrderNumberMP

```
mpz_class getOrderNumberMP(const std::set<bddvar>& s)
```

引数で指定した集合 s が、辞書順で何番目か（0 始まり）を返す。s が含まれていない場合、-1 を返す。
返り値は GMP ライブラリの mpz_class であり、任意桁の整数を表現できる。
この関数を利用する際は、SBDDH_GMP マクロを冒頭で定義する。
本メンバ関数は内部にカウント用の情報を記憶させる。

辞書順の定義は getOrderNumber 関数の説明を参照。

## getSet

```
std::set<bddvar> getSet(llint order)
```

辞書順で order 番目（0 始まり）の集合を返す。
order が範囲外（マイナスの値か、要素数以上）の場合は空集合を返す。
本メンバ関数は内部にカウント用の情報を記憶させる。

辞書順の定義は getOrderNumber 関数の説明を参照。

## getSet（GMP版）

```
std::set<bddvar> getSet(mpz_class order)
```

辞書順で order 番目（0 始まり）の集合を返す。
order が範囲外（マイナスの値か、要素数以上）の場合は空集合を返す。
返り値は GMP ライブラリの mpz_class であり、任意桁の整数を表現できる。
この関数を利用する際は、SBDDH_GMP マクロを冒頭で定義する。
本メンバ関数は内部にカウント用の情報を記憶させる。

辞書順の定義は getOrderNumber 関数の説明を参照。

## getKSetsZBDD

```
ZBDD getKSetsZBDD(ullint k)
```

辞書順で先頭から k 個の集合を ZBDD として返す。
k が 0 の場合は空集合を返す。
本メンバ関数は内部にカウント用の情報を記憶させる。

辞書順の定義は getOrderNumber 関数の説明を参照。

## getKSetsZBDD（GMP版）

```
ZBDD getKSetsZBDD(const mpz_class& k)
```

辞書順で先頭から k 個の集合を ZBDD として返す。
k が 0 の場合は空集合を返す。
本メンバ関数は内部にカウント用の情報を記憶させる。
この関数を利用する際は、SBDDH_GMP マクロを冒頭で定義する。

辞書順の定義は getOrderNumber 関数の説明を参照。

## getKLightestZBDD / getKHeaviestZBDD

```
ZBDD getKLightestZBDD(ullint k, const std::vector<llint>& weights, int strict)
ZBDD getKHeaviestZBDD(ullint k, const std::vector<llint>& weights, int strict)
```

getKLightestZBDD は重みが小さい順に k 個の集合を ZBDD として返す。
getKHeaviestZBDD は重みが大きい順に k 個の集合を ZBDD として返す。
strict が 0 なら、返り値の ZBDD は必ず k 個の集合を含む。
strict が -1 なら、返り値の ZBDD は k 個以下の、
strict が 1 なら、k 個以上の集合を含む。
これらは strict が 0 の場合よりも計算は速い。
重み weights については getMaximum 関数の説明を参照。
辞書順の定義は getOrderNumber 関数の説明を参照。

## getKLightestZBDD / getKHeaviestZBDD（GMP版）

```
ZBDD getKLightestZBDD(const mpz_class& k, const std::vector<llint>& weights, int strict)
ZBDD getKHeaviestZBDD(const mpz_class& k, const std::vector<llint>& weights, int strict)
```

getKLightestZBDD は重みが小さい順に k 個の集合を ZBDD として返す。
getKHeaviestZBDD は重みが大きい順に k 個の集合を ZBDD として返す。
strict が 0 なら、返り値の ZBDD は必ず k 個の集合を含む。
strict が -1 なら、返り値の ZBDD は k 個以下の、
strict が 1 なら、k 個以上の集合を含む。
これらは strict が 0 の場合よりも計算は速い。
重み weights については getMaximum 関数の説明を参照。
辞書順の定義は getOrderNumber 関数の説明を参照。
この関数を利用する際は、SBDDH_GMP マクロを冒頭で定義する。

## sampleRandomly

```
template <typename U> std::set<bddvar> sampleRandomly(U& random_engine)
std::set<bddvar> sampleRandomly()
```

BDD/ZBDD が表す集合族から集合を1つ一様ランダムに選んで返す。
`U& random_engine` を引数にとる関数は C++11 以降で利用できる。
`U` は乱数エンジンの型であり、`std::mt19937` などが指定できる。
引数のない関数は、C++98 や C++03 で利用できる。内部では rand() を利用している。
本メンバ関数は内部にカウント用の情報を記憶させる。
GMP を用いた（SBDDH_GMP マクロを定義した）場合、これらの関数は利用できない。
GMP 版の sampleRandomly を用いる。

いずれの関数も、要素数が 2^64 - 1 を超える場合、正しい結果を返さない。

## sampleRandomly（GMP 版）

```
std::set<bddvar> sampleRandomly(gmp_randclass& random)
```

BDD/ZBDD が表す集合族から集合を1つ一様ランダムに選んで返す。
GMP の乱数エンジン gmp_randclass を引数に指定する。
本メンバ関数は内部にカウント用の情報を記憶させる。

この関数は要素数が 2^64 を超える場合でも、GMP が提供する乱数が一様であると仮定すれば、
集合族から近似ではなく厳密に一様に1つの集合を選んで返す。

## sampleRandomlyA

```
std::set<bddvar> sampleRandomlyA(ullint* rand_state)
```

BDD/ZBDD が表す集合族から集合を1つ一様ランダムに選んで返す。
乱数の種として、ullint 型の変数へのポインタを引数に指定する。
乱数として、XORShift を用いている。
本メンバ関数は内部にカウント用の情報を記憶させる。

この関数は内部で近似計算を行うため、サンプリング結果は完全な一様ランダムではなく、
わずかに偏りが生じる。
この関数は要素数が 2^64 を超える場合でも利用できるが、乱数の周期は 2^64 - 1 であるので、高々 2^64 - 1 種類の要素しかサンプリングされない。
この関数は乱数の種が同じなら環境に依存せず同じ結果を返す。
GMP を用いた（SBDDH_GMP マクロを定義した）場合でもこの関数は提供される。

Ver 1.1.0 で追加。

## root

```
DDNode<T> root()
```

根を表す [DDNode<T>](reference_cpp_DDNode.md) を返す。

## terminal

```
DDNode<T> terminal(int t)
```

終端を表す [DDNode<T>](reference_cpp_DDNode.md) を返す。

## getNode

```
DDNode<T> getNode(int level, ullint pos)
```

レベルが level の pos 番目（0 始まり）の [DDNode<T>](reference_cpp_DDNode.md) を返す。

Ver 1.1.0 より第2引数 pos の型を llint から ullint に変更

## makeCountIndex

```
void makeCountIndex()
```

count メンバ関数などで用いられる内部情報を計算して格納する。
count 関数など内部で必要に応じて自動的に呼び出されるので、
事前に makeCountIndex を呼び出しておく必要はない。

## begin

```
DDNodeIterator begin()
```

DDNodeIterator の開始イテレータを返す。詳細はイテレータの説明を参照。

## end

```
DDNodeIterator end()
```

DDNodeIterator の終了イテレータを返す。詳細はイテレータの説明を参照。

## weight_min_begin

```
WeightIterator weight_min_begin(const std::vector<llint>& weights) const
```

WeightIterator の重み最小化の開始イテレータを返す。詳細はイテレータの説明を参照。

weights は要素の重みであり、BDD/ZBDD の変数番号を添え字とし、値が重みである vector である。
（BDD/ZBDD の変数番号は 1 から n である。weights[i] が変数番号 i の重みを表す。
weights[0] は用いられないので任意の値を格納しておく。）

## weight_min_end

```
WeightIterator weight_min_end() const
```

WeightIterator の重み最小化の終了イテレータを返す。詳細はイテレータの説明を参照。


## weight_max_begin

```
WeightIterator weight_max_begin(const std::vector<llint>& weights) const
```

WeightIterator の重み最大化の開始イテレータを返す。詳細はイテレータの説明を参照。

weights は要素の重みであり、BDD/ZBDD の変数番号を添え字とし、値が重みである vector である。
（BDD/ZBDD の変数番号は 1 から n である。weights[i] が変数番号 i の重みを表す。
weights[0] は用いられないので任意の値を格納しておく。）

## weight_max_end

```
WeightIterator weight_max_end() const
```

WeightIterator の重み最大化の終了イテレータを返す。詳細はイテレータの説明を参照。

## random_begin

```
RandomIterator random_begin(ullint rand_seed = 1) const
```

RandomIterator の開始イテレータを返す。詳細はイテレータの説明を参照。

rand_seed で乱数の種を指定する。

## random_end

```
RandomIterator random_end() const
```

RandomIterator の終了イテレータを返す。詳細はイテレータの説明を参照。

## dict_begin

```
DictIterator dict_begin()
```

DictIterator 辞書順の開始イテレータを返す。詳細はイテレータの説明を参照。

辞書順の定義は getOrderNumber 関数の説明を参照。

## dict_end

```
DictIterator dict_end()
```

DictIterator 辞書順の終了イテレータを返す。詳細はイテレータの説明を参照。

辞書順の定義は getOrderNumber 関数の説明を参照。

## dict_rbegin

```
DictIterator dict_rbegin()
```

DictIterator 逆辞書順の開始イテレータを返す。詳細はイテレータの説明を参照。

辞書順の定義は getOrderNumber 関数の説明を参照。

## dict_rend

```
DictIterator dict_rend()
```

DictIterator 逆辞書順の終了イテレータを返す。詳細はイテレータの説明を参照。

辞書順の定義は getOrderNumber 関数の説明を参照。

## イテレータ

DDIndex<T> のクラス内クラスとして、各種イテレータが提供されている。
いずれのイテレータも std::iterator<std::input_iterator_tag, bddp> を継承しており、
STL ライブラリの入力イテレータとなる。

DDNodeIterator は各ノードを巡行する。
WeightIterator は重み最小順、または、重み最大順に集合を巡行する。
RandomIterator は集合をランダムに巡行する。
DictIterator は要素を辞書順、または、辞書逆順に集合を巡行する。
辞書順の定義は getOrderNumber 関数の説明を参照。

### 使用例

```
DDIndex<int> index = ... ; DDIndex を何らかの方法で作成

DDIndex<int>::DDNodeIterator itor = index.begin();
while (itor != index.end()) {
    bddp f = *itor;

    // f に対して何らかの処理を行う

    ++itor;
}

bddp g = ... ; g を何らかの方法で作成

// STL の find 関数などが使用可
if (std::find(index.begin(), index.end(), g) != index.end()) {
    std::cout << "g found" << std::endl;
}
```
