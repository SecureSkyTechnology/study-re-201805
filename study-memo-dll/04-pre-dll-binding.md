# 勉強メモ 04 : DLLの事前バインド(BindImageEx())

- DLLの事前バインドとは、DLLがロードされた時のエクスポートシンボルのアドレスを事前に計算しておき、IATに予め書き込んでおく仕組み。 
- 多数のDLLをロードするEXEでは、DLLのロードと再配置＋IATの書き換えで起動時間が遅くなる場合がある。DLLを事前バインドしておくことで、IATの書き換えをスキップでき起動時間の短縮に効果がある。 
- ただしDLLのバージョンはもとよりOSのバージョン・環境が異なれば当然、ロードされるアドレスも変わる。開発環境で事前バインドしても、実際に動作するクライアント環境ではバインドされたアドレスが無効となる可能性がある。そのため通常はインストール時に、つまりクライアント上で事前バインドを行う。 
- 開発環境で事前バインドを試す場合は、コマンドラインツールの "EDITBIN.exe"を使う。
- クライアント上でのインストール時に事前バインドを行いたい場合は、インストーラの処理内で、イメージヘルプAPI(imagehlp.dll)のBindImageEx()APIを使う。 
- **注意1 : この機能は、EXEファイルのIATを書き換える。そのため、署名チェックが必要なファイルには適用できない。**
- **注意2 : ASLR機能(`/DYNAMICBASE` リンカオプション)が有効化された実行ファイルに対して EDITBIN で事前バインドしても、無視される。**
- 参考:
  - EDITBIN オプション : `/ALLOWBIND`
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/allowbind
  - LINK オプション : `/ALLOWBIND[:NO]`
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/allowbind-prevent-dll-binding
  - BindImageEx()
    - https://msdn.microsoft.com/library/windows/desktop/ms679279.aspx
  - イメージヘルプAPI
    - https://msdn.microsoft.com/ja-jp/library/windows/desktop/ms680181.aspx
  - https://www.glamenv-septzen.net/view/669

今回は「こういう機能もあるんだな～」程度に留めておく。
ASLR (`/DYNAMICBASE`) がデフォルトで有効化された現在、本機能を使用することはまず無いと思われるが、もし使うときがきたらサンプルコード作成などして調査してみる。
