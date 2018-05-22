# 勉強メモ 02 : DLLのエクスポート転送(forwarding)

DLLにはエクスポートするシンボルの実体を別のDLLに転送(forward)する機能がある。例えば kernel32.dll のエクスポート情報を見てみると、ntdll.dll へ転送されているエントリーがいくつか見つかる。

```
> dumpbin /exports C:\Windows\System32\kernel32.dll
...
    849  350 000371F0 HeapQueryInformation
    850  351          HeapReAlloc (forwarded to NTDLL.RtlReAllocateHeap)
    851  352 000111B0 HeapSetInformation
    852  353          HeapSize (forwarded to NTDLL.RtlSizeHeap)
...
```

forwardingによるDLLの階層化やインポートライブラリの集約は、ライブラリやソースの構成・依存関係の整理に役立つだろう。

セキュリティ対策技術の向上を目的としたバイナリ解析の知識を深めるため、今回はモジュール定義ファイル(.def)を使ったDLLのforwardingを試してみた。

（全体的に https://www.glamenv-septzen.net/view/670 の内容の x64版 でのやり直しです）

## DLL側のサンプルコード

- [dll01.c](./02-dll-forwarding/dll01.c)
  - "funcA1()" の実体を提供する。
- [dll01.def](./02-dll-forwarding/dll01.def)
  - "funcA1()"をエクスポートする。
  - また "funcB1()" という名前でdll02.cの"funcB2()"をエクスポートする。これにより、"funcB1()"を呼ぶ場合はdll02.cの"funcB2()"に転送される。 
- [dll02.c](./02-dll-forwarding/dll02.c)
  - "funcB2()" の実体を提供する。

ビルド:

```
(最初にdll02.dllをビルドする)
> cl /c /W4 dll02.c
> link /dll dll02.obj

(次にdll01.dllをビルドする。リンク時にインポートライブラリ dll02.lib を追加する。)
> cl /c /W4 dll01.c
> link /dll /def:dll01.def dll01.obj dll02.lib
```

dll02.dll のエクスポート情報をdumpbinで確認してみると、"funcB2"がエクスポートしていることを確認できる。

```
> dumpbin /exports dll02.dll
(...)
  Section contains the following exports for dll02.dll

    00000000 characteristics
    FFFFFFFF time date stamp
        0.00 version
           1 ordinal base
           1 number of functions
           1 number of names

    ordinal hint RVA      name

          1    0 00001000 funcB2
```

続いて dll01.dll のエクスポート情報をdumpbinで確認してみると、"funcA1"と"funcB1"がエクスポートされており、さらに"funcB1" が dll02.dll の"funcB2" に転送される設定になっている。

```
> dumpbin /exports dll01.dll
(...)
  Section contains the following exports for dll01.dll

    00000000 characteristics
    FFFFFFFF time date stamp
        0.00 version
           1 ordinal base
           2 number of functions
           2 number of names

    ordinal hint RVA      name

          1    0 00001000 funcA1
          2    1          funcB1 (forwarded to dll02.funcB2)
```

## EXE側のサンプルコード

- 暗黙的リンク(implicit-link)と明示的リンク(explicit-link)で2種類のexeを作ってみた。
- 後ほどデバッガでdll02.dllのロードタイミングを比べてみる。

### 暗黙的リンク(implicit-link)する main_implicit.c とビルド

- [サンプルコード : main_implicit.c](./02-dll-forwarding/main_implicit.c)


コンパイル時は dll01.lib を指定するだけでよい。dll02.libは不要。(後でwindbgでデバッグするため、`/Zi /DEBUG` オプションを追加している)

```
> cl /Zi /DEBUG main_implicit.c dll01.lib
```

インポート情報を見てみると"dll01.dll"として"funcA1", "funcB1"がインポートされることが確認出来る： 

```
> dumpbin /imports main_implicit.exe
(...)
  Section contains the following imports:

    dll01.dll
             140012220 Import Address Table
             14001AF80 Import Name Table
                     0 time date stamp
                     0 Index of first forwarder reference

                           1 funcB1
                           0 funcA1
```

実行:

```
> main_implicit.exe
funcA1(2, 3) = 5
funcB1(2, 3) = 6
```

### 明示的リンク(explicit-link)する main_explicit.c とビルド

- [サンプルコード : main_explicit.c](./02-dll-forwarding/main_explicit.c)

ビルドではインポートライブラリ不要(後でwindbgでデバッグするため、`/Zi /DEBUG` オプションを追加している)：

``` 
> cl /Zi /DEBUG main_explicit.c
``` 

インポート情報を見てみるとKERNEL32だけが登録されていることが確認出来る： 

