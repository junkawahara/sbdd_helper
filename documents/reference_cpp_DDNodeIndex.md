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

std::vector<ullint> vec;
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

インデックスは、渡された BDD/ZBDD の根を指すポインタ（bddp）を、
参照カウンタを増やさずに保持する。そのため、インデックスを使用している間は、
コンストラクタに渡した BDD/ZBDD をユーザ側で保持しておく必要がある。
一時オブジェクトから構築した場合、その BDD/ZBDD は文の終わりで破棄され、
以降のガベージコレクションでノードが回収されて、インデックスが
解放済みのノードを指す可能性がある。

```
ZBDD f = getPowerSet(10);
DDNodeIndex index1(f);              // f を保持しているので正しい

DDNodeIndex index2(getPowerSet(10)); // 誤り。一時オブジェクトは破棄される
```

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
void sizeEachLevel(std::vector<ullint>& arr)
```

インデックス元の BDD/ZDD の各レベルのノード数を、引数で指定した arr に
格納する。レベル i のノード数は arr[i] に格納される。arr の size は
BDD/ZDD の高さ（根ノードのレベル）+ 1 となる。

Ver 1.3.0 から arr の要素の型を bddvar から ullint に変更。

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
STL ライブラリの入力イテレータの要件を満たす。
前置・後置の operator++ と operator* を持ち、value_type は bddp である
（operator* は参照ではなく値を返すため、reference も bddp である）。

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
