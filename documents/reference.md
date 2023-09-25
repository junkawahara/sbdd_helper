# SBDD_helper リファレンス

Version 0.9.0

本プログラムで提供している関数は開発中であるため、予告なく仕様を変更することがある。

* [C++言語版リファレンス](documents/reference_cpp.md)
* [C言語版リファレンス](documents/reference_c.md)

C++ 版では、C 版の全ての関数も使用可能である。

## 変更点

以下では C 版、C++ 版の両方の変更点について記述する。

### Version 0.9.0 (2023/9/25)

* バージョン番号の付け方を3つの形式（メジャー、マイナー、パッチ）に変更。
* C++ 版で DDIndex<T> クラスの追加。DDNodeIndex クラスは obsolate に。DDIndex<T> は以下の機能を提供する。
    * 否定枝表現を用いないノード数のカウント。
    * GMP (GNU Multiple Precision Arithmetic Library) を用いた 64 ビットを超える要素の個数の計算。
    * 集合の重み最大、最小、和の計算。
    * ランダムサンプリング（GMP を用いると要素数が 2^64 を超える場合も対応）。
    * 集合と、集合の辞書順の値を相互変換。
* 以下の関数を追加。
    * 複数の BDD/ZBDD の（否定枝表現を用いない）ノード数を数える countNodes 関数
    * 引数に BDD/ZBDD と、raw 表現かどうかを指定可能な bddgetchild0g、bddgetchild1g、bddgetchildg 関数（C 版のみ）
    * n 変数のランダムな ZBDD を構築する getUniformlyRandomZBDD 関数（C++11 以降で使用可能）
    * ノードに任意の型 T の情報を格納する機能。
* constructZBDDFromElements 関数と bddconstructzbddfromelements 関数の large_sep、small_sep 引数を廃止。
* ZBDD を SVG に出力する機能を追加（C++ 版のみ）。
* `constructBDD*`、`writeBDD*` 系の関数を整理して、名前を `importBDD*As*`、`exportBDD*As*` に統一。
* `getPowerSet*` 系の関数を整理して、名前を `getPowerSet*` に統一。
* 内部で用いている vector, map を、C++ 版では std 標準ライブラリを用いるように変更。

### Version 0.07 (2023/9/22)

* 以下の関数を追加: bddmakenodeb、bddmakenodez、bddMakeNode。
* isMemberZ を isMember にリネーム（isMemberZ も残している）。

### Version 0.06 (2023/9/21)

* 以下の関数を追加: bddisemptymember、getAllSetsIncluding、getAllPowerSetsIncluding、
getAllPowerSetsNotIncluding、getAllSetsWithCard、makeDontCare、zstr。
* bddtruthtabletobdd 関数を削除。
* bddNodeIndex に与える DD が終端や bddnull の場合の不具合を修正。
* DD が大きい場合に bddwritebddforgraphviz がエラーになる不具合を修正。
* 一部の関数でメモリリークを修正。

### Version 0.05 (2022/1/3)

* SAPPOROBDD のバージョンは 1.85 以上を必須とした。
* bddgetpowersetn(int) 関数、getPowerSet(int) 関数の追加。
* ZBDD の要素の文字列表現を返す関数 ZStr を追加。
* writeZBDDForGraphillion 関数の追加。

### Version 0.04

getSingleSet, getPowerSet, isMemberZ の引数を const std::vector<bddvar>& から、
const T& に変更した。T は、T::const_iterator を持つ任意の型（例えば
std::vector<bddvar> や std::set<bddvar>）をサポート。

### Version 0.03

C++ 版の関数名の先頭の大文字を小文字に変更した。（例: IsNegative -> isNegative）
