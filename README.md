# One header library for SAPPOROBDD

[SAPPOROBDD](https://github.com/Shin-ichi-Minato/SAPPOROBDD) を使用する際に便利な関数を提供しています。
ライブラリは1つのヘッダファイルだけから構成されているため、
インクルードするだけで使用できます。
C/C++ 言語の両方に対応しています。

ほとんどの機能は ZDD に関するものです。機能は将来的に増やす予定です。
本プログラムで提供している関数は開発中であるため、予告なく仕様を変更することがあります。

## リファレンス

* [C++言語版リファレンス](documents/reference_cpp.md)
* [C言語版リファレンス](documents/reference_c.md)

## 機能の例

* ZDD が表す集合族の集合を1つずつ出力
* Knuth/graphillion/DD binary 形式のファイルの読み込みと書き込み
* BDD/ZDD ノードの0-枝、1-枝が指す子ノードの取得（否定枝表現を直接扱う場合と、否定枝表現が無いものとして扱う場合の両方。BDD/ZDD の違いを意識せずに利用可）
* べき集合を表すZDDの構築（3 要素の例では、{{}, {1}, {2},...,{1,2,3}} の8通り）
* メンバシップ関数（ある集合が集合族に含まれるかを判定）
* ZDD の各ノードを訪問するイテレータ
* ZDD が表す集合族の集合を1つずつ出力するイテレータ

## インストール

[SBDD_helper.h ファイル](SBDD_helper.h) をダウンロードして、任意の場所に配置してください
（C/C++ 両方とも同じファイルです）。

C 言語（bddc.c のみを使用）の場合は以下のように、bddc.h ヘッダのインクルードの後に SBDD_helper.h をインクルードしてください。
```
#include "bddc.h"
#include "SBDD_helper.h"
```

C++ 言語（BDD, ZBDD クラスを使用）の場合、以下のように、BDD.h や ZBDD.h ヘッダのインクルードの後に SBDD_helper.h をインクルードしてください。
```
#include "ZBDD.h"
#include "SBDD_helper.h"
```

## 使用例

C++ 言語
```
#include "ZBDD.h"
#include "SBDD_helper.h"

int main()
{
    BDD_Init(100000, 100000000);

    std::ifstream ifs("file.txt"); // ifstream などの include は略

    // Knuth 形式のファイルからインポート
    ZBDD z = ConstructZBDDFromFileKnuth(ifs, false);

    return 0;
}
```

## 免責事項

本プログラムの使用に関して、作者は一切の責任を負いません。本プログラムは [SAPPOROBDD](https://github.com/Shin-ichi-Minato/SAPPOROBDD) と同様、[MIT ライセンス](https://en.wikipedia.org/wiki/MIT_License)で提供しています。
