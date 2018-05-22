# 勉強メモ 01 : コマンドラインからのコンパイル(cl, link コマンド)の入門

- "Hello, World!"レベルの単純なプログラムを題材に、コマンドラインからのコンパイル(clコマンド), リンク(linkコマンド)の使い方を勉強してみた。
- プリプロセッサを展開したソースや、アセンブラ出力を保存できるようなコンパイルオプションがあったので実験してみた。
- 参考URL
  - "Build C/C++ code on the command line"
    - https://docs.microsoft.com/ja-jp/cpp/build/building-on-the-command-line
    - コマンドラインでのC/C++のビルド方法のチュートリアル。
  - コンパイラ(cl.exe)オプション
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/compiler-options
  - リンカ(link.exe)オプション
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/linker-options

## step01 : HelloWorld.c をコマンドプロンプトからコンパイルしてみた(clコマンド)。

- HelloWorld.c を作成し、コマンドプロンプトからclコマンドでコンパイル・実行してみた。
- [サンプルコードはこちら](./01-hello-world/)

clコマンドでコンパイル: 
```
> cl HelloWorld.c
Microsoft(R) C/C++ Optimizing Compiler Version 19.13.26131.1 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

HelloWorld.c
Microsoft (R) Incremental Linker Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:HelloWorld.exe
HelloWorld.obj
```

実行:
```
> HelloWorld.exe
Hello, World!
```

## step02 : 最適化オプションを無効化, 著作権表示OFF, 警告オプション設定(/Od, /nologo, /Wall, /W0, /W1-/W4)

- `cl` コマンドはそのままだと著作権やバージョン情報が出力されるので、 `/nologo` オプションで非表示にしてみた。
- 今後、アセンブラ出力などを確認する際になるべく素直な(=最適化無しの)アセンブラに変換してほしいので、最適化を無効化する `/Od` オプションをつけてみた。
- C言語の勉強や、より厳密な調査・検証のきっかけとなるので、全ての警告を表示する `/Wall` オプションをつけてみた。
- サンプルコードは step01 と同じ。

`/Od /nologo /Wall` のコンパイル例：
```
> cl /Od /nologo /Wall HelloWorld.c
HelloWorld.c
HelloWorld.c(4): warning C4255: 'main': 関数プロトタイプがありません: '()' を '(void)' に変換します。
C:\Program Files (x86)\Windows Kits\10\include\10.0.16299.0\ucrt\stdio.h(946): warning C4710: 'int printf(const char *const ,...)': インライン関数ではありません。
C:\Program Files (x86)\Windows Kits\10\include\10.0.16299.0\ucrt\stdio.h(946): note: 'printf' の宣言を確認してください
```

警告用オプションについて補足：
- 警告用のオプションには `/W0` (警告を全て抑制する : `/w` に相当)から `/W1` - `/W4` まである。
- また `/WX` を指定すると警告をコンパイルエラーとして扱うので、品質を求めたいプロジェクトでは使ってみると良いかも。
- VS2017のIDEから新規にWindowsコンソールアプリのプロジェクトを作成すると、デフォルトは `/W3` になっていた。

`/W3` のコンパイル例：
```
> cl /Od /nologo /W3 HelloWorld.c
HelloWorld.c
(何も警告表示無し)
```

## step03 : 複数モジュールの分割コンパイル(/c, linkコマンド, /OUT:filename)

- 複数モジュール(.cソースファイル)の分割コンパイルに挑戦してみた。
- linkコマンドで分割コンパイル結果の .obj ファイルをリンクし、 `/OUT:filename` リンカオプションで生成するファイル名を指定してみた。
- [サンプルコードはこちら](./01-multi-module/)

分割コンパイル例：
```
> cl /nologo /c foo.c
> cl /nologo /c bar.c
> cl /nologo /c main.c
```

