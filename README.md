# re-study-201805

2018年4月 - 5月にかけて、Windowsでのリバースエンジニアリングについて「リバースエンジニアリングバイブル」を中心に勉強したメモやサンプルコード。

筆者：坂本 昌彦

目的：
- CTFで出題される問題を解いたり、チート対策を考えるためには以下のような技術・知識の理解が必要となる。
  - x86/x64 アセンブラ, 呼び出し規約
  - EXE/DLLのビルド方法とファイルフォーマット(PEフォーマット)
  - デバッガの使い方
  - API Hook

そのために、以下の調査を行った：
- Visual Studio 2017 と WinDbg をインストールし、EXE/DLLのビルド方法やx86/x64における呼び出し規約、WinDbgの使い方などを調査。
  1. [Visual Studio 2017 と WinDbg をインストール](./setup-vs2017-windbg/README.md)
  2. [コマンドラインで EXE/DLL をビルドするやり方を勉強](./study-memo-vs2017-native-tools/README.md) (C/C++の復習を兼ねて。)
  3. [WinDbg の Getting Started を見ながら WinDbg の使い方を勉強](./study-memo-windbg/README.md)
  4. [DLLの作り方のバリエーションを勉強](./study-memo-dll/README.md)
  5. [x86, x64 の呼び出し規約をアセンブラ出力で勉強](./study-memo-calling-convention/README.md)
  6. [リンカの /DYNAMICBASE の効果を検証](./study-memo-dynamicbase/README.md)
- 自作のサンプルコードで API Hook の実装を調査・検証。
  - [Sleep() API のHOOKに挑戦](./api-hook-challenge/README.md)
- [書籍「リバースエンジニアリングバイブル」](https://book.impress.co.jp/books/1113101030) を読み、読書メモを整理。
  - [リバースエンジニアリングバイブル](./books-reading-memo/reverse-engineering-bible.md)
- [書籍「解析魔法少女美咲ちゃん マジカル・オープン！」](http://www.shuwasystem.co.jp/products/7980html/0853.html) を読み、書籍中で解説されている解析作業やAPI Hookの内容を手元で再現し、検証した読書メモを整理。
  - [解析魔法少女美咲ちゃん](./books-reading-memo/re-magical-girl-misaki-chan.md)
- その他、Linuxでのデバッガの使い方を学ぶためにgdbの使い方を整理した。また、PEフォーマットについて調査するために参考URLを整理した。
  - [gdbでのデバッグについて勉強メモ](./gdb-intro.md)
  - [PEフォーマットの参考URLのメモ](./pe-format-links.md)

## 注意

- 本リポジトリで公開している全てのファイル内容は、CTFやセキュリティコンテストで出題される問題を解いたり、チート対策等のセキュリティ対策を行うなど、セキュリティ対策技術を向上させる目的のために調査・検証したものとなります。犯罪や不正行為を助長するものではありません。
- 本リポジトリで公開しているファイル内容あるいはファイル内容から得た知識をもとに、一般的に流通するソフトウェアやサービスの利用規定に反する行為を計画・実践した場合、罪・違反・侵害・損害賠償などを問われ、罰せられる可能性があります。
- 本リポジトリで公開しているファイル内容あるいはファイル内容から得た知識をもとにしたいかなる行為も、またそれにより発生したいかなるトラブルや損害についても、筆者および筆者が所属する、あるいは筆者が過去所属した会社は一切の責任を負いかねます。

## 補足 : Visual Studio 2017 で C/C++ 開発を勉強するには

以下の公式資料を参照してみると良さそう。
- https://docs.microsoft.com/ja-jp/cpp/windows/desktop-applications-visual-cpp
  - Visual C++ でデスクトップアプリケーションを作るためのチュートリアル。
- https://docs.microsoft.com/ja-jp/cpp/cpp/c-cpp-language-and-standard-libraries
  - Visual C++ でのC/C++言語と標準ライブラリのリファレンス。
- https://docs.microsoft.com/ja-jp/cpp/build/building-c-cpp-programs
  - C/C++ プログラムのビルド に関する包括的な解説とリファレンス。コマンドラインツールのリファレンスも含まれている。

