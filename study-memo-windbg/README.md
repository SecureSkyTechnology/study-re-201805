# WinDbg の勉強メモ

セキュリティ対策技術の向上を目的としたバイナリ解析の知識を深めるため、Windows上でのデバッガ WinDbg  の使い方を勉強・整理したメモ。

公式サイト : https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/

## 検証時の環境

- Windows 10 Pro 64bit 日本語版
- [こちらの記事](../setup-vs2017-windbg/README.md) に従ってVisual Studio 2017とWinDbgをインストールした状態
- Visual Studio 2017 Community Edition
- WinDbg 10.0.16299.15

## "Getting Started with WinDbg (User-Mode)" で始めてみる

- "Getting Started with WinDbg (User-Mode)" というチュートリアルがあるので、これでwindbgを触ってみた。
  - https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/getting-started-with-windbg
- チュートリアル記事そのままでは分かりづらかったり、ハマった点があるので以下にメモ。

### notepad.exe のデバッグ -> シンボルロードで時間がかかる。

- 最初の `.sympath` コマンドが `.sympath srv\*` と表示されているが、正しくは `.sympath srv*` となり、エスケープ不要。
  - 以降の他のコマンドも `*` の前のバックスラッシュエスケープはエスケープ無しでOK.
- **シンボルのロードで10分以上時間がかかることがある。遅延してロードされるので、突然重くなったりする。辛抱強く待つこと。**

### 0除算例外の発生 -> サンプルコードを自力で作り直した。

notepad.exe のデバッグ例に続き、0除算が発生するサンプルコードで例外の解析をデモしている。

しかし掲載されているのは純粋に関数本体だけであり、これを単純にVisual Studioから C/C++ コンソールアプリケーションプロジェクトを作成して埋め込んでも、デフォルトで設定される様々なオプションの影響か、windbg上でステップ実行しても0除算が発生する関数呼び出しをスキップしてしまった。

そのため Visual Stusio 2017 の Native Tools でコマンドラインコンパイルできるようにした [hello-world-div0.c](./hello-world-div0.c) を作成し、以下のようにビルドしたところ、windbg上でも0除算例外を発生させることができた。

`/Zi` オプション(シンボル情報をデフォルトの `vc(バージョン).pdb` ファイルに出力)付きでコンパイル:

```
> cl /c /Zi hello-world-div0.c
```

.objファイルに.pdbファイルのパスが埋め込まれるので、linkコマンドはそれを見て複数のモジュールのシンボル情報を統合する。

`/DEBUG` オプションを付けてPDBを出力、exeにはデバッグ情報は含めない状態でリンク:

```
> link /DEBUG hello-world-div0.obj
```

以下が実際に実行したwindbgの操作:
1. windbgを起動し、"File" -> "Open Executable" で hello-world-div0.exe を開く。
2. Microsoftからのシンボルダウンロードを無効化する。具体的には、Symbol Path に cache も srv も指定せず、単に空のディレクトリだけを指定する。
   - `.sympath C:\work\tmp\empty`
   - (ERROR が表示されるが気にしない)
3. hello-world-div0.pdb は探させたいので、ファイルのあるディレクトリを `.sympath` に `+` オプションを付けて追加する。
   - `.sympath+ C:\(...)\study-memo-windbg`
4. ソースコードがあるため、 `.srcpath` コマンドでソースパスも設定してみる。
   - `.srcpath C:\(...)\study-memo-windbg`
5. `.reload` コマンドを実行する。ntdll でシンボルファイルが見つからないエラーが出るが、気にしない。
6. hello-world-div0.exe 中の main 関数を探す。
   - `x hello-world-div0!main`
7. 無事シンボルが見つかれば、シンボル名でブレークポイントを設定する `bm` コマンドで main() にブレークポイントをセット。
   - `bm hello-world-div0!main`
   - (正しく張れたか `bl` コマンドで確認。)
8. `g` コマンドで実行開始
9. main()関数の先頭でブレークする。対応するソースコードが表示された。
10. `p` コマンドでステップ実行していく。
    - `pt` コマンドでレジスタを表示させたり、
    - `uf (6. で取得したmain関数のアドレス)` でmain()関数全体のアセンブルソースを確認したり、
    - `u` で現在のRIPからのアセンブルソースを確認したり、
    - `r` / `rF`, `rX` コマンドでレジスタ内容を確認したり、
    - `dc @rsp` でスタックポインタ以降のメモリ内容を表示したり、
    - `dv /V` でローカル変数を表示したり、
    - `k` や `kP` コマンドでバックトレースを表示したり、
    - div0() 呼び出しまで来たら `t` コマンドでステップインしたり、
    - するなどして練習がてら遊んで見る。