``` 
> dumpbin /imports main_explicit.exe
(...)
  Section contains the following imports:

    KERNEL32.dll
             140012000 Import Address Table
             14001AD38 Import Name Table
                     0 time date stamp
                     0 Index of first forwarder reference

                         1AF FreeLibrary
(...)
```

実行:

```
> main_explicit.exe
funcA1(2, 3) = 5
funcB1(2, 3) = 6
```

### 転送先のDLLのロードタイミング

- dll01.dll, dll02.dllとも、DllMain()にOutputDebugString()を組み込んでいる。
- デバッガ上でデバッグログなどを確認すれば、どのタイミングでDllMain()が呼ばれた、つまりロードされたかを確認出来る。
- 暗黙的リンク、明示的リンクでどのように転送先のDLL、今回ならdll02.dllがロードされるのかを比べてみた。
- デバッガにはOllyDbgを使用している。
  - `.sympath+`, `.srcpath` でシンボル(.pdb)パスとソースパスを設定。
  - `.reload /f main_[implicit|explicit].exe` した後、`x main_[implicit|explicit]!main` で main() のアドレスを取得してそこに `bp` でブレークポイント設定し、 `g` で実行、ブレークポイントで止まったら `p` でステップ実行。


暗黙的リンクを使ったmain_implicit.exeの場合、EXEがロードされて実行準備が整った段階で、dll01.dll, dll02.dllともにロードされた。

明示的リンクを使ったmain_explicit.exeの場合、dll01.dllは `hDll = LoadLibrary("dll01");` の段階でロードされ、DllMain()が実行される。
dll02.dllは `funcB1 = (DLLFUNC)GetProcAddress(hDll, "funcB1");` の段階でロードされ、DllMain()が実行される。
恐らく"funcB1"のエクスポートシンボルを解決する時にdll02.dllへの転送情報を見つけ、自動的にdll02.dllをロードしているものと思われる。 

なお当然であるが  `funcA1 = (DLLFUNC)GetProcAddress(hDll, "funcA1");` は"dll01.dll"のメモリ空間上のアドレスを返し、 `funcB1 = (DLLFUNC)GetProcAddress(hDll, "funcB1");` は"dll02.dll"のメモリ空間上のアドレスを返す。 **"dll01.dll"上にdll02.dll, funcB2を呼ぶようなラッパコードが生成され、そのアドレスを返す訳ではない。**

## おまけ : 転送したのと同じ名前のシンボルがあるとどうなる？

dll01.c/def では、"funcB1" を "dll02.funcB2" に転送していた。ここでもし、dll01.c 自身が funcB1() 関数を持っていたら何が起こるだろうか？

- [サンプルコード : dll03.c](./02-dll-forwarding/dll03.c)
  - dll01.c をコピーし、自前の funcB1() 関数実装を追加した。
- [サンプルコード : dll03.def](./02-dll-forwarding/dll03.def)
  - dll01.def をコピーし、中身はそのまま。
- 

dll03.c 単体でDLLを生成しようとすると、dll03.def の転送設定により dll02 を見に行く（にも関わらずインポートライブラリ未指定の）ため、"funcB2" シンボルの未定義エラーとなる:

```
> cl /c dll03.c

> link /dll /def:dll03.def dll03.obj
(...)
dll03.def : error LNK2001: 外部シンボル "funcB2" は未解決です。
dll03.lib : fatal error LNK1120: 1 件の未解決の外部参照
```

dll02.lib を追加したらビルド成功したが、dumpbinで見てみたところ、結局 dll03.funcB1 は dll02.funcB2 への転送となるようだ:

```
> link /dll /def:dll03.def dll03.obj dll02.lib

> dumpbin /exports dll03.dll
(...)
  Section contains the following exports for dll03.dll

    00000000 characteristics
    FFFFFFFF time date stamp
        0.00 version
           1 ordinal base
           2 number of functions
           2 number of names

    ordinal hint RVA      name

          1    0 00001000 funcA1
          2    1          funcB1 (forwarded to dll02.funcB2)
```

main_implicit.c を、dll01.lib ではなく dll03.lib をリンクして生成してみる:

```
> cl /Zi /DEBUG /Fedll03_main.exe main_implicit.c dll03.lib

> dumpbin /imports dll03_main.exe
(...)
  Section contains the following imports:

    dll03.dll
             140080388 Import Address Table
             1400807B8 Import Name Table
                     0 time date stamp
                     0 Index of first forwarder reference

                           1 funcB1
                           0 funcA1
```

実際に実行してみると、dll03.c 中の funcB1() ではなく dll02.c 中の funcB2() が呼び出されたことを確認できる。

```
> dll03_main
funcA1(2, 3) = 5
funcB1(2, 3) = 6
```
