# SBDD_helper リファレンス

Version 1.3.0

本プログラムで提供している関数は開発中であるため、予告なく仕様を変更することがある。

* [C++言語版リファレンス](reference_cpp.md)
* [C言語版リファレンス](reference_c.md)

C++ 版では、C 版の全ての関数も使用可能である。

## マクロ

SBDD_helper の機能は以下のマクロによって切り替わる。
各マクロの詳細は [README のマクロ一覧](../README.md#マクロ一覧)を参照のこと。

| マクロ | 定義する場所 | 説明 |
|--------|--------------|------|
| SBDDH_GMP | ユーザが `SBDD_helper.h` のインクルードより前に定義する | GMP を用いた機能を有効にする。Version 1.0.3 以前の名称は USE_GMP。 |
| SBDDH_BDDCT | ユーザが `BDDCT.h` のインクルードより後、`SBDD_helper.h` のインクルードより前に定義する | BDDCT（重み最適化）を用いた機能を有効にする。 |
| SAPPOROBDD_PLUS_PLUS | [SAPPOROBDD++](https://github.com/junkawahara/SAPPOROBDD-plus-plus) の `bddc.h` が定義する（ユーザは定義しない） | SAPPOROBDD++ に対応した動作に切り替える。名前空間 sbddh の中で `using namespace sapporobdd;` を行い、64 ビット版かどうかの判定を B_32 マクロの有無で行う。 |
| B_64 / B_32 | SAPPOROBDD（SAPPOROBDD++）側のマクロ | 64 ビット版と 32 ビット版のどちらを使用するかを表す。bddis64bitversion 関数（C++ 版は is64BitVersion 関数）の返り値が決まる。 |

## 変更点

以下では C 版、C++ 版の両方の変更点について記述する。

### Version 1.3.0 (未リリース)

#### 非互換な変更

* 互換性のために残されていた以下の旧関数名の定義を、`#define` によるマクロから
インライン関数に変更。旧名はこれまで通り使用できる。
マクロは名前空間 sbddh の外にも影響し、利用者側の同名の関数や変数を
書き換えてしまう問題があったが、インライン関数にすることでその問題がなくなった。
（左が旧名、右が新しい名前である。新しい名前を使用することを推奨する。）
    * getAllSetsIncluding、getAllPowerSetsIncluding → getPowerSetIncluding
    * getAllPowerSetsNotIncluding → getPowerSetNotIncluding
    * getAllSetsWithCard、getAllPowerSetsWithCard → getPowerSetWithCard
    * bddconstructzbddfrombinary → bddimportzbddasbinary
    * constructZBDDFromBinary → importZBDDAsBinary
    * bddwritezbddtobinary → bddexportzbddasbinary
    * writeZBDDToBinary → exportZBDDAsBinary
    * constructZBDDFromGraphillion → importZBDDAsGraphillion
    * bddwritebddforgraphillion → bddexportbddasgraphillion
    * writeZBDDForGraphillion → exportZBDDAsGraphillion
    * bddconstructbddfromfileknuth → bddimportbddasknuth
    * bddconstructzbddfromfileknuth → bddimportzbddasknuth
    * constructBDDFromFileKnuth → importBDDAsKnuth
    * constructZBDDFromFileKnuth → importZBDDAsKnuth
    * bddwritezbddtofileknuth → bddexportzbddasknuth
    * writeZBDDToFileKnuth → exportZBDDAsKnuth
    * bddwritebddforgraphviz → bddexportbddasgraphviz
    * writeBDDForGraphviz → exportBDDAsGraphviz
    * writeZBDDForGraphviz → exportZBDDAsGraphviz
* 同様に名前空間の外に影響していた `unused` マクロを `sbddextended_unused` に改名。
* bddNodeIndex_sizeEachLevel、DDNodeIndex::sizeEachLevel、DDIndex::sizeEachLevel が
格納するノード数の型を bddvar（変数番号の型）から ullint に変更。1 つのレベルのノード数が bddvar で表せる最大値を超える場合に、正しい個数を格納できなかったため。

#### 追加

* [SAPPOROBDD++](https://github.com/junkawahara/SAPPOROBDD-plus-plus) に対応。SAPPOROBDD_PLUS_PLUS マクロを参照のこと。
* DDIndex にインデックスを破棄する clear 関数と、インデックスが有効かどうかを調べる isValid 関数を追加。
* C 版に bddNodeIterator_destruct 関数を追加。bddNodeIterator_make で確保したイテレータを解放する（bddElementIterator_destruct と同様）。
* countNodes を std::set<BDD>、std::set<ZBDD> に対しても使用できるようにした（BDD、ZBDD に operator< がないため、これまでは事実上呼び出せなかった）。

#### 不具合の修正

* 複数の翻訳単位から SBDD_helper.h をインクルードするとリンクエラーになる不具合を修正（C 版の定数と、GMP 版の sbddh_divide の明示的特殊化）。
* DDIndex::getOrderNumber、getOrderNumberMP が、対象の族に含まれない集合に対して -1 ではなく誤った順序番号を返す不具合を修正。
* bddcountnodes が BDD と ZBDD の混在を検出できず、誤ったノード数を返す不具合を修正。
* bddgetsingleset が重複した変数を検出できず、誤った集合を返す不具合を修正。
* getSingleSet に変数を 1 つも渡さなかったときに不正な ZBDD を返す不具合を修正。
* getKHeaviestZBDD が k に要素数より大きい値を渡されたときの結果を、GMP の有無によらず全ての集合を返すように統一。
* 要素数が 2^63 以上の族に対して getSet と辞書順イテレータが正しく動作しない不具合を修正。
* weightRange 系の関数で、bddcost の範囲を超える重みや境界値を検出できていなかった不具合を修正。
* getPowerSetWithCard に負の濃度を渡したときの動作と、getRandomZBDDWithCard、getRandomZBDDWithCardX に実現不可能な濃度を渡したときの無限ループを修正。
* GMP 版の sampleRandomlyA が、要素数が 2^64 の倍数のときに空集合を返す不具合を修正。非 GMP・C++11 版の sampleRandomly が空の族に対して未定義動作となる不具合を修正。
* DDIndex、DDNodeIndex をコピー不可にした（生ポインタを所有しているため、コピーすると二重解放になっていた）。bddNodeIndex_copy の実装も修正。
* sbddextended_MyDict_copy の作業領域のリークと、メモリ不足時の誤ったポインタ検査を修正。
* DDIndex::clear の後に usedVar を呼ぶと segfault する不具合を修正。
* BDD から構築した DDIndex に対して ZBDD 専用の機能を呼び出したとき、および DDNodeIndex(const BDD&) に対して count を呼び出したときの異常終了を、実行時のエラー検出に変更。
* DDIndex のコンストラクタの is_raw 引数が黙って無視されていたため、raw モードを指定した場合は明示的にエラーとするようにした。また、bddnull から構築した DDIndex を無効なインデックスと判定するようにした。
* ElementIterator が終端を越えて進み、終端で直前の要素を返す不具合を修正。

#### 入出力の堅牢性

* バイナリ形式の読み込みで、全ての読み込みの成否とノード ID の範囲を検査するようにし、作業領域のリークを修正。
* バイナリ形式の入出力をバイトオーダーに依存しないようにし、異なる機種の間でもファイルを受け渡せるようにした。
* graphillion 形式の読み込みで、空ファイル、root_level の指定、未登録の子ノード ID を正しく扱うようにし、フォーマットエラー時のリークを修正。
* Knuth 形式の読み込みで、ノードを含まない入力、範囲外の子ノード ID、順序の誤ったレベルのヘッダ行を検出するようにし、フォーマットエラー時のリークを修正。レベルのヘッダ行の検査は assert ではなく実行時に行うため、NDEBUG を定義したビルドでも有効。
* C 版の readLine が、最終行に改行のないファイルを C++ 版と同じように扱うようにした。
* 要素形式の読み込みで、範囲外の変数番号と変数番号のオーバーフローを検出するようにした。
* graphviz 形式の出力で、raw モードで構築したインデックスを拒否するようにし、自前で構築したインデックスのリークを修正。
* 文字列の連結時に snprintf へ残りのバッファサイズを渡すようにした。

#### その他

* C 版の MyDict の実装を AVL 木に変更し、単調な順序でキーを挿入した場合に性能が劣化しないようにした。
* C++ 版の makeNode が C 版と同じ引数検査を行うようにし、optimize と getSum が weights の大きさを検査するようにした。
* std::iterator が非推奨であることによる警告を解消。
* リポジトリのルートの Makefile が tests/Makefile へ転送するようにし、combine.py が失敗時に配布用ヘッダを壊さないようにした。
* bddisvalid、bddgetchild 系の返す弱参照、DDIndex がインデックス対象の DD を所有しないこと、clear が DDNode の値を無効にすることなどをドキュメントに追記。

### Version 1.2.0 (2024/1/4)

* SBDDH_NO_BDDCT（BDDCT を使用しない場合に定義するマクロ）を廃止。SBDDH_BDDCT マクロを導入。この変更により、BDDCT 関連の機能を使用する場合、SBDD_helper.h をインクルードする前に SBDDH_BDDCT マクロを定義する必要がある。
* 複数のオブジェクトファイル（*.o）を生成する際のリンクエラーを修正。

### Version 1.1.0 (2023/10/18)

* 以下の関数を追加
    * 環境に依存しないランダムサンプリング関数 DDIndex::sampleRandomlyA 。
    * k 個の集合を抽出する DDIndex::getKSetsZBDD 関数。
    * 指定した重み以下からなる集合のみを抽出する weightRange 関数など。
    * 重みが小さい、または、大きい順に k 個の集合のみを抽出する DDIndex::getKLightestZBDD、DDIndex::getKHeaviestZBDD 関数。
    * インデックス対象の ZBDD を取得する DDIndex::getZBDD 関数。
    * BDD 変数を宣言する SBDDH_NewVar、SBDDH_NewVarRev 関数。
    * ZBDD に含まれる変数の集合を取得する DDIndex::userVar 関数。
* USE_GMP マクロの名称を SBDDH_GMP に変更。
* 重み最小順、最大順、辞書順、辞書逆順、ランダムサンプリング用のイテレータを追加。
* bddNodeIndex や DDIndex の size、count 関数の返り値を llint から ullint に変更。getBddp の第2引数の型を llint から ullint に変更。

### Version 1.0.3 (2023/10/13)

* getChild0Raw、getChildRaw、takeNot、addNot、eraseNot（いずれも C++ 版のみ）の不具合を修正。

### Version 1.0.2 (2023/10/10)

* Mac / Linux 環境に正式に対応。複数のコンパイラでコンパイルできることを確認。

### Version 1.0.1 (2023/10/2)

* ランダムに BDD/ZBDD を作成する `getUniformlyRandomBDDX` 関数と `exampleBdd` 関数を追加。

### Version 1.0.0 (2023/9/28)

* 正式版としてリリース。
* `importBDD*As*`、`exportBDD*As*` 関数を整理。一部関数で BDD に対応。
* TdZdd 用の関数を追加。`SBDD_helper_tdzdd.h` として提供。
* 以下の関数を追加。
    * 要素数を指定してランダムに ZBDD を作成する getRandomZBDDWithCard 関数（C++ 版のみ）
    * 終端を取得する bddgetterminal 関数（C 版のみ）

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
