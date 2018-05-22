# Visual Studio 2017 (Community Edition) の Native Tools を使ったC/C++開発の勉強メモ

セキュリティ対策技術の向上を目的としたバイナリ解析の知識を深めるために、Visual Studio 2017 (Community Edition) の Native Tools 用コマンドプロンプトを使って、コマンドプロンプトからC/C++開発を行えるように勉強したときのメモです。


1. [勉強メモ 01 : コマンドラインからのコンパイル(cl, link コマンド)の入門](./01-cl-link-command-1st-step.md)
2. [勉強メモ 02 : linkコマンド, dumpbin コマンドを触ってみる。(/SUBSYSTEM:{CONSOLE|WINDOWS} の実験)](./02-linker-subsystem-option.md)
3. [勉強メモ 03 : 静的ライブラリを作ってみる](./03-static-library-example.md)
4. [勉強メモ 04 : 動的リンクライブラリ(DLL : Dynamic Link Library)を作ってみる](./04-dll-example.md)

**Native Tools 用コマンドプロンプトの開き方**
- Windowsキーを押すなどしてスタートメニューを開き、"Visual Studio 2017" -> "VS 2017 用 x64 Native Tools コマンドプロンプト" を開く。
- スタートメニューやタスクバーにピン留めしておくと便利。


**重要 : 本資料における全てのMicrosoftサイトへのURLは、全て2018年5月における、それぞれの勉強・調査実施時点のものになります。Microsoftにより予告なく変更される場合がありますので、URLリンクが無効になった場合はお手数ですが関連キーワードで検索し直してください。**

## 検証時の環境

- Windows 10 Pro 64bit 日本語版
- Visual Studio 2017 Community Edition
  - [こちらの記事](../setup-vs2017-windbg/README.md) に従ってインストールした状態

検証時のバージョン情報, "VS 2017 用 x64 Native Tools コマンドプロンプト":
```
C:\Program Files (x86)\Microsoft Visual Studio\2017\Community>cl
Microsoft(R) C/C++ Optimizing Compiler Version 19.13.26131.1 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

C:\Program Files (x86)\Microsoft Visual Studio\2017\Community>link
Microsoft (R) Incremental Linker Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.
```

検証時のバージョン情報, "VS 2017 用 x86 Native Tools コマンドプロンプト":
```
C:\Program Files (x86)\Microsoft Visual Studio\2017\Community>cl
Microsoft(R) C/C++ Optimizing Compiler Version 19.13.26131.1 for x86
Copyright (C) Microsoft Corporation.  All rights reserved.

C:\Program Files (x86)\Microsoft Visual Studio\2017\Community>link
Microsoft (R) Incremental Linker Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.
```

注意：本勉強メモでは基本的に x64 版で実行した結果を記載しています。特に差異が無い限りは、x86版の実行結果は省略しているのでご注意ください。

