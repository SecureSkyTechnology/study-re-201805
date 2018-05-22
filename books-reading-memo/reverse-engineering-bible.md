# 「リバースエンジニアリングバイブル」読書メモ

- CTFやセキュリティコンテストで出題される問題を解いたり、チート対策等のセキュリティ対策を行うなど、セキュリティ対策技術を向上させる目的のために「リバースエンジニアリングバイブル」を読み、つまづいたところやツールなどを整理した読書メモです。
  - https://book.impress.co.jp/books/1113101030
- 主につまづいたところなどのメモ。
  - 2013年に初版発行されたものを、5年たったWin10 Pro 64bit上でサンプルコード(exeなど)動かしてみると色々動かなくなってたりしてた。
  - Windows Defender やAVに色々消される。

筆者：坂本 昌彦

## 第1部 リバースエンジニアリングの基礎(第1～3章)

第1～3章
- サンプルコードzipに含まれているソースコードやexeを、exeのIDA解析結果やデバッグ結果と突き合わせながら読み込んでいった。

第4章
- 「パッキングされたDLLのDllMain()を探索」で、サンプルコードzipには Themida でパッキングされたバージョンのDLLが含まれていたようだが、まさしくウィルスに使われているのと同じコードが含まれているせいか、Windows Defender により削除されてしまった・・・。

## 第2部 リバースエンジニアリング中級(第5～7章)

第5章, 第6章
- サンプルコードzipに含まれているソースコードやexeを、exeのIDA解析結果やデバッグ結果と突き合わせながら読み込んでいった。
- 文字列処理系は、1～2回読み直してもまだ理解しきれない。rep系の命令と周辺レジスタとの組み合わせでパターンとして見分けられられるようになるには、もっと経験が必要そうだった。

第7章
- Windows Defender に削除されたのか分からないが、Chapter 7 のサンプルコードフォルダの中にexeが入っていなかった。
- そのため、VS2017でビルドし直した。(x86版で)
- VS2017の場合、MFCのインポートライブラリは以下のフォルダにあったので、それをOllyDbgにロードした。
- `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.14.26428\atlmfc\lib\x86`
- mfc140.lib / mfc140d.lib / mfc140u.lib などと何種類かある。これは 実際にプログラムをOllyDbg上で実行し、実行モジュール一覧でどれがロードされるのかを確認し、それに対応するインポートライブラリを選択する。
- 書籍ではその後、メッセージマップ構造体が `.rdata` セクションに含まれているのでそこから "Terminate" ボタンのリソースIDになる 0xE9030000 を検索しているが、VS2017でビルドし直したバイナリで検索した場合、 `.rdata` セクションに 0xE903... を検索しても見つからなかった。
- もともと上記操作は、メッセージハンドラを探すための操作。他のアプローチとして、"Terminate" ボタンをクリックすると "hi !" というメッセージボックスが表示される。
- そこで、 "hi !" という文字列をモジュールの全ての参照文字列一覧から探し出し、そのアドレス近辺から周辺を辿り直していくと、どうにかボタンをクリックしたときの関数まで到達できた。
- ちょっとメッセージハンドラの仕組みが最新でどう変わったか、バイナリ生成時にどう変わったのか不明点が大きい・・・。

## 第3部 演算ルーチンのリバーシング(第8～9章)

第8章
- FSC_Level1.exe を実行してみたら msvcr71.dll が見つからず実行できなかった。(Win10上・・・)
  - msvcr71.ddll 自体は大分古く、VC++ 2003 時代
    - https://support.microsoft.com/en-us/help/326922/redistribution-of-the-shared-c-runtime-component-in-visual-c
  - MS公式からDLできる再頒布バージョンも、恐らくその次の 2005 から
    - https://www.microsoft.com/ja-jp/download/details.aspx?id=3387
  - MSのQAサイトで、英語版のアーカイブサイトから英語版をDLできるらしいことが書いてあるが、アーカイブサイトのURLアクセスするとサイトはダウンしてた。
    - https://answers.microsoft.com/ja-jp/windows/forum/windows8_1-winapps-appother/msvcr71dll/89a54257-c9af-48bb-b68c-62e90383c441
  - 以下の記事を読むと、有志の人がVectorで公開してるパッケージをDLして、SysWOW64フォルダにコピーして再起動すれば動いたよ！とのことだが・・・
    - http://tade-blog.info/personal-computer/1734/
    - https://www.vector.co.jp/soft/win95/util/se435079.html
  - 正直、かなり古い環境を前提としたバイナリということが分かり、無理に今の環境で動かすことはせず、とりあえずIDAの解析結果を本と突き合わせて座学で読み進めて終わらせた。
  - → IDAでロードしたら Strings 一覧から "Sorry" or "Conguratulation" を探してダブルクリック -> `.rdata` セクションのダンプが表示されたら、xref でコードロケーションが表示されるのでそれをダブルクリックすると、チェックロジックのフローが表示される。
  - → 簡単な条件分岐になってるので、文字列比較しているところは分かりやすい。「正解文字列」をどう用意しているのか最初分からなかったが、別の文字列の中から一文字ずつ切り出してたので、デバッグ実行してブレークすれば、すぐわかったと思う。
