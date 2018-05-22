# 勉強メモ 02 : linkコマンド, dumpbin コマンドを触ってみる。(/SUBSYSTEM:{CONSOLE|WINDOWS} の実験)

- linkコマンドの `/SUBSYSTEM` オプションで、コンソール/GUIアプリのどちらかを指定できるらしいので、シンプルなコンソール/GUIアプリケーションで実験してみる。
- また、dumpbin コマンドを使ってみて、上記実験で生成したexeの、PEフォーマットのヘッダー中の "subsystem" がどうなったか見てみる。
- 参考URL
  - コンパイラ(cl.exe)オプション
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/compiler-options
  - リンカ(link.exe)オプション
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/linker-options

## 実行ファイルのフォーマット(PE Format)について

exe/dllにはコンパイルされたマシン語やデータが含まれ、それらは "PE Format(Portable Executable Format)" というフォーマットで保存される。
```
.c, .cpp -> .obj (COFF : Common Object File Format) -> .exe, .dll (PE Format : Portable Executable Format)
```

- .objで使われるCOFFについて気にする必要はほとんど無いが、 .exe, .dll のPEフォーマットについてはセキュリティ・チート・RE関連分野で必要になってくる。
- PEフォーマットの仕様については、文章で書かれた正式なものは無いようだ。
- 解説資料はいくつかあるので、以下にそれらのリンクをまとめた。
  - [PEフォーマットの解説資料リンクまとめ](../pe-format-links.md)
- 恐らく、正式なのはWindows SDK中に含まれるヘッダーファイル中の、PEフォーマット関連の構造体定義が事実上の「正式仕様」になると思われる。

この勉強メモの主旨としては、dumpbin コマンドを触ってみることでPEフォーマットの世界をチラ見することなので、特にPEフォーマットの詳細な知識はこの段階では不要。

## "/SUBSYSTEM:{CONSOLE|WINDOWS}" の実験について

- `/SUBSYSTEM` リンカオプションでコンソールアプリケーションか、GUIアプリケーションかを実行ファイルに設定することができる。
  - デフォルトは main()/WinMain() の有無で自動判別
- 実際にコンソール/GUIアプリケーションを切り替えることで、実行ファイルの設定がどう変化するかdumpbinコマンドで見てみた。
- 参考 : https://docs.microsoft.com/ja-jp/cpp/build/reference/subsystem-specify-subsystem
- [サンプルコードはこちら](./02-con-gui/)

## console.c と /SUBSYSTEM:CONSOLE の例

サンプルコードの console.c は main() 関数を持つコンソールアプリケーション。これをコンパイルしたときの実行ファイルをdumpbinで見てみる。

コンパイル -> `/SUBSYSTEM` 無しでリンクする例(x64):
```
> cl /c console.c
> link console.obj
> console.exe
Hello, World!
```

dumpbinコマンドでサブシステムの設定を確認してみると、"subsystem" が "Windows CUI" と表示される。リンカが main() 関数を見つけ、自動で設定してくれたっぽい。
```
> dumpbin /headers console.exe
(...)
OPTIONAL HEADER VALUES
             20B magic # (PE32+)
(...)
               3 subsystem (Windows CUI)
```

link コマンドに `/SUBSYSTEM:CONSOLE` を指定しても、同様の実行ファイルが生成された。
```
> link /SUBSYSTEM:CONSOLE console.obj
(dumpbin コマンドの結果は省略)
```

ここでわざと `/SUBSYSTEM:WINDOWS` を指定してみると、WinMain()関数が見つからずエラーとなった。
```
> link /SUBSYSTEM:WINDOWS console.obj
Microsoft (R) Incremental Linker Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.

LIBCMT.lib(exe_winmain.obj) : error LNK2019: 未解決の外部シンボル WinMain が関数 "int __cdecl __scrt_common_main_seh(void)" (?__scrt_common_main_seh@@YAHXZ) で参照されました。
console.exe : fatal error LNK1120: 1 件の未解決の外部参照
```

なお x86版のNative Tools コマンドプロンプト上で生成すると、以下のようにOPTIONAL HEADER の magic 番号などがx64版と異なってくるのが確認できる。subsystem設定については同様。
```
OPTIONAL HEADER VALUES
             10B magic # (PE32)
(...)
               3 subsystem (Windows CUI)
```

## gui.c と /SUBSYSTEM:WINDOWS の例

サンプルコードの gui.c は WinMain()関数を持つGUIアプリケーション(MessageBoxを表示するだけですが・・・)。これをコンパイルしたときの実行ファイルをdumpbinで見てみる。

なお MessageBox() APIを使っているので、MessageBox() APIが含まれている user32.dll を参照する必要がある。
今回は Windows SDK に含まれるインポートライブラリ user32.lib をリンクすることで user32.dll への参照を追加する。
（使いたいWindows API のインポートライブラリを調べるには、MSDNサイトでAPI名で検索してリファレンスを表示すると、下の方にインポートライブラリのファイル名が出ているのでそれを使う。）

**DLLやインポートライブラリについては別途解説予定。とりあえずおまじないと思っておくか、適宜検索して解説記事など参照。**


