# SAPPOROBDD one header library

このツールは SAPPOROBDD について知っている人向けです。
SAPPOROBDD を使用する際に便利な関数を提供しています。
ライブラリは1つのヘッダファイルだけから構成されているため、
インクルードするだけで使用できます。C/C++ 言語の両方に
対応しています。

## インストール

SBDD_helper.h ファイルをダウンロードして、任意の場所に配置してください。

C 言語の場合は以下のようにインクルードしてください。
```
#include "bddc.h"
#include "SBDD_helper.h"
```

C++ 言語の場合、以下のようにインクルードしてください。
```
#include "ZBDD.h"
#include "SBDD_helper.h"
```

## 使用例

C 言語
```
#include "bddc.h"
#include "SBDD_helper.h"

int main()
{
    FILE* fp;
    bddp p;

    bddinit(100000, 100000000);
    fp = fopen("file.txt", "r");
    p = bddconstructzddfromfile(fp, 0); // import from file
    fclose(fp);

    return 0;
}
```

C++ 言語
```
#include "ZBDD.h"
#include "SBDD_helper.h"

int main()
{
    FILE* fp;
    bddp p;
    
    BDD_Init(100000, 100000000);

    std::ifstream ifs("file.txt");
    ZBDD z = ConstructZDDFromFile(ifs, false); // import from file

    return 0;
}
```

現状ではファイルから ZDD への変換のみ対応しています。