- FSC_Level2.exe は msvcr71.dll が無くてもすんなり実行できた。
  - upx最新版で無事unpackできたが、ざっとOllyDbgで動かしたりIDAで解析してみたが、特にIDAの解析でコードブロックが少ないのに中の制御がぐちゃぐちゃで、一部解析失敗のメッセージすら出てしまう。
  - さらに、MessageBoxやCommandLineなどのCALLを探ってみても、妙に PUSHでコードブロックのアドレスをPUSH-> C3 (RET) が多くてごちゃごちゃしている。
  - 仕切り直して、アンパックしたEXEをPEiDに突っ込むとEPが 0x0000135A と表示される。OllyDbg側の実行モジュール一覧だと EXE は0x00400000にロードされてるので、EPを足して 0x0040135A でブレークポイント設定してプログラムをリスタートしてみたら、どうにか止まってくれた。
  - そこから少しステップ実行して驚いたのは、このコードは、次のコードのアドレスをESPにPUSHして、その後RET命令を呼ぶことで次の処理にジャンプさせていて、それにより処理全体をぶつ切りにした上でシャッフルしている。
  - → ステップ実行で、どうにか GetCommandLineA() を呼んで処理を始めるところはわかったが、ロジックについてはギブアップして書籍の解説に頼った。
  - そのため、元の(packedの)PEファイルのTLSの中でデバッガかどうか判定しているロジックが文字列判定まで影響しているところについても、自分で理解するところまで到達できなかった。
  - が・・・それとは別に、試しにオリジナルのexeで、書籍で紹介されている「正解」を入れてみても、成功メッセージが表示されなかった・・・。何か間違えたかな・・・。

第9章
- PEフォーマットからENTRY POINTにブレークするところまではできたが、そこから先のロジック解析はまるで理解できず、ギブアップ。

## 第4部 アンチリバースエンジニアリング(第10～13章)

- 第10章はサンプルコードを実際にデバッガ上から動かして「おー、確かに検出できてる」と感心して、書籍の解説読んで終わり。
- 第11章も同様だが、デバッガを全部終了させているにも関わらず、OutputDebugString() で検知してしまう・・・なぜ・・・？
  - また第11章の anti2.exe を explorer からダブルクリックで実行しようとすると、Windows Defender になぜか実行を止められてしまう。
  - コマンドプロンプトから実行すると、親プロセスが explorer.exe にならないので、どうしてもそちらのチェックで検知が発生してしまった。

第12章
- anti3.exe をエクスプローラから実行しようとすると Windows Defender に止められる。コマンドプロンプトから実行しても、何も表示されない。
- ・・・と思いきや、ここにはself debuggingのデモとして `"C:\\FantasticTools\\DllAdmin\\AdvancedDll.exe"` を CreateProcess() してそこにアタッチするコードが含まれてて、そこでずっとwaitになってしまってた。
- そこを書籍通りに `GetCommandLine()` をCreateProcess()するよう修正すれば、無事（？）自分自身を起動してセルフデバッグしたことになる。
- ただ、どうもOllyDbgやうさみみハリケーンからは、CreateProcess()したほうのプロセスが見えない。デバッガの方でプロセス一覧から除外してるのだろうか。
- WinDbgからは、確かにCreateProcess()したプロセスが見えて、invasive な attach はエラーになった。(Non invasive なら attach 自体は成功したが、そこからどうやってメモリ内容など解析しに行くかはスキル不足で不明。)
- その他、スタックセグメントレジスタの検知がうまく動いてくれなかった。
  - こちら、書籍では "「push ss」「pop ss」の先のコードによって、デバッグ中はEAXに0xFFFFFFFFが入り、～" とあるが間違いではなかろうか？
  - その後の pushs -> pop eax により、デバッグ中であろうとなかろうと、flagsがEAXに入ってそこでTF(Trap Flag, 8bit)とマスクして0/1判定しているが、デバッグ中であろうとなかろうとそこは実行される。
  - 正直、pop ss の次の命令でブレーク出来ないということが、どうアンチデバッグになるのか分からない・・・。
