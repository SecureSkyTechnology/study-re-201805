# gdb の使い方メモ

セキュリティ対策技術の向上を目的としたバイナリ解析の知識を深めるため、Linux上でのデバッガ gdb の使い方を整理したメモ。

参考:

- `gdb の使い方・デバッグ方法まとめ`
  - http://uguisu.skr.jp/Windows/gdb.html
- `GDB Cheat Sheet`
  - https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf
- `GDBデバッギング覚え書き - しばそんノート`
  - http://d.hatena.ne.jp/shibason/20090624/1245840061
- `GDB入門`
  - http://www.tohoho-web.com/ex/draft/gdb.htm
- `GDB マニュアル - Memory`
  - http://flex.phys.tohoku.ac.jp/texi/gdb-j/gdb-j_41.html

## 起動

```
gdb (プログラム名)
gdb ./hello

-> gdbが起動され、プログラム実行前の状態。
```

`break main` などでブレークポイントなど設定できる。

```
run (プログラム引数)

-> 本来のプログラムに渡すプログラム引数を指定してプログラム実行開始
```

こんな感じでshell?も動かせる。
```
run $(perl -e 'print "Hello"')
run $(printf "\x41\x42\x43")
```

## ブレークポイント

```
break (関数名)
break (ファイル名:行数)
-> 関数名, 行数にブレークポイント設定

info break
i b
-> ブレークポイント一覧

delete N
-> N番目のブレークポイント削除

delete
-> 全てのブレークポイント削除
```

## ブレークポイントからの実行制御

```
step(s) : 関数の中までステップ実行
next(n) : 関数の中には潜らず、戻るまで1行実行
finish : 選択されてるスタックフレームから戻るまで実行(要するにreturnするまで)
until : ループ先頭に戻るまで実行

stepi(si) : 単一マシン命令を実行
nexti(ni) : 単一マシン命令を実行するが、サブルーチンコールの場合はサブルーチンから戻るまで実行。

cont(c) : プログラムの実行を再開
```

## stack frame, back trace

```
where
-> 現在のどのフレームにいるのか表示

frame n
f n
-> フレーム番号nを表示

up n
down n
-> フレームn個分を上下に移動

frame
f
-> 簡単なフレーム情報の表示

info frame
info f
i f
-> フレームの詳細表示

info args
i args
info locals
i locals
-> フレームの引数やローカル変数を表示できるらしい。

backtrace
bt
-> バックトレースの表示

backtrace n
bt n
-> n個分中のバックトレース表示

backtrace -n
bt -n
-> n個分外のバックトレース表示
```

## show registers

```
info registers
info all-registers
info registers eip
info registers rip
i r eip
i r rip
i r rdi rsi

i r a
-> print all registers 
```

## show source list

```
list
-> ソース・ファイル行の続きを表示します。 
既に表示された最後の行がlistコマンドによって表示されたのであれば、 その最後の行の次の行以降が表示されます。
しかし、 既に表示された最後の行が、 スタック・フレーム （スタックの検査参照） の表示の一部として1行だけ表示されたのであれば、 その行の前後の行が表示されます。

list +
-> 最後に表示された行の次の行以降を表示します。

list -
-> 最後に表示された行の前の行以前を表示します。

disassemble
disas
-> 選択中のフレームにおける関数全体のマシン命令をダンプ出力。

disas /m : 可能なら対応するCソースを出力
disas /r : マシン語命令の内容をhexで出力
disas /mr : 両方

disas from,to : from - to のアドレス内容をディスアセンブル出力
```

なおGDBのdisas出力はAT&T構文なので、レジスタには `%` が付き、値 -> 格納先の順になる。
つまり Intel 表記で即値1をEAXに格納するのが `mov eax, 1` なら、AT&T 表記だと `mov $1, %eax` となる。(即値には `$` が付く)

## dump memory

スタックポインタから高位32バイト(8WORD/4DWORD) (WORD = 32bit = 4byte) Hex表示

```
x/32xb $rsp
x/8xw  $rsp
x/4xg  $rsp
```

## LinuxでのASLR/Exec-Shield

参考:

- `ulimit -s unlimitedによる32bit ASLR無効化 - ももいろテクノロジー`
  - http://inaz2.hatenablog.com/entry/2014/07/23/001318
- `linuxカーネルの実行保護機能 - ぴょぴょぴょ？ - Linuxとかプログラミングの覚え書き -`
  - http://d.hatena.ne.jp/pyopyopyo/20080222/p1
