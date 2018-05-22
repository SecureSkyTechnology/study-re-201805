# API Hook に挑戦

- 「リバースエンジニアリングバイブル」では第14章でAPI Hookについて解説している。
- セキュリティ対策技術向上を目的としてAPI Hookの知識を深めるため、自分でも API Hook に挑戦してみた。
- API Hook 対象としては `VOID Sleep(DWORD dwMilliseconds)` を選択。`dwMilliseconds` を Hook 関数中で 1/2 にして、元の Sleep() に渡すようにしてみる。
  - うまく成功すれば、Sleep()をループで回すプログラムが、実行速度見た目2倍になるはず。
- 方式としては DLL転送 / IAT 書き換え / JMP命令直接書き込み と何種類かある。
  - DLL転送は簡単すぎるのと、IAT 書き換えでは `LoadLibrary() + GetProcAddress()` 方式に対応できない。
  - そこで、JMP命令直接書き込みに挑戦してみた。

筆者：坂本 昌彦

検証環境:
- Windows 10 Pro 64bit 日本語版
- Visual Studio 2017 Community Edition
  - [こちらの記事](../setup-vs2017-windbg/README.md) に従ってインストールした状態

検証時のバージョン情報, "VS 2017 用 x86 Native Tools コマンドプロンプト":

```
C:\Program Files (x86)\Microsoft Visual Studio\2017\Community>cl
Microsoft(R) C/C++ Optimizing Compiler Version 19.13.26131.1 for x86
Copyright (C) Microsoft Corporation.  All rights reserved.

C:\Program Files (x86)\Microsoft Visual Studio\2017\Community>link
Microsoft (R) Incremental Linker Version 14.13.26131.1
Copyright (C) Microsoft Corporation.  All rights reserved.
```

## 注意

- 本勉強メモおよびサンプルコードは x86 (32bit) 環境でのみ検証しています。
- 本勉強メモおよびサンプルコードの内容は、CTFやセキュリティコンテストで出題される問題を解いたり、チート対策等のセキュリティ対策を行うなど、セキュリティ対策技術を向上させる目的のために調査・検証したものとなります。犯罪や不正行為を助長するものではありません。
- 本勉強メモおよびサンプルコードの内容あるいはその内容から得た知識をもとに、一般的に流通するソフトウェアやサービスの利用規定に反する行為を計画・実践した場合、罪・違反・侵害・損害賠償などを問われ、罰せられる可能性があります。
- 本勉強メモおよびサンプルコードの内容あるいはその内容から得た知識をもとにしたいかなる行為も、またそれにより発生したいかなるトラブルや損害についても、筆者および筆者が所属する、あるいは筆者が過去所属した会社は一切の責任を負いかねます。

## ファイル構成とビルド方法

- [hook_rensyu.c](./hook_rensyu.c)
  - step1 として、まずは自分のプロセス内でSleep()をHOOKするのを実験したサンプルコード。
  - ビルド : `cl hook_rensyu.c`
- [hook.c](./hook.c)
  - step2 として、HOOK処理を DllMain() に分離したDLLのソースコード。
  - ビルド : `cl /LD hook.c`
- [dll_test.c](./dll_test.c)
  - hook.dll を `LoadLibrary()` してみて、HOOKがちゃんと動作するかテストするようのサンプルコード。
  - ビルド : `cl dll_test.c`
- [victim.c](./victim.c)
  - step3 として、実際にDLLをロードする対象となる、forループでSleep()し続けるサンプルコード。
  - ビルド : `cl victim.c`
- [inject.c](./inject.c)
  - step3 として、実際に victim.exe プロセスに hook.dll をロードさせるソースコード。
  - ビルド : `cl inject.c`

## ざっくりとした作業ログ

※実際はこの中でさらに、コンパイルしてはOllyDbgでデバッグしてを繰り返すなど、行ったり来たりしてます。書いて/読んで分かりやすいように実際の順序から前後させて整理しています。

### Step 1 : まずは自分のプロセス内でHOOKしてみる。

[サンプルコード](./hook_rensyu.c)

1. まず Sleep() がどう呼ばれるか調べるため、implicit link と `LoadLibrary() + GetProcAddress()` の2種類を呼び出すサンプルコードを作成し、OllyDbg でデバッグした。
2. その結果、以下のことが分かった。
   1. implicit link の場合は絶対間接CALL(`FF ...`) で、IATで kernel32.Sleep のアドレスが書き込まれた箇所を参照してそこをCALLしていることが分かった。
   2. 直接呼び出し の場合も、GetProcAddress() で取得した値を書き込んだメモリアドレスに対して絶対間接CALL(`FF ...`) でCALLしていることが分かった。
   3. 実際に kernel32.Sleep() にステップインしてみると、kernelbase.Sleep() へのJMP命令になっていた。
   4. kernelbase.Sleep() にジャンプすると、hot patch 用のMOV命令に続いて関数 prolog が始まった。
3. よって、以下のアプローチを試すことにした。
   1. kernel32.Sleep() = kernelbase.Sleep() へのJMP命令のアドレスを、独自に用意した `MySleep()` のアドレスに書き換える。
   2. `MySleep()` の中では、呼び出し元が渡してきた Sleep() の第一引数 `dwMilliseconds` を 1/2 にして、本来の飛び先である kernelbase.Sleep() にJMPする。
