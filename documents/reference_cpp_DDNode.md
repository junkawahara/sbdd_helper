# DDNode クラス

`DDNode<T>` クラスは、[DDIndex&lt;T&gt;](reference_cpp_DDIndex.md) クラスで扱う BDD/ZBDD の「ノード」を扱うクラスである。
BDD/ZBDD の各ノードに情報を格納させる際に用いるクラスである。

`DDNode<T>` はテンプレートクラスである。型 T は、各ノードに情報を保持させる場合に、保持させたいデータの型を指定する。
ノードに保持された情報は T の参照型（T&）である value メンバ変数で参照できる。value は DDIndex に格納されているため、`DDNode<T>` の value に値を格納した後に、`DDNode<T>` を削除（デストラクタ呼び出し）しても情報は保持されたままである。

使用例は [DDIndex クラスのリファレンスを参照](reference_cpp_DDIndex.md)。

## コンストラクタ

```
DDNode(bddp f, DDIndex<T>& index)
```

DDNode を構築する。ユーザが明示的にコンストラクタを呼ぶことは想定していない。
`DDIndex<T>` クラスの root や terminal、getNode 関数が `DDNode<T>` を返すので、それらを用いる。

## getBddp

```
bddp getBddp() const
```

ノードが表す BDD/ZBDD へのポインタ（bddp 型）を返す。

## child

```
DDNode<T> child(int c) const
```

ノードの c-枝 (c は 0 か 1) が指すノードを返す。

## isTerminal

```
bool isTerminal() const
bool isTerminal(int t) const
```

ノードが終端であるかを判定する。終端なら true を返す。引数 t (0 か 1) を与えたときは、t-終端であるかを判定する。

## DDNode に関するグローバル関数

```
template <typename T>
bool operator==(const DDNode<T>& node1, const DDNode<T>& node2)
template <typename T>
bool operator!=(const DDNode<T>& node1, const DDNode<T>& node2)
```

2つのノードが同じであるかを判定する。