コンパイル -> `/SUBSYSTEM` 無しでリンクする例(x64):
```
> cl /c gui.c
> link gui.obj user32.lib
> gui.exe
(メッセージボックス表示)
```

dumpbinコマンドでサブシステムの設定を確認してみると、"subsystem" が "Windows GUI" と表示される。リンカが WinMain() 関数を見つけ、自動で設定してくれたっぽい。
```
> dumpbin /headers gui.exe
(...)
OPTIONAL HEADER VALUES
             20B magic # (PE32+)
(...)
               2 subsystem (Windows GUI)
```

link コマンドに `/SUBSYSTEM:WINDOWS` を指定しても、同様の実行ファイルが生成された。
```
> link /SUBSYSTEM:WINDOWS gui.obj user32.lib
(dumpbin コマンドの結果は省略)
```

ここでわざと `/SUBSYSTEM:CONSOLE` を指定してみると、main()関数が見つからずエラーとなった。
```
> link /SUBSYSTEM:CONSOLE gui.obj user32.lib
Microsoft (R) Incremental Linker Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.

LIBCMT.lib(exe_main.obj) : error LNK2019: 未解決の外部シンボル main が関数 "int __cdecl __scrt_common_main_seh(void)" (?__scrt_common_main_seh@@YAHXZ) で参照されました。
gui.exe : fatal error LNK1120: 1 件の未解決の外部参照
```

### main() と WinMain() が両方含まれている場合

サンプルコードの guicon.c は main() 関数とWinMain()関数の両方を含んでいる。これをコンパイルするとどうなるか？

コンパイル -> `/SUBSYSTEM` 無しでリンクすると、CONSOLEが仮定された旨の警告が表示される例(x64):
```
> link guicon.obj user32.lib
Microsoft (R) Incremental Linker Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.

LINK : warning LNK4031: サブシステムが指定されていません。CONSOLE を仮定します。

> guicon.exe
Hello, World!
```

WinMain() 関数は実行されない。
dumpbinで確認すると、"subsystem" が "Windows CUI" になっていたので、そのためと思われる。
```
> dumpbin /headers guicon.exe
(...)
               3 subsystem (Windows CUI)
```

そこで `/SUBSYSTEM:WINDOWS` を指定してリンクすると、警告が消えた。実行してみると WinMain() 関数が実行されてメッセージボックスが表示された。
```
> link /SUBSYSTEM:WINDOWS guicon.obj user32.lib

> guicon.exe
(メッセージボックス表示)
```

今度は逆に main() 関数が実行されなくなった。
dumpbinで確認すると、"subsystem" が "Windows GUI" になっていたので、そのためと思われる。
```
> dumpbin /headers guicon.exe
(...)
               2 subsystem (Windows GUI)
```

## おまけ : その他使うかもしれないリンカオプション

- `/OUT:filename`
  - 出力ファイル名を指定するらしい。デフォルトはコマンドラインで最初に指定された .obj ファイルのbasenameに .exe/.dll を付けたものになるらしい。
- `/NOLOGO`
  - 著作権情報とバージョン情報が非表示になるらしい。
- `/VERBOSE`
  - リンカの詳細情報が表示されるらしい。
- `/FUNCTIONPADMIN`
  - hotpatch可能な実行イメージを生成するらしい。引数で各関数の先頭に追加するpaddingバイト数を指定できるらしい。
  - モジュール(.obj)側も `/hotpatch` コンパイラオプションを指定してコンパイルされている必要があるらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/functionpadmin-create-hotpatchable-image
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/hotpatch-create-hotpatchable-image

### デバッグ情報生成系

- `/DEBUG`
  - デバッグ情報を生成し、exe/dllファイル名のbasename + ".pdb" に埋め込むらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/debug-generate-debug-info
- `/DEBUGTYPE:[CV | PDATA | FIXUP]`
  - 生成するデバッグ情報の種類を指定するらしい。`/DEBUG` 指定時のデフォルトは CV みたい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/debugtype-debug-info-options
- `/PDB:filename`
  - PDBファイル名を指定できるらしい。

### セキュリティ系

- `/DYNAMICBASE`
  - ASLR有効な実行イメージを生成できるらしい。(デフォルトON, `/DYNAMICBASE:NO` でOFF)
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/dynamicbase-use-address-space-layout-randomization
- `/HIGHENTROPYVA`
  - 64bit かつ `/DYNAMICBASE` ON の場合に高エントロピーのASLRを使う設定の実行イメージを生成するっぽい。(デフォルトON, `/HIGHENTROPYVA:NO` でOFF)
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/highentropyva-support-64-bit-aslr
- `/NXCOMPAT`
  - Windows データ実行防止機能(DEP)と互換性のある実行イメージを生成するっぽい。(デフォルトON, `/NXCOMPAT:NO` でOFF)
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/nxcompat-compatible-with-data-execution-prevention
- `/GUARD:CF`
  - 制御フローガード有効な実行イメージを生成するっぽい。(デフォルトOFF = `/GUARD:NO`)
  - 有効にするには `/DYNAMICBASE` 有効かつ、モジュール(.obj) が `/guard:cf` でコンパイルされている必要があるらしい。
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/guard-enable-guard-checks