- HWブレークの検知については、OllyDbgでHWブレークポイントを適当に設定して動かしてみたらちゃんと検知してくれた。
  - ただ、書籍の方ではその解説に Hello, World するだけの sample.exe を使う流れで書いてあるのだが、Chapter 12 の binary フォルダに入ってる sample.exe は普通に anti3.cpp をビルドしたものっぽい感じで、どうも違うっぽい。
  - とりあえず OllyDbg からHWブレークポイントセットできたし、検知も動いたのでこの章はこれで終わりにしちゃう。

第13章
- サンプルコードも無いので書籍の解説をざっと読んで終わりにした。
- 一個、OllyDbgで Ctrl-G で `fs:[30]` でPEBを表示できると書いてあったが、試してみたがよくわからないメモリに移動して終わって、よくわからない・・・。何か勘違いした？？
- OllyDbgとそのエコシステム（プラグイン）が本当にリバース界隈で盛り上がってて、OllyDbgがすごいよく使われてた、ということは分かった。

## 第5部 一段高いレベルのバイナリ作り(第14～16章)

第14章

- CreateRemoteThread()からのDLLインジェクションは、サンプルコードをビルドして実際に32bitバイナリの適当なexeとdllで動かしてみた。
- ProcessExplorer でDLLを見てみると、本当にDLLがロードされてた、すごい・・・。
- コードフックについては、書籍解説・サンプルは ws_32.dll の send() をフックしている、それも send のアセンブラコード中のジャンプ命令を改ざんしていた。
- なんでIATあたりを書き換えたりしないのかというと、他のフック(セキュリティソフトなど)が使ってる可能性があるからで、競合を避けるためとのこと。
- スゴイ・・・。
- 書籍ではMSNの送信パケットをキャプチャしていたが、適当なアプリが見当たらなかったので、手元での動作確認は断念。
- 頑張れば、HTTPSになる前の平文をキャプチャできたりするのだろうか。今ってWindowsアプリのHTTP(S)通信はどのAPI使ってるのかな・・・。機会があればその辺深掘りしても面白いかも。

第15章
- 手を動かしてて一番楽しい章だった。
- サンプルコードzipの中にあったexeをそのまま動かして、書籍のとおりに色んなパッチングを試して、「おお～」って感じになれる。
- サンプルコードzipにはソースコードもあるので、元のソースも見れる。先にソースを見てしまうと楽しみが半減するかも。

第16章
- ざっと解説を座学で通した。
- Windows API の "MOV EDI, EDI" については以下のように、WindowsXP頃からホットパッチング用に用意されていたらしい。
  - https://stackoverflow.com/questions/11337433/windows32-api-mov-edi-edi-on-function-entry
  - https://redchat.exblog.jp/18259671/
  - https://lab.planetleaf.com/windows/apihook-code.html

## その他