11. div0() 呼び出しの中で0除算例外が発生したら、 `!analyze -v` コマンドを実行し、解析結果が表示されることを確認した。
    - **シンボルダウンロードを無効化しているので、有効な内容が表示されない。シンボルが解決できると、詳細な結果が表示される。** 

## コマンドリファレンス

詳細はwindbgのヘルプから、"Debugger Reference" -> "Debugger Commands" 参照。
- `.` で始まるのはデバッガそれ自体の実行を制御するメタコマンド。(シンボルロードやデバッグ対象の終了・再起動など)
- `!` で始まるのはデバッガの拡張コマンド。(PEB/TEBを分かりやすく表示したり、例外の解析をするなど)
- それ以外がレジスタ・メモリを参照したり、デバッガ対象の実行制御を行うデバッガコマンド。

シンボル・ソースパス参照:
- `.sympath` : シンボルダウンロード/キャッシュの設定
- `.srcpath` : ソースパスの設定
- `.reload` : シンボルの再読込
- `!sym noisy` : シンボルロード時の詳細出力をON
- `!sym quiet` : シンボルロード時のエラー出力をOFF

※以下、一部のコマンドはシンボルが正常にロードされていないと動かないものがあります。

デバッグ対象の制御:
- `.attach PID` : 実行中のプロセスにアタッチ
- `.restart` : デバッグ対象プロセスを再起動
- `.kill` : デバッグ中のプロセスを強制終了
- `q` : デバッグセッション終了
- `qd` : デタッチしてデバッグ終了

モジュール・プロセス情報:
- `lm` : ロードされたモジュールの一覧
- `lm v m モジュール名` : モジュールの詳細情報を表示
- `!lmi モジュール名` : モジュールヘッダーの表示

シンボル情報:
- `x (option) モジュール名!シンボル名` : 指定されたモジュール名 + シンボル名の情報を表示。以下のようにワイルドカード(`*`)も使える。
  - `x myapp.exe!*` : "myapp.exe" 中の全てのシンボルを表示
  - `x myapp.exe!*myapp*` : "myapp" という文字列を含む "myapp.exe" 中の全てのシンボルを表示
- `ln Address` : 指定されたアドレスを含むシンボルを探して、表示

ブレークポイント:
- `bp` : マシン語アドレスにブレークポイント設定。
- `bm` : シンボル名でブレークポイント設定。例 : `bm helloworld!main`
- `bl` : ブレークポイント一覧
- `bc (ID)` : ブレークポイント削除 (`bc *` で全てのブレークポイント削除)
- `ba` : メモリへの読み書き実行などでのブレークポイント設定(詳細はヘルプドキュメント参照)

実行制御:
- 実行中のプログラムに対して一時停止 : windbg画面に戻り Ctrl-Break (CDB/KDBなら Ctrl-C)
- 実行再開 : `g` (Go) コマンド
- Step Over : `p` (steP) コマンド
- Step In : `t` (Trace) コマンド
- 現在位置から return するところまで実行 : `gu` (Go Up) コマンド
- 終了したプログラムを再実行 : `.restart` コマンド

バックトレース表示 : `k` コマンド (`kP` などオプションあり)

スレッド制御:
- `~` : スレッド一覧
- `~s` : 現在のスレッド情報表示
- `~(スレッド番号)s` : 指定したスレッドに移動

レジスタ内容表示:
- `r` : 主要レジスタ内容表示
- `r(レジスタ名)` : 指定したレジスタのみ表示。`r` の後ろに半角空白を入れても良いので、次は等しい : `rrax`, `r rax` (共にRAXの内容を表示する)

アドレス表記などにおけるレジスタ参照について:
- `@(レジスタ名)` とすることで、レジスタの値を参照できる。
  - 例えば `dc @rsp @rbp` とすると、RSPレジスタからRBPレジスタの範囲のスタック内容を表示する。
- `@(レジスタ名)+num` など簡単な足し算・引き算式も評価できる。
  - 例えば `dd @rsp @rsp+8` とすると、RSPレジスタから8バイト分のメモリダンプを表示する。
- `$` 始まりの疑似レジスタが用意されている。
  - `$ip`, `$peb`, `$teb`, `$tpid`, `$tid` ...
- 詳細は windbg ヘルプの "Register Syntax", "Pseudo-Register Syntax" 参照。