linkコマンドでリンクする例 : 先頭に指定した".obj"ファイル名で".exe"が生成される。
```
> link /nologo main.obj foo.obj bar.obj
> main.exe
foo() = 10
bar() = -2
```

`/OUT:filename` オプションで生成するファイル名を指定する例：
```
> link /nologo /OUT:foobarmain.exe main.obj foo.obj bar.obj
> foobarmain.exe
foo() = 10
bar() = -2
```

## step04 : CなのかCPPなのか明示 (/TC, /TP)

- clコマンドは、デフォルトではソースファイルの拡張子からCなのかCPPなのか自動判定する。
- clコマンドオプションの `/TC`, `/TP` を使うと、CなのかCPPなのか明示的に設定できるので、試してみた。
- [サンプルコードはこちら](./01-unkown-src-extension/)

`/TC` オプションを指定し、全てのソースファイルをC言語として扱うコンパイルと実行例:
```
> cl /nologo /TC HelloWorld_c.foo
HelloWorld_c.foo

> HelloWorld_c.exe
Hello, World!
```

`/TP` オプションを指定し、全てのソースファイルをC++言語として扱うコンパイルと実行例:
```
> cl /nologo /TP HelloWorld_cpp.foo
HelloWorld_cpp.foo
C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\include\xlocale(313): warning C4530: C++ 例外処理を使っていますが、アンワイン ド セマンティクスは有効にはなりません。/EHsc を指定してください。

> HelloWorld_cpp.exe
Hello, World!
```

※警告に従い `/EHsc` オプションを指定した例:
```
> cl /nologo /TP /EHsc  HelloWorld_cpp.foo
HelloWorld_cpp.foo

> HelloWorld_cpp.exe
Hello, World!
```

## step05 : C言語でもCPP形式の単一行コメントが使える。他、MicrosoftのC/C++拡張機能関連オプション (/Za, /Zc)

- MicrosoftのC/C++拡張機能により、C言語のソースファイル中で2つのスラッシュにより始まる単一行コメントを利用できるので、試してみた。
- 参考 : デフォルトで有効化されている拡張機能
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/microsoft-extensions-to-c-and-cpp
- `/Za` コンパイラオプションによりANSI C89 または ISO C 11 と互換性がない言語構成要素をエラーにしますが、単一行コメントは無効にできないので、試してみた。
- 参考 : `/Za`, 非推奨となった `/Ze` オプション
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/za-ze-disable-language-extensions
- サンプルコードは step04 と同じ。

C言語として明示(`/TC`)して、単一行コメントを正常にコンパイルしてくれる例:
```
> cl /nologo /TC HelloWorld_c.foo
HelloWorld_c.foo
```

`/Za` でも単一行コメントは有効な例:
```
> cl /nologo /TC /Za HelloWorld_c.foo
HelloWorld_c.foo
```

`/Za` で警告を全て有効化 (`/Wall`) しても単一行コメントについては怒られてない例:
```
> cl /nologo /TC /Za /Wall HelloWorld_c.foo
HelloWorld_c.foo
HelloWorld_c.foo(6): warning C4100: 'argv': 引数は関数の本体部で 1 度も参照されません。
HelloWorld_c.foo(6): warning C4100: 'argc': 引数は関数の本体部で 1 度も参照されません。
C:\Program Files (x86)\Windows Kits\10\include\10.0.16299.0\ucrt\stdio.h(946): warning C4710: 'int printf(const char *const ,...)': インライン関数ではありません。
C:\Program Files (x86)\Windows Kits\10\include\10.0.16299.0\ucrt\stdio.h(946): note: 'printf' の宣言を確認してください
```

補足 : `/Zc` オプションで、標準規格とは非互換な拡張機能の一部をON/OFFできるらしい。
- https://docs.microsoft.com/ja-jp/cpp/build/reference/zc-conformance

## step06 : インクルードパスをカスタマイズ(/I, /X) + インクルードされたファイルパスを表示(/showIncludes)

