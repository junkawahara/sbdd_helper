# ElementIterator クラスと ElementIteratorHolder クラス

BDD/ZBDD が表す集合族の要素を巡行するイテレータ。
BDD/ZBDD が表す集合族の各要素に対する処理を行うときなどに使用できる。


### 使用例

```
bool is_size2(std::set<bddvar> s) {
    return s.size() == 2;
}
```

```
ZBDD f = ...;  // {{1, 2}, {2, 3}} を表す集合族であるとする

// ElementIterator を使用するときは、ElementIteratorHolder を
// 作成する。コンストラクタに f を指定する。
ElementIteratorHolder eih(f);

// ElementIteratorHolder の begin メンバ関数により、イテレータを取得
ElementIterator itor = eih.begin();

// イテレータの使い方は C++ の STL と同じである。
// 以下の while 文で、f が表す集合族の各集合を巡行する。
while (itor != eih.end()) {
    // 1回目の実行で s は {1, 2} となり、
    // 2回目の実行で s は {2, 3} となる。
    std::set<bddvar> s = *itor;
    ++itor;
}

// 要素（集合）の大きさが 2 であるものの個数を出力
std::cout << std::count_if(eih.begin(), eih.end(), is_size2);
```