アドレスとレンジ表記について:
- アドレスは基本的には16進数を特にprefix無しでそのまま表記する。
  - (特殊なprefixを付けるとリアルモードやプロテクトモードのメモリを参照できるらしいが、ユーザモードのデバッグではまず使わないだろう)
- `.` は、現在のEIP/RIPを指すことが多い。
- レンジ表記では、`Address1 Address2` とそのまま書いて、Address1 から Address2 の範囲を指定したことになる。
  - 終端アドレスを `L(数)` 表記で簡略化することもできる。数の部分が実際何バイトになるかはコマンドによる。
  - 例1 : `80000000 L20` -> 0x80000000 から 0x8000001F まで。
  - 例2 : `80000000 L-20` -> 0x7FFFFFE0 から 0x7FFFFFFF まで。(80000000 からマイナスした、つまり範囲の始まりと終わりがひっくり返った)

ディスアセンブル:
- `u (Address or Range)` : 引数無しだと現在位置からディスアセンブル(= `u .`)。
- `uf (Address)` : 指定したアドレスを関数ブロックとしてディスアセンブル。 `uf .` も有効。
  - (`x` で関数名からアドレスを取得して、それで `uf` で関数をディスアセンブルするなど)

メモリ内容の表示:
- `dv` : ローカル変数情報の表示, `/i`, `/t`, `/V` オプションが便利
- `dt モジュール名!構造体/クラス名 Address` : 指定されたアドレスのメモリ内容を、構造体やクラスにマッピングしてダンプ表示する。(シンボルロード必須, 詳細はヘルプドキュメント参照)
- `da (Rane)` : ASCII文字
- `db (Rane)` : Byte + ASCII文字
- `dc (Rane)` : DWORD(4byte) + ASCII文字
- `dd (Rane)` : DWORD(4byte)
- `df (Rane)` : 単精度浮動少数(4byte)
- `dp (Rane)` : ポインタサイズ(32bit/645bitにより変わる)
- `dq (Rane)` : QWORD(8byte)
- `dW (Rane)` : Unicode 文字
- `dW (Rane)` : WORD(2byte)
- `dp (Rane)` : WORD(2byte) + ASCII文字
- `dps (Range)` : シンボル抽出
- `dpu (Rane)` : Unicode 文字列
- `ds (Address)` : STRING, ANSI_STRING
- `dS (Address)` : UNICODE_STRING

メモリ内容の検索 : `s` コマンド(詳細はヘルプドキュメント参照。文字列検索などもできる)

仮想メモリの使用状況などを表示 : `!address -summary` (シンボルロード必須, 詳細はヘルプドキュメント参照)

## TIPS

### とりあえずシンボルダウンロードを有効化したい

- `.sympath srv*` を実行する。
- 今回検証した時は、`C:\ProgramData\dbg\sym` 以下に保存された。
- Visual Studio も同じ場所を参照してくれるのかは不明。
- **シンボルパスは奥が深く、また突然のシンボルファイルのダウンロードでデバッガが固まるなどトラブルの要因にもなる。Microsoft公式のシンボルサーバからのダウンロードはかなり時間がかかる場合もあるので、ダウンロードが始まったら辛抱強く待つこと。**

### Windowsモジュールのシンボルダウンロードを無効化したい

- 最初に `.sympath (空のディレクトリ)` を実行する。
- これだけだと、シンボルロードのエラー出力が煩わしいので、 `!sym quiet` しておくと良さそう。
- その後、ローカルで開発・デバッグしてるPDBがあればそのディレクトリを個別に `.sympath+` しておくと良い。

### x86/x64 アーキテクチャのサマリとか無い？

あった: https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/processor-architecture

### 32bit版/64bit版のどちらを使えば良いのか？

- 64bitホスト上でユーザモードでデバッグするなら、64bit版のWinDbgを使えば良いらしい。
- 32bit版アプリをWOW64上で動かしてる場合は、 `.effmach` コマンドで32bitに切り替えられるらしい。
- 参考: "Choosing the 32-Bit or 64-Bit Debugging Tools"
  - https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/choosing-a-32-bit-or-64-bit-debugger-package

実際に試してみたところ、以下のように自動でx86モードに遷移し、 `.effmach` コマンドを実行する必要が無かった。

ただし、0除算例外発生でそのままステップ実行を続けたらいつのまにか64bitモードに切り替わってしまった。
するとコールバック表示がWOW64の中になってしまったので、改めて `.effmach x86` して32bitでのコールバック表示に戻した。