- `/I` コンパイラオプションでインクルードディレクトリを追加できるので、試してみた。
  - (複数指定したい場合は、ディレクトリの分だけ `/I ディレクトリ名` を追加)
- `/showIncludes` コンパイラオプションでインクルードされるファイルパスを表示できるので、試してみた。
- `/X` コンパイラオプションで標準のインクルードディレクトリを無視するので、どんなエラーになるか試してみた。
- [サンプルコードはこちら](./01-include-path/)

まずは `/I` を未指定でコンパイルすると、コンパイルエラー(fatal error C1083)になる例:
```
> cl /nologo /c pptest.c
pptest.c
pptest.c(1): fatal error C1083: include ファイルを開けません。'pptest.h':No such file or directory
```

続いて `/I` を指定すると、コンパイルに成功するので、おまけで `/showIncludes` フラグも付けてみた例:
```
> cl /nologo /c /I .\inc /showIncludes pptest.c
pptest.c
メモ: インクルード ファイル:  C:\Program Files (x86)\Windows Kits\10\include\10.0.16299.0\ucrt\stdio.h
メモ: インクルード ファイル:   C:\Program Files (x86)\Windows Kits\10\include\10.0.16299.0\ucrt\corecrt.h
メモ: インクルード ファイル:    C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\include\vcruntime.h
メモ: インクルード ファイル:     C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\include\sal.h
メモ: インクルード ファイル:      C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\include\concurrencysal.h
メモ: インクルード ファイル:     C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\include\vadefs.h
メモ: インクルード ファイル:   C:\Program Files (x86)\Windows Kits\10\include\10.0.16299.0\ucrt\corecrt_wstdio.h
メモ: インクルード ファイル:    C:\Program Files (x86)\Windows Kits\10\include\10.0.16299.0\ucrt\corecrt_stdio_config.h
メモ: インクルード ファイル:  .\inc\pptest.h
```

最後に `/X` オプションを付けると stdio.h が見つからなくなりエラーになる例:
```
> cl /nologo /c /I .\inc /X pptest.c
pptest.c
pptest.c(1): fatal error C1083: include ファイルを開けません。'stdio.h':No such file or directory
```

## step07 : プリプロセス結果を保存(/P, /E, /EP, /C)

- プリプロセス結果を標準出力に表示したり(`/E`)、ファイルに保存(`/P`)できるので試してみた。
  - `/P` で保存されるファイル名はソースファイル名の basename + `.i` となる。
- `/EP` オプションを指定すると `#line` が省略されるので試してみた。
- `/C` オプションを指定するとプリプロセス時にコメントを残しますので確認してみた。
- なお `/E`, `/EP`, `/P` オプションを使うとコンパイルは行われない。
- サンプルコードは step06 と同じ。

`/E` で標準出力する例: 大量に出力されるので手動でファイルにリダイレクト
```
> cl /nologo /E /I .\inc pptest.c > pptest.i
pptest.c

(pptest.iの中身 : 以降、見やすくする為、プリプロセス結果中の連続する空行は一行に縮めています。)
#line 1 "pptest.c"
(...)
#line 1 ".\\inc\\pptest.h"

char message[] = "Release Message";
#line 6 ".\\inc\\pptest.h"
#line 3 "pptest.c"

int foo(int a, int b) {
    return a + b;
}

int bar(int a, int b) {
    return a * b;
}
```

`/P` でファイル保存 + `/EP` オプションで `#line` 省略 + `/C` でコメントを残す例:
```
> cl /nologo /P /EP /C /I .\inc pptest.c
pptest.c

(pptest.iの中身)
//
// stdio.h
//
(...)
char message[] = "Release Message";

/**
 * comment1
 */
int foo(int a, int b) {
    return a + b;
}

/**
 * comment2
 */
int bar(int a, int b) {
    return a * b;
}
```

