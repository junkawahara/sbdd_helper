# DDNodeIndex クラス

このクラスは非推奨になった。代わりに [DDIndex](reference_cpp_DDIndex.md) クラスを推奨する。

BDD/ZBDD に対して「インデックス」を付加することで、
ZBDD が表す集合族の要素の個数や、BDD/ZBDD のノードの個数を
求める機能を提供する。
要素の個数を求める count 関数は ZDD のみに対応している。

### 使用例

```
ZBDD f = ...;  // 何らかの方法で f を作成
DDNodeIndex index(f, false);  // インデックスを作成する（否定枝表現は使用しない）

std::cout << index.count() << std::endl; // f が表す集合族の要素の個数を出力
std::cout << index.size() << std::endl; // f のノード数を表示

// f のレベル 2 にあるノードの数を表示
std::cout << index.sizeAtLevel(2) << std::endl;

std::vector<bddvar> vec;
index.sizeEachLevel(vec); // f の各レベルのノード数を vec に格納
```

## コンストラクタ

```
DDNodeIndex(const BDD& f, bool is_raw = true)
DDNodeIndex(const ZBDD& f, bool is_raw = true)
```

インデックスを作成する。is_raw が false の場合、
BDD/ZBDD が否定枝表現されていないとみなしたときの
インデックスを作成する。is_raw が true の場合、
BDD/ZBDD が否定枝表現されているとみなしたときの
インデックスを作成する。

## コピー・代入について

`DDNodeIndex` は内部で確保したメモリを所有するため、
コピーコンストラクタと代入演算子を禁止している（private 宣言のみで定義がない）。
コピーや代入を行うコードはコンパイルエラーとなる。
関数の引数として渡す場合は参照を用いること。

## size

```
ullint size()
```

インデックス元の BDD/ZDD のノード数を取得する。


## sizeAtLevel

```
ullint sizeAtLevel(int level)
```

インデックス元の BDD/ZDD の、引数で指定した level のノード数を取得する。

## sizeEachLevel

```
void sizeEachLevel(std::vector<bddvar>& arr)
```

インデックス元の BDD/ZDD の各レベルのノード数を、引数で指定した arr に
格納する。レベル i のノード数は arr[i] に格納される。arr の size は
BDD/ZDD の高さ（根ノードのレベル）+ 1 となる。

## count

```
ullint count()
```

インデックス元の ZDD が表す集合族の要素の個数を取得する。

要素の個数を数える機能は ZDD のみに対応している。
BDD から構築したインデックス（`DDNodeIndex(const BDD& f)` コンストラクタ）に
対して本関数を呼び出した場合は、エラーメッセージを表示して `exit(1)` する
（ただし、BDD が終端の場合は 0 または 1 を返す）。
BDD から構築したインデックスでは size、sizeAtLevel、sizeEachLevel、
および DDNodeIterator による巡行が利用できる。

## DDNodeIndex::DDNodeIterator

DDNodeIterator は DDNodeIndex のクラス内クラスである。
各ノードを巡行するイテレータを表す。
std::iterator<std::input_iterator_tag, bddp> を継承しており、
STL ライブラリの入力イテレータとなる。

### 使用例

```
DDNodeIndex index = ... ; DDNodeIndex を何らかの方法で作成

DDNodeIndex::DDNodeIterator itor = index.begin();
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