1. 0除算のサンプルコード [hello-world-div0.c](./hello-world-div0.c)を x86版 Native Tools コマンドプロンプト上でビルドする。
   1. `cl /c /Zi hello-world-div0.c`
   2. `link /DEBUG hello-world-div0.obj`
2. WinDbg 64bit版を起動し、上でビルドした32bit版の hello-world-div0.exe を開く。
3. 一応少しちゃんと見たいので、 `.sympath srv*` して、`sympath+` でアプリのPDBディレクトリ追加、`.srcpath` でソースパス設定。
4. `.reload /f hello-world-div0.exe` でシンボルロードさせる。
5. `g` で一旦進めてみると、以下のようにWOW64を検出して自動で32bitモードに切り替わった。(プロンプトにも `:x86` というのが追加で表示されるようになった)

```
0:000> g
ModLoad: 00000000`02b10000 00000000`02bbe000   WOW64_IMAGE_SECTION
ModLoad: 00000000`74680000 00000000`74750000   WOW64_IMAGE_SECTION
ModLoad: 00000000`02c10000 00000000`02d9f000   WOW64_IMAGE_SECTION
ModLoad: 00000000`5d810000 00000000`5d81a000   C:\WINDOWS\System32\wow64cpu.dll
ModLoad: 00000000`74680000 00000000`74750000   C:\WINDOWS\SysWOW64\KERNEL32.DLL
ModLoad: 00000000`75a10000 00000000`75be7000   C:\WINDOWS\SysWOW64\KERNELBASE.dll
(...)
(1b00.4dc): WOW64 breakpoint - code 4000001f (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
ntdll_77b00000!LdrpDoDebuggerBreak+0x2b:
77badb1f cc              int     3
0:000:x86> k
 # ChildEBP RetAddr  
00 00aff638 77ba8c37 ntdll_77b00000!LdrpDoDebuggerBreak+0x2b
01 00aff898 77b638da ntdll_77b00000!LdrpInitializeProcess+0x1b42
02 00aff8f8 77b637be ntdll_77b00000!_LdrpInitialize+0x10f
03 00aff90c 77b6376c ntdll_77b00000!LdrpInitialize+0x34
04 00aff91c 00000000 ntdll_77b00000!LdrInitializeThunk+0x1c
(コールスタックも32bitのアドレス表記になってる)
```

6. ここで main() 関数にブレークポイントを張って実行してみると、ディスアセンブルまで32bitで動いていることが確認できる。

```
0:000:x86> x hello-world-div0!main
003f6890          hello_world_div0!main (int, char **)

0:000:x86> bm hello-world-div0!main
  1: 003f6890          @!"hello_world_div0!main"

0:000:x86> g
Breakpoint 1 hit
hello_world_div0!main:
003f6890 55              push    ebp

0:000:x86> uf .
hello_world_div0!main [c:\(...)\study-memo-windbg\hello-world-div0.c @ 12]:
   12 003f6890 55              push    ebp
   12 003f6891 8bec            mov     ebp,esp
   12 003f6893 83ec08          sub     esp,8
   13 003f6896 c745f802000000  mov     dword ptr [ebp-8],2
   14 003f689d c745fc00000000  mov     dword ptr [ebp-4],0
   15 003f68a4 6a03            push    3
   15 003f68a6 8b45fc          mov     eax,dword ptr [ebp-4]
   15 003f68a9 50              push    eax
   15 003f68aa 8b4df8          mov     ecx,dword ptr [ebp-8]
   15 003f68ad 51              push    ecx
   15 003f68ae e851cbffff      call    hello_world_div0!ILT+9215(_div0) (003f3404)
(...)
```

7. そのまま実行を進めて0除算が発生すると、first chance では32bitのまま。

