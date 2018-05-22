# 勉強メモ 04 : 動的リンクライブラリ(DLL : Dynamic Link Library)を作ってみる

- DLLとそれを使うEXEを作ってみる。
- VSのIDEからプロジェクトを作成するのではなく、内部の仕組みを学ぶため、libコマンドでライブラリを生成してみる。
- 今回は関数をexport/importしてみる。データやクラスのexport/importについては参考URLのドキュメント参照のこと。
- [サンプルコードはこちら](./04-dll-src/)
- 参考URL
  - "Visual C++ の DLL"
    - https://docs.microsoft.com/ja-jp/cpp/build/dlls-in-visual-cpp
    - DLLの話題全体を細かく解説している包括的なドキュメント。

## step1 : DLL側のビルド例1(ファイル分割無し, CLコマンドでDLLまで一気に生成)

- foobarbaz.c は `__declspec(dllexport)` で関数をエクスポートしている。
- これを、clコマンドで `/LD` オプションを指定することで、ファイル分割無しで一気にDLLを生成させた。

```
> cl /LD foobarbaz.c
Microsoft(R) C/C++ Optimizing Compiler Version 19.13.26131.1 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

foobarbaz.c
Microsoft (R) Incremental Linker Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:foobarbaz.dll
/dll
/implib:foobarbaz.lib
foobarbaz.obj
   ライブラリ foobarbaz.lib とオブジェクト foobarbaz.exp を作成中
```

エクスポートファイル(.exp) やインポートライブラリ(.lib) が同時に生成される。

dumpbin でエクスポート領域を確認してみる:

```
C:\work\SVNWORK\gitbucket01\sakamoto\re-study-201805\study-memo-vs2017-native-tools\04-dll-src>dumpbin /exports foobarbaz.dll
Microsoft (R) COFF/PE Dumper Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file foobarbaz.dll

File Type: DLL

  Section contains the following exports for foobarbaz.dll

    00000000 characteristics
    FFFFFFFF time date stamp
        0.00 version
           1 ordinal base
           2 number of functions
           2 number of names

    ordinal hint RVA      name

          1    0 00001020 bar
          2    1 00001000 foo

  Summary

        2000 .data
        1000 .pdata
        9000 .rdata
        1000 .reloc
        B000 .text
```

`__declspec(dllexport)` が指定されたfoo(),bar()関数がエクスポートされ、baz()関数はエクスポートされていないことを確認出来る。 

メモ:
- `/LD` だとデバッグ情報無し。 `/LDd` でデバッグ用のDLLを生成する。
- 出力ファイル名を指定したい場合は `/Fe` オプションで指定する。

## step2 : DLL側のビルド例2(clコマンドで分割コンパイル, linkコマンドでDLL生成)

- foo.c, bar.c, baz.c を分割コンパイルし、linkコマンドでDLLを生成してみる。
- `/DLL` リンカオプションを指定してDLLを生成。

```
> cl /nologo /c foo.c
> cl /nologo /c bar.c
> cl /nologo /c baz.c
> link /nologo /OUT:foobarbaz2.dll /DLL foo.obj bar.obj baz.obj
```

特に結果は記載しないが、 `dumpbin /exports foobarbaz2.dll` をやってみると、step1と同様のエクスポート情報を確認できた。

## step3 : 暗黙的なリンク(implicit link)を使ったアプリ側のビルド

- 暗黙的なリンクを行うサンプルコードをビルドし、実行してみる。
- usedll_implicit.c が foobarbaz.dll がエクスポートしている関数を `__declspec(import)` でインポートしている。
- 参考:
  - "DLL と実行形式のリンク" > "DLL を暗黙的にリンクする方法"
    - https://docs.microsoft.com/ja-jp/cpp/build/linking-an-executable-to-a-dll#how-to-link-implicitly-to-a-dll
  - "Importing Function Calls Using __declspec(dllimport)"
    - https://docs.microsoft.com/ja-jp/cpp/build/importing-function-calls-using-declspec-dllimport

usedll_implicit.c をビルドするには、DLLをビルドした時に生成されるインポートライブラリを一緒に指定する。
```
> cl usedll_implicit.c foobarbaz.lib

> usedll_implicit.exe
foo(2, 3) = 5
bar(2, 3) = 6
```

ビルドしたexeのIAT(Import Address Table) をdumpbinで確認してみると、確かに foobarbaz.dll 用にbarとfooのエントリーが埋め込まれていることを確認できた。
```
> dumpbin /imports usedll_implicit.exe
(...)
  Section contains the following imports:

    foobarbaz.dll
             140012220 Import Address Table
             14001AF80 Import Name Table
                     0 time date stamp
                     0 Index of first forwarder reference

                           0 bar
                           1 foo
```

## step4 : 明示的なリンク(explicit link)を使ったアプリ側のビルド
 
- 手動でDLLをロードし、関数ポインタを取得するサンプルコードをビルドし、実行してみる。
- usedll_explicit.c が foobarbaz.dll を `LoadLibrary()` でロードし、`GetProcAddress()` でエクスポートされている関数のポインタを取得し、呼び出している。
- 参考:
  - "DLL と実行形式のリンク" > "DLL を明示的にリンクする方法"
    - https://docs.microsoft.com/ja-jp/cpp/build/linking-an-executable-to-a-dll#how-to-link-explicitly-to-a-dll
  - "Importing Function Calls Using __declspec(dllimport)"
    - https://docs.microsoft.com/ja-jp/cpp/build/importing-function-calls-using-declspec-dllimport
  - https://docs.microsoft.com/ja-jp/cpp/build/loadlibrary-and-afxloadlibrary
  - https://docs.microsoft.com/ja-jp/cpp/build/getprocaddress

usedll_explicit.c のビルドは、特にライブラリを指定する必要はない。日本語コメントがUTF-8で書かれているので、 `/utf-8` オプションを指定している。
```
> cl /utf-8 usedll_explicit.c

> usedll_explicit.exe
foo(2, 3) = 5
bar(2, 3) = 6
```

ビルドしたexeのIATをdumpbinで確認してみると、foobarbaz.dll のエントリーが無いことを確認できた(結果省略)。

## step5 : ビルドターゲット(x86/x64)がEXEとDLLで異なる場合どうなるか？

DLLがx86でビルドしており、EXEが64bitの場合、あるいはその逆の場合にどうなるか実験してみる。

通常、リンク時にインポートライブラリのビルドターゲットが異なっているとエラーとなってしまうので、暗黙的リンクの場合を想定する必要は薄い。
明示的なリンクでどうなるか実験してみた。

1. foobarbaz.dll を x86 でビルド、usedll_explicit.exe を x64 でビルド
   - `LoadLibrary()` で失敗。
2. foobarbaz.dll を x64 でビルド、usedll_explicit.exe を x86 でビルド
   - `LoadLibrary()` で失敗。

以上のように、ビルドターゲットが異なるDLLのロードは失敗することが確認できた。