4. このときの注意点
   - kernel32.Sleep() が呼ばれたときのスタックの状態(ESP, EBP, およびスタックに積まれた `dwMilliseconds` や戻り先アドレスなど)を壊さないようにする。
   - `MySleep()` からは普通の関数呼び出しではなく、インラインアセンブラで kernelbase.Sleep() にJMPさせる必要がある。
   - そのため、`MySleep()` では通常のprolog/epilogは使えない。つまり、`__declspec(naked)` が必要となる。
5. ここで kernel32.Sleep() が呼ばれた = 元のJMP命令のところで止めた状態の ESP + 4 が `dwMilliseconds` であることを突き止めた。
6. そこから、インラインアセンブラで全部 `MySleep()` を書こうと思った・・・が、1/2 にする処理はCの世界で書きたい。
   - (将来的には共有メモリ使うなどして、外部からどれだけの倍速にするか変更できると面白そうなので、それをCで書く余地を残したかった)
7. そこで `MySleep()` 中で DWORD のローカル変数を宣言すると、それが EBP - 4 として参照されることが分かった。
8. 実際に動かしてみると、EBPはかなり深いところを指していたため、手動で prolog 部分を組み立てることにした。
   - 具体的には `PUSH EBP` -> `MOV EBP, ESP` して EBP = ESP の状態にした後、`dwMilliseconds` は ESP + 8 で参照するようインラインアセンブラを書き換えた。
   - さらに kernelbase.Sleep() を呼び出す直前で `POP EBP` してEBPを戻すようにした。
9. kernelbase.Sleep() の呼び出し方だが、いくつか試行錯誤した結果、DWORDとして保存しておいたkernelbase.Sleep()のアドレスをPUSHしてからRETする方式にした。
   - kernelbase.Sleep() のアドレスについては事前に `LoadLibrary() + GetProcAddress()` で取得しておく。
   - もうちょっと頑張ればちゃんと絶対間接参照でのJMPで書けたかもしれない・・・。
10. これで `MySleep()` は準備できたので、いよいよ kernel32.Sleep() で kernelbase.Sleep() にJMPするアドレスを、`MySleep()` のアドレスに書き換える。 **が、ここでJMP命令の絶対間接パターンに気付かず、ハマった。**
    - というのも、OllyDbgでの逆アセンブラ命令として "JMP ..." と表示されたので、対応するマシン語やオペランドの表記をよく読まずに、「JMPの後に kernelbase.Sleep() と表示されてるっぽいアドレスがあるから、 そのアドレスが書かれてる場所に `MySleep()` のアドレスを書き込めばそこにジャンプするはず」と思い込んでしまった。
    - 実際動かしてみたら、わけのわからない場所にジャンプしてしまい、実行できないメモリ領域にジャンプして実行できずエラーとなってしまう。
    - ここでようやく、絶対間接JMPを使っていることに気づき、 **そこのアドレスに飛ぶのではなく、そのアドレスに書き込まれているメモリ番地に飛ぶ** という動きになっていることに気づいた。
    - つまり `MySleep()` の先頭にかかれているマシン語を、間接参照してアドレス番地と読み取ってしまい、そこに飛んでしまった。
11. どうしようかな・・・というところで、諦めて「リバースエンジニアリングバイブル」p319 掲載の `MakeJMP` 関数をコピペさせてもらい、`E9` による相対ジャンプを書き込むことにした。
    - その結果、無事 `MySleep()` にジャンプできた。
    - じつは `MySleep()` のインラインアセンブラでEBPを調整したり、kernelbase.Sleep() へのジャンプを PUSH + RET 方式にしたりとかは、ここからいよいよOllyDbgを使って試行錯誤した結果だったりする。
    - さらに、実は最初は kernelbase.Sleep() のアドレスをJMP命令に続くDWORDから採取していた。 **が、これは上記の通り絶対間接なので、kernelbase.Sleep()のアドレスが書き込まれたメモリ番地である。** これにジャンプしてしまってたので、OllyDbg上ではいきなりわけのわからないアセンブラコードにジャンプ＋例外発生となり「ええええええ？？？？？」となった。
    - その後に、ちゃんと `LoadLibrary() + GetProcAddress()` で kernelbase.Sleep() のアドレスを取って、そこにジャンプするようにした。
    - この辺、アセンブラの基礎体力不足を感じる・・・。

### Step 2 : Hook処理をDLLに分離し、DllMain() で呼ばれるようにして、テストする。

1. 自分のプロセス内でのHookに成功したので、続いてHook処理を [hook.c -> hook.dll](./hook.c) に分離した。
2. さらに、`hook.dll` を `LoadLibrary()` して、その前後で Sleep() を呼んでみる [dll_test.c](./dll_test.c) を作成し、ビルド・実行してみた。
3. その結果、確かに `LoadLibrary()` 後の Sleep() が、ソースコード上で指定した値の 1/2 の時間で動いている = HOOK成功していることを確認できた。

### Step 3 : 実際にDLLをターゲットEXEプロセスにロードさせてみる。

1. Sleep()を何回もループで呼ぶサンプルコードを [victim.c](./victim.c) として作成した。
2. victim.exe プロセスを探して hook.dll を `CreateRemoteThread()` 経由で `LoadLibrary()` させるコードを [inject.c](./inject.c) として作成した。
   - 参考 : https://github.com/msakamoto-sf/cheap2el/blob/20110306-1.2.2/samples/replace_impaddr/remoteload.c
3. コマンドプロンプトを2つ立ち上げ、一方で victim.exe を起動したあと、もう一方で inject.exe を実行し、実行後、確かに Sleep() 秒数が 1/2 になって動くことを確認できた。