```
(2dfc.1be4): Integer divide-by-zero - code c0000094 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
hello_world_div0!div0+0x1d:
003f686d f77d0c          idiv    eax,dword ptr [ebp+0Ch] ss:002b:00bbf904=00000000
0:000:x86> k
 # ChildEBP RetAddr  
00 00bbf8f8 003f68b3 hello_world_div0!div0+0x1d [c:\(...)\study-memo-windbg\hello-world-div0.c @ 7] 
01 00bbf914 003f6b7a hello_world_div0!main+0x23 [c:\(...)\study-memo-windbg\hello-world-div0.c @ 15] 
02 (Inline) -------- hello_world_div0!invoke_main+0x1c [f:\dd\vctools\crt\vcstartup\src\startup\exe_common.inl @ 78] 
03 00bbf95c 74698674 hello_world_div0!__scrt_common_main_seh+0xf9 [f:\dd\vctools\crt\vcstartup\src\startup\exe_common.inl @ 283] 
04 00bbf970 77b64b47 KERNEL32!BaseThreadInitThunk+0x24
05 00bbf9b8 77b64b17 ntdll_77b00000!__RtlUserThreadStart+0x2f
06 00bbf9c8 00000000 ntdll_77b00000!_RtlUserThreadStart+0x1b
0:000:x86> r
eax=00000005 ebx=00d48000 ecx=00000002 edx=00000000 esi=00454d1c edi=02fa9d50
eip=003f686d esp=00bbf8f0 ebp=00bbf8f8 iopl=0         nv up ei pl nz na pe nc
cs=0023  ss=002b  ds=002b  es=002b  fs=0053  gs=002b             efl=00010206
hello_world_div0!div0+0x1d:
003f686d f77d0c          idiv    eax,dword ptr [ebp+0Ch] ss:002b:00bbf904=00000000
```

8. ここでうっかりステップ実行など処理を進めてしまうと、second chance で64bitに戻ってしまった。コールバックもWOW64の世界になってしまう。

```
(2dfc.1be4): Integer divide-by-zero - code c0000094 (!!! second chance !!!)
wow64!Wow64pNotifyDebugger+0x1d:
00000000`5d8ab3e5 65488b042530000000 mov   rax,qword ptr gs:[30h] gs:00000000`00000030=????????????????
0:000> k
 # Child-SP          RetAddr           Call Site
00 00000000`00abdb20 00000000`5d8aaacd wow64!Wow64pNotifyDebugger+0x1d
01 00000000`00abdb50 00000000`5d8ab62f wow64!Wow64pDispatchException+0x12d
02 00000000`00abe0c0 00000000`5d8ab824 wow64!Wow64NtRaiseException+0x117
03 00000000`00abe160 00000000`5d8a6523 wow64!whNtRaiseException+0x14
04 00000000`00abe190 00000000`5d811923 wow64!Wow64SystemServiceEx+0x153
05 00000000`00abea50 00000000`5d8bac86 wow64cpu!ServiceNoTurbo+0xb
06 00000000`00abeb00 00000000`5d8abea0 wow64!RunCpuSimulation+0xecd6
07 00000000`00abeb30 00007ffe`e4875b3d wow64!Wow64LdrpInitialize+0x120
08 00000000`00abede0 00007ffe`e481941e ntdll!LdrpInitializeProcess+0x18a1
09 00000000`00abf220 00007ffe`e48192db ntdll!LdrpInitialize+0x12e
0a 00000000`00abf2c0 00007ffe`e481928e ntdll!LdrpInitialize+0x3b
0b 00000000`00abf2f0 00000000`00000000 ntdll!LdrInitializeThunk+0xe
```

9. そこで `.effmach x86` コマンドを実行し、32bitの世界に戻った。

```
0:000> .effmach x86
Effective machine: x86 compatible (x86)
0:000:x86> k
 # ChildEBP RetAddr  
00 00bbf8f8 003f68b3 hello_world_div0!div0+0x1d [c:\(...)\study-memo-windbg\hello-world-div0.c @ 7] 
01 00bbf914 003f6b7a hello_world_div0!main+0x23 [c:\(...)\study-memo-windbg\hello-world-div0.c @ 15] 
02 (Inline) -------- hello_world_div0!invoke_main+0x1c [f:\dd\vctools\crt\vcstartup\src\startup\exe_common.inl @ 78] 
03 00bbf95c 74698674 hello_world_div0!__scrt_common_main_seh+0xf9 [f:\dd\vctools\crt\vcstartup\src\startup\exe_common.inl @ 283] 
04 00bbf970 77b64b47 KERNEL32!BaseThreadInitThunk+0x24
05 00bbf9b8 77b64b17 ntdll_77b00000!__RtlUserThreadStart+0x2f
06 00bbf9c8 00000000 ntdll_77b00000!_RtlUserThreadStart+0x1b
```

### OutputDebugString() の実験

[OutputDebugString()を呼ぶサンプルコード, debugout.c](./debugout.c) を作ってみたので、WinDbg上でどう出力されるか見てみる。

ビルド: `cl /Zi /Od debugout.c`

WinDbg上での実行結果 : そのまま表示された。

```
0:000> g
Hello, Debugger(1)
Hello, Debugger(2)
Hello, Debugger(3)
```