## step08 : アセンブラコードを保存(/FA)

- `/FA` オプションを指定すると、コンパイル時に生成されたアセンブラコードを保存してくれる。(コンパイル自体も実行される)
- c, s 引数を指定するとマシン語を含めたりソースコードを含めることができるので、試してみた。
- 参考 : https://docs.microsoft.com/ja-jp/cpp/build/reference/fa-fa-listing-file
- [サンプルコードはこちら](./01-save-asm/)

`/FA` オプションを指定して、アセンブラコードを保存する例:
```
> cl /nologo /c /Od /FA foo.c
(foo.asm が生成されるので、中身を見てみるなど。)
```

`/FAc` オプションを指定して、対応するマシン語が埋め込まれたアセンブラコードを生成する例:
```
> cl /nologo /c /Od /FAc foo.c
(foo.cod が生成されるので、中身を見てみるなど。)
```

`/FAs` オプションを指定して、対応するソースコードが埋め込まれたアセンブラコードを生成する例:
```
> cl /nologo /c /Od /FAs foo.c
(foo.asm が生成されるので、中身を見てみるなど。)
```

c, s 両方指定する例 : c と s の順番はどちらでも良い。
```
> cl /nologo /c /Od /FAcs foo.c
(foo.cod が生成されるので、中身を見てみるなど。)
```

補足:
- **x86版で生成したアセンブラコードと見比べてみると、呼び出し規約や関数のプロローグ・エピローグの違いなどが見えてきて面白いかも。**
- 参考 : 64bit ソフトウェア規約
  - https://docs.microsoft.com/ja-jp/cpp/build/x64-software-conventions

## おまけ : その他使うかもしれないコンパイラオプション

- `/Fe[pathname]`
  - コンパイラにより生成されるexe/dllのファイル名またはディレクトリを指定するらしい。`/c` オプション付きの場合は無視されるらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/fe-name-exe-file
- `/Fo[pathname]`
  - コンパイラにより生成されるobjファイル名またはディレクトリを指定するらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/fo-object-file-name
- `/Zp{1|2|4|8|16}`
  - 構造体メンバの配置、パッキングに使うらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/zp-struct-member-alignment
- `/Zs`
  - 構文をチェックするだけらしい。
- `/Oy`
  - フレームポインタの省略(x86専用)
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/oy-frame-pointer-omission

### デバッグ情報系

- `/Z7, /Zi, /ZI`
  - デバッグ情報の形式を指定するらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/z7-zi-zi-debug-information-format
- `/Fd[pathname]`
  - `/ZI` などデバッグ情報オプション指定時に生成されるプログラムデータベース(PDB)ファイル名を指定するらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/fd-program-database-file-name

### セキュリティ向上系

- `/RTC`
  - 実行時のエラーチェックを有効にするらしい。
  - 最適化OFFにする必要があるため、品質保証などテスト段階でのビルドで使うのが推奨されているらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/rtc-run-time-error-checks
- `/GS`
  - バッファオーバーランの検出コードを挿入するらしい。
  - デフォルトで有効化されており、無効化するには `/GS-` を指定するらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/gs-buffer-security-check
- `/sdl`
  - `/GS` を強制するとともにセキュリティ関連のいくつかのコンパイル時/実行時エラーチェックを有効化するらしい。
  - デフォルトは無効らしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/sdl-enable-additional-security-checks
- `/guard:cf`
  - 制御フローガードを有効化するらしい。(デフォルトは無効)
  - コンパイラで有効にしただけでは、制御フローガードは行われないようだ。
  - リンカで `/GUARD:CF` を指定し、EXE/DLLのヘッダーで制御フローガードをONにする必要があるっぽい。
  - (逆に、リンカで `/GUARD:CF` を指定しても、モジュールコンパイル時に `/guard:cf` が指定されていなければ制御フローガードは行われないものと推測される)
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/guard-enable-control-flow-guard
