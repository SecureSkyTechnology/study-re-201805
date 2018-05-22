# DLL の勉強メモ

[Native Tools の勉強メモ](../study-memo-vs2017-native-tools/) でDLLの作成について簡単に触れているが、セキュリティ対策技術の向上を目的としたバイナリ解析の知識を深めるため、もう少し詳しく勉強してみたメモ。

1. [勉強メモ 01 : モジュール定義ファイル(".DEF")を使ったDLLのエクスポート](./01-def-export.md)
2. [勉強メモ 02 : DLLのエクスポート転送(forwarding)](./02-dll-forwarding.md)
3. [勉強メモ 03 : DLLの遅延ロード(delay load)](./03-delay-loading.md)
4. [勉強メモ 04 : DLLの事前バインド(BindImageEx())](./04-pre-dll-binding.md)

全体的な参考:
- "Visual C++ の DLL"
  - https://docs.microsoft.com/ja-jp/cpp/build/dlls-in-visual-cpp

## 検証時の環境

- Windows 10 Pro 64bit 日本語版
- [こちらの記事](../setup-vs2017-windbg/README.md) に従ってVisual Studio 2017とWinDbgをインストールした状態
- Visual Studio 2017 Community Edition
- WinDbg 10.0.16299.15

※今回の勉強メモでは基本的にx64版 Native Tools, WinDbg での実行結果を記載しています。特に差異が無い限りは、x86版 Native Tools, WinDbg での実行結果は省略しています。

