# 勉強メモ 03 : 静的ライブラリを作ってみる

- DLLの前に、静的にリンクするライブラリ(.lib)を作ってみる。
- VSのIDEからプロジェクトを作成するのではなく、内部の仕組みを学ぶため、libコマンドでライブラリを生成してみる。
- [サンプルコードはこちら](./03-static-library-src/)
- 参考URL
  - "スタティック ライブラリ (C++/CX)"
    - https://docs.microsoft.com/ja-jp/cpp/cppcx/static-libraries-c-cx
    - (VSのIDEからプロジェクトを開始するやり方で、libコマンドについては書かれていない。)
  - "チュートリアル: スタティック ライブラリの作成と使用 (C++)"
    - https://docs.microsoft.com/ja-jp/cpp/windows/walkthrough-creating-and-using-a-static-library-cpp
    - こちらが、VSを使ったちゃんとしたチュートリアル。真面目に勉強するならこちらを読むべし。
  - "LIB リファレンス"
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/lib-reference
    - こちらが "The Microsoft Library Manager" である LIB.exe の使い方のリファレンスとなる。

## 静的ライブラリ(.lib)のビルドと lib コマンド

コンパイル:
```
> cl /nologo /c /Fo.\lib\ lib\foo.c
> cl /nologo /c /Fo.\lib\ lib\bar.c
```

ライブラリに結合:
```
> lib /out:.\lib\foobar.lib lib\foo.obj lib\bar.obj
```

.lib ファイルに含まれるオブジェクトファイル(COFF)の確認:
```
> lib /list lib\foobar.lib
Microsoft (R) Library Manager Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.

lib\foo.obj
lib\bar.obj
```

## 静的ライブラリ(.lib)のリンク

作成した静的ライブラリをリンクしてみる。ライブラリに含まれる関数 foo(), bar() を呼び出す main.c をコンパイルする。

clコマンドの段階で .lib ファイルを指定すれば、自動的にリンクしてくれる:
```
> cl main.c lib\foobar.lib
Microsoft(R) C/C++ Optimizing Compiler Version 19.13.26131.1 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

main.c
Microsoft (R) Incremental Linker Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:main.exe
main.obj
lib\foobar.lib

> main.exe
foo(2, 3) = 5
bar(2, 3) = 6
```

分割コンパイルして、link コマンドで .lib ファイルをリンクする例:
```
> cl /nologo /c main.c
main.c

> link /nologo main.obj lib\foobar.lib

> main.exe
foo(2, 3) = 5
bar(2, 3) = 6
```