- OllyDbg のCPUウインドウで、Ctrl-G でAPI名(シンボル名？)入力したらそこにジャンプできるの、知らなかった。超便利。
- FS レジスタ + OFFSET という組み合わせのアドレス参照があると TEB へのアクセス。(x64だとGSレジスタになる via https://en.wikipedia.org/wiki/Win32_Thread_Information_Block )
- VC++の組み込みアセンブラ `__asm` で、 `_emit` 疑似命令でバイナリを直接埋め込める。データを生で埋め込むときなどに使える。
  - https://docs.microsoft.com/ja-jp/cpp/assembler/inline/emit-pseudoinstruction
- 全体的に色々凄かった。

## 「リバースエンジニアリングバイブル」のサンプルコード

「リバースエンジニアリングバイブル」のサポートサイトからサンプルコードのzipファイルをDLできる。
  - https://book.impress.co.jp/books/1113101030

結構大きなzipで、展開するとソースコードだけでなく、PE解析/デバッグ系ツールのパッケージやインストーラも含まれていた。

ちなみに、サンプルコードのDL先の案内が、表紙めくって最初の1ページ目をめくったところに書いてあったので、最初見落としてて「これ、サンプルコードのDL案内とか無いけど、どうやって勉強すれば良いんだ・・・」と不思議に思いながら第8章くらいまで座学で読んでて、その後ふと1ページ目をぱらっとめくって、そこで初めてサンプルコードをDL出来るのを知って激しく落ち込んだ。

### サンプルコードzipに含まれていたソースコードについて

- Visual Studio のプロジェクトファイルとして作成されている。
- ほとんどはexeが含まれているのでビルドする必要はないが、いくつかはexeが含まれてないので、実行にはビルドし直す必要がある。
- また、アンチデバッグ系のサンプルコードでは色々なアンチデバッグの技術を盛り込んだコードになっておりそのままでは動かなかったりする。
  - → 書籍の解説と見比べつつ、コメントアウトしてビルドし直して動作確認したりした。
- まとめると、Visual Studio が必要。

### サンプルコードzipに含まれていたPE解析/デバッグ系ツールについて

以下のツールのパッケージやインストーラが含まれていた。
初版2013年ということもあり、いろいろ古くなっていたので、検索して新しいのをDLし直したりした。

- dbgview : 次のサイトから最新版をDLし直した。
  - https://technet.microsoft.com/ja-jp/sysinternals/debugview.aspx
- ExeInfoPE : 次のサイトから最新 0.0.4.9 をDLし直した。
  - http://exeinfo.atwebpages.com/
  - こちらの、PE解析ツールの紹介記事でも第一位で紹介されてた。
  - http://www.wivern.com/security20141205.html
- HeWin : バイナリエディタだが、Bz, Stirling などあるので自分はそっち使った。
- ImportREC : 本来は "ImpREC" らしいのだが・・・
  - https://www.aldeid.com/wiki/ImpREC : これが作者っぽいのだけれど・・・
  - http://www.woodmann.com/collaborative/tools/index.php/ImpREC : これが作者サイトからリンクされてる公式DLサイトっぽいのだけれど・・・
  - https://tuts4you.com/e107_plugins/download/download.php?action=list&id=38 : "Tuts 4 You" という、やや昔のRE系フォーラムのDLページ。
  - 最新は 1.7 っぽいのだけれど、上記の通りどれからDLすればよいのかイマイチ不明で、サンプルソースzip同梱も 1.6 とそこまで古いわけではないので、諦めてサンプルソースzip同梱の物を使う。
  - ImpREC はアンパックしたEXEのIATを再構築するためのツールっぽい。以下参考。
    - https://ameblo.jp/norio001/entry-11932101391.html
    - http://www.wivern.com/ctf20141106.html
    - https://qiita.com/yakumo890/items/a458e993dcd37a595a89
- PEiD : サンプルソースzip同梱のものは、`Trojan:Win32/Bitrep.A` として Windows Defender に削除されてしまった。
  - 以下のサイトから改めて最新版をDLし直したら特に怒られなかったのでそれを使うことにした。
  - https://www.aldeid.com/wiki/PEiD
- ProcessExplorer : MSのsysinternalsから最新版をインストール済みなのでスキップ。
- StudPE : zip添付のものは 1.8.0 と古い。以下のサイトから最新版をDLし直した。
  - http://www.cgsoftlabs.ro/studpe.html
- WinDBG : 別途手動でWinDbgをインストール済みなのでスキップ。

なお、 `Part 1\Chapter 4\Dll\Binary\Dll_themida.dll` も Windows Defender に `Trojan:Win32/Bitrep.A` とみなされ削除されてしまった・・・。

### その他PE解析/Viewer系ツール入れてみた

以下のサイトに、ExeInfoPE以外にも良さげなツールが紹介されていた。
- wivern.com | 見る価値のある５つの PE 解析ツール
  - http://www.wivern.com/security20141205.html

そこから、以下をDLしてインストールしてみた。
- NTTCore CFF Explorer Suite
  - http://www.ntcore.com/exsuite.php
- FileAlyzer - Spybot Anti-malware and Antivirus
  - https://www.safer-networking.org/products/filealyzer/
- WJR Software - PEview (PE/COFF file viewer),...
  - http://wjradburn.com/software/

少し動かしてみた感じ、いずれも良さげな感じ。

### その他, IA-64, 32 のマニュアル

- Digital Travesia からIA-64, 32のマニュアル入手 : https://hp.vector.co.jp/authors/VA028184/#REFERENCE

### その他, リソースエディタ

- ResEdit : `.rc` ファイル(コンパイル前のテキスト表現リソースファイル)を作成・編集できる。Win32のGUIをVSのサポート無しで素組する場合などで使う。今もボチボチ開発継続してる。 **exe/dllからリソースを抽出するなどの解析系の機能はない。**
  - http://www.resedit.net/
  - http://gurigumi.s349.xrea.com/programming/visualcpp/resedit.html
- XN Resource Editor : `.res` ファイル(=コンパイルずみのリソースファイル)を編集したり、exe/dllから抽出・変更することもできる。
  - https://stefansundin.github.io/xn_resource_editor/
  - → オフィシャルサイトが閉鎖されてて、有志の人がforkして立ち上げたらしい。こちらのサイトでは ver 3.0.0.1 を公開している。
  - → ver 3.0.0.1 なら日本語化されたのを公開してくれてる人がいる。以下からzipをDLしたほうが早い。
  - https://ht-deko.com/tech053.html#XNRESOURCEEDITOR
- Resource Hacker(TM) : exe/dllなどからリソースを抽出し、変更することも可能。
  - http://www.angusj.com/resourcehacker/

触ってみた感じ、リバースエンジニアリングという観点からは XN Resource Editor が使いやすそうだった。

