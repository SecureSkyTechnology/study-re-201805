# 勉強メモ 01 : モジュール定義ファイル(".DEF")を使ったDLLのエクスポート

- [Native Tools の勉強メモ 04 でのDLLサンプルコード](../study-memo-vs2017-native-tools/04-dll-example.md) では、何の説明も無しに `__declspec(dllexport)` を使った。
- しかしDLLで関数を公開するには、他にもモジュール定義ファイル(".DEF")を使った方式がある。
- セキュリティ対策技術の向上を目的としたバイナリ解析の知識を深めるため、モジュール定義ファイル(".DEF")を使った方式について簡単なサンプルコードで練習してみた。
- 参考:
  - "DEF ファイルを使った DLL からのエクスポート"
    - https://docs.microsoft.com/ja-jp/cpp/build/exporting-from-a-dll-using-def-files
  - "モジュール定義 (.def) ファイル"
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/module-definition-dot-def-files
  - "__declspec(dllexport) を使った DLL からのエクスポート"
    - https://docs.microsoft.com/ja-jp/cpp/build/exporting-from-a-dll-using-declspec-dllexport
  - "__declspec(dllimport) を使った関数呼び出しのインポート"
    - https://docs.microsoft.com/ja-jp/cpp/build/importing-function-calls-using-declspec-dllimport

## DLL側のサンプルコード紹介

モジュール定義ファイル(".DEF")での公開と、`__declspec(dllexport)` による公開の両方をサポートするDLLサンプルコードを作ってみた。
実行コード本体である "dll_lib.c" と、モジュール定義ファイル "dll_lib.def" の二本立てになる。 

- [dll_lib.c](./01-def-export/dll_lib.c)
  - "func1" - "func7"がモジュール定義ファイルによりエクスポートされる関数になる。 
  - 比較用に foo(), bar() を"__declspec(dllexport)"で自動的にエクスポートされるようにしておく。 
- [dll_lib.def](./01-def-export/dll_lib.def)
  - func1: 特にオプションを指定していないので、序数はリンカ側で決定され、エクスポート名もfunc1のまま。 
  - func2: 序数を5に手動指定。最初の序数が5なので、func1などリンカ側で決定される序数は5以降の空いている序数が使われる。エクスポート名はfunc2のまま。 
  - func3: "NONAME"オプションを指定されているので、エクスポート名は無い。序数でのみエクスポートされる。 
  - func4: "PRIVATE"オプションが指定されているので、インポートライブラリ(.lib)には含まれない。DLLファイルでは通常通り序数7, エクスポート名はそのままでエクスポートされる。 
  - func5: "NONAME" + "PRIVATE"オプションが指定されているので、インポートライブラリ(.lib)には含まれず、さらにDLLファイルでも序数でのみエクスポートされる。 
  - func6: "funcX"としてエクスポートされる。 
  - func7: "funcY"としてエクスポートされるが、"PRIVATE"オプションが指定されるので、インポートライブラリ(.lib)には含まれない。 

ビルドし、dumpbin で内容を確認してみる。

```
> cl /c dll_lib.c
> link /dll /def:dll_lib.def dll_lib.obj

> dumpbin /exports dll_lib.dll
(...)
Dump of file dll_lib.dll

File Type: DLL

  Section contains the following exports for dll_lib.dll

    00000000 characteristics
    FFFFFFFF time date stamp
        0.00 version
           5 ordinal base
           9 number of functions
           7 number of names

    ordinal hint RVA      name

          9    0 00001030 bar
         10    1 00001000 foo
         11    2 00001060 func1
          5    3 00001080 func2
          7    4 000010C0 func4
         12    5 00001100 funcX
         13    6 00001120 funcY
          6      000010A0 [NONAME]
          8      000010E0 [NONAME]

  Summary
(...)
```

dumpbinの出力より、以下を確認出来る。 
- 序数が5から始まっている。
- func6, func7 がそれぞれ funcX, funcY でエクスポートされている。
- DEFファイルで"NONAME"を指定した func3, func5 が実際に名前無しとなり、dumpbinの出力上は"[NONAME]"となっている。
- dllexport でエクスポートした foo(), bar() 関数も、関数名と同じ名前でエクスポートされている。

また生成されたインポートライブラリ "dll_lib.lib" をdumpbinで確認してみると、DEFファイルで"PRIVATE"オプションを指定したfunc4, func5, funcY(func7) がインポートライブラリに含まれていないことを確認出来る。

```
> dumpbin /headers dll_lib.lib
(出力は省略)
```

## EXE側のサンプルコード紹介

`__declspec(dllimport)` + インポートライブラリによる暗黙的なロードと、`LoadLibrary()` + `GetProcAddress()` による明示的なロードの二種類をデモするサンプルコードを作ってみた。

- [dll_main.c](./01-def-export/dll_main.c)

ビルド + 実行:

```
> cl dll_main.c dll_lib.lib

> dll_main.exe
dll module handle = 00007FFEDCE20000
foo(2, 3) = 5
bar(2, 3) = 6
func1(2, 3) = 6
func2(2, 3) = 7
func3(2, 3) = 8
func4(2, 3) = 9
func5(2, 3) = 10
funcX(2, 3) = 11
funcY(2, 3) = 12
```

### EXE側サンプルコード解説

- func4, func5, funcY 以外はインポートライブラリに含まれているので `__declspec(dllimport)` を使って宣言し、リンカにリンクしてもらう。 

```
// (...)
 
__declspec(dllimport) int foo(int a, int b);
__declspec(dllimport) int bar(int a, int b);
__declspec(dllimport) int func1(int a, int b);
__declspec(dllimport) int func2(int a, int b);
__declspec(dllimport) int func3(int a, int b);
/* __declspec(dllimport) int func4(int a, int b); */
/* __declspec(dllimport) int func5(int a, int b); */
__declspec(dllimport) int funcX(int a, int b); /* func6 */
/* __declspec(dllimport) int funcY(int a, int b); */ /* func7 */
 
// (...)
```

- func4, funcY(func7)はエクスポート名を使って `GetProcAddress()` でDLLから直接関数ポインタを取り出している。

```
func4 = (DLLFUNC)GetProcAddress(hDll, "func4");
(...)
funcY = (DLLFUNC)GetProcAddress(hDll, "funcY");
```

- func5については序数でしかエクスポートされていない。この場合はMAKEINTRESOURCEA()マクロを使う。
  - `GetProcAddress()` の第二引数は本来はLPCSTRだが、上位WORDを0埋め＋下位WORDに序数値を指定したDWORDでも良い。 
  - `MAKEINTRESOURCEA()` マクロを使うとそのような値を作ってくれるので、これで序数によりエクスポートシンボルのアドレスを取得出来る。 

```
func5 = (DLLFUNC)GetProcAddress(hDll, MAKEINTRESOURCEA(8));
```

- エクスポート名が有る場合でも、序数値からエクスポートシンボルを取得することは可能。例えばfuncY(func7)については次のようにしても問題ない。 

```
funcY = (DLLFUNC)GetProcAddress(hDll, MAKEINTRESOURCEA(13));
```

