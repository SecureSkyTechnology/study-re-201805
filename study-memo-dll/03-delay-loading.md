# 勉強メモ 03 : DLLの遅延ロード(delay load)

- VC++ ではリンカのサポートにより、DLLの遅延ロードが可能となる。
- 暗黙的リンクでは、EXE実行の初期にDLLを全てロードする。しかし遅延ロードを組み込むことで、暗黙的リンクを有効にしつつ、エクスポート関数を初めて呼び時に自動でロードするような動きを実現できる。
- 参考:
  - "リンカーによる DLL の遅延読み込み"
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/linker-support-for-delay-loaded-dlls
  - https://www.glamenv-septzen.net/view/671

今回は「こういう機能もあるんだな～」程度に留めておく。今後、実際に使ってみるときが来たら、サンプルコード作成などして調査してみる。


