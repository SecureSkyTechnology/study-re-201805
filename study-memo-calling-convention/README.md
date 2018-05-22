# 呼び出し規約の勉強メモ

- CTFやセキュリティコンテストで出題される問題を解いたり、チート対策等のセキュリティ対策を行うなど、セキュリティ対策技術を向上させる目的のために呼び出し規約を勉強した。
- 色々な呼び出し規約があるので、サンプルコードをコンパイルして "/FA" でアセンブルリストを出力し、各呼び出し規約でどのような引数渡しになるか確認してみた。
- ソースファイルをリポジトリに登録しているので、実際に手元でアセンブルリストを出力し、確認してみてください。
- アセンブルリストを出力するには Native Tools コマンドプロンプト上で x86版 : `build.bat x86` / x64版 : `build.bat x64` を実行します。
- 全体的に https://www.glamenv-septzen.net/view/615 でVC特化 + x64版追記した内容です。

総合参考:
- x86版呼び出し規約
  - https://docs.microsoft.com/ja-jp/cpp/cpp/calling-conventions
  - https://docs.microsoft.com/ja-jp/cpp/build/reference/gd-gr-gv-gz-calling-convention
- x64版呼び出し規約
  - https://docs.microsoft.com/ja-jp/cpp/build/x64-software-conventions

## C言語の関数で使われる呼び出し規約

[サンプルコード : regular.c](./regular.c)

### x86系 (cdecl, fastcall, stdcall)

いずれもx86系でのみ利用で決まる。x64版clコマンドにオプションを指定しても、無視されてx64用の呼び出し規約 or vectorcall が適用される。

- `cdecl (/Gd)` (VisualC++, およびUNIX系のデフォルト)
  - https://docs.microsoft.com/ja-jp/cpp/cpp/cdecl
  - 全ての引数を右から左の順でスタックにPUSHする。
  - **関数を呼ぶ(caller)側でスタックをクリーンアップする。**
    - 呼ぶ側はcall命令から戻ってきた後に自分でesp/rspをスタックにPUSHしたバイト分だけ即値を加算する。
    - (x86ではスタックはアドレスの小さい方へ進んでいくので、「スタックのクリーンアップ」＝「戻る」＝SPのアドレス値を大きくする)
  - 可変長引数を使う場合はこの呼び出し規約を使う必要がある。
    - 通常、引数がいくつPUSHされたのかを正確に判断出来るのは関数を呼ぶ側(caller)だけ。よってスタックのクリーンアップも関数を呼ぶ側(caller)で行う必要がある。
- `fastcall (/Gr)`
  - https://docs.microsoft.com/ja-jp/cpp/cpp/fastcall
  - ECX, EDX レジスタ経由で渡される。残りの引数は右から左へスタックにPUSHする。
  - (スタック渡しの引数がある場合は) **関数(callee)側でスタックをクリーンアップする。**
  - アセンブラレベルではRET imm16命令を使って、戻る時にスタックポインタを"imm16"バイト分だけ戻す。つまり、"imm16"バイト分だけSPが増える。
  - (x86ではスタックはアドレスの小さい方へ進んでいくので、「戻る」＝SPのアドレス値は大きくなる)
- `stdcall (/Gz)` (Win32 API の呼び出し用)
  - https://docs.microsoft.com/ja-jp/cpp/cpp/stdcall
  - 全ての引数を右から左の順でスタックにPUSHする。
  - **関数(callee)側でスタックをクリーンアップする。** (ここが cdecl との違い)
  - アセンブラレベルではRET imm16命令を使って、戻る時にスタックポインタを"imm16"バイト分だけ戻す。つまり、"imm16"バイト分だけSPが増える。
  - (x86ではスタックはアドレスの小さい方へ進んでいくので、「戻る」＝SPのアドレス値は大きくなる)

可変長引数参考:
- https://docs.microsoft.com/ja-jp/cpp/cpp/functions-with-variable-argument-lists-cpp
- https://docs.microsoft.com/ja-jp/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start

### x64での呼び出し規約

- 整数値とポインタ引数は **左から順に** RCX, RDX, R8, R9 レジスタ渡しとなり、5個目以降はスタック渡し(どうもPUSHではなく直接RSPのオフセットに書き込んでる)。
- 浮動小数点は **左から順に** XMM0 - XMM3 レジスタ渡しとなり、5個目以降は整数値と同様にスタック渡し。
- 雰囲気としては、fastcallを拡張したような感じ。
- x64になり、レジスタの使い方で volatile と non-volatile の2種類に別れた。
  - volatile は関数呼び出しにより関数の中で値が書き換わってしまうタイプ。RAX, RBX, RCX など。
  - non-volatile は関数呼び出ししても、関数の中で値が書き換わらないタイプ。R12 - R15 など。
    - これは、関数の中でもし non-volatile レジスタを使う場合は、関数自身が自分でそれらを退避し、returnする前に戻しましょう、そういう規約で作りましょう、という意味。
  - 詳細 : https://docs.microsoft.com/ja-jp/cpp/build/register-usage
- 可変長引数の場合、生成されたアセンブルソースを見た限りでは、関数(callee)からの戻りはRET0で、呼び出し(caller)側も特にスタックを戻していない。
  - これは、スタック渡しといってもPUSHしてるわけではなく、RSPからのオフセット書き込みで渡しているためと思われる。

### vectorcall : x86/x64 共通

- `vectorcall (/Gv)` (/arch:SSE2 以上をサポートする x86 or x64でサポート)
  - 浮動小数点やベクター値(?)をXMMレジスタを使って渡すことでパフォーマンスを向上させる。
  - 浮動小数点とベクター値(?)を返す時はXMM0レジスタを使う。
  - 関数(callee)側でスタックをクリーンアップする。
  - x86/x64 での細かい差異についてはMicrosoftのドキュメント参照:
    - https://docs.microsoft.com/ja-jp/cpp/cpp/vectorcall

## C++のthiscall

[サンプルコード : thiscall.cpp](./thiscall.cpp)

x86:
- 可変長引数を使わない時
  - 引数は右から左へスタックにPUSHされる。
  - **"this"ポインタはECX経由で渡される。**
  - stdcallと同様、 **関数(callee)側でスタッククリーンアップする。**
- 可変長引数を使う時
  - 引数は右から左へスタックにPUSHされる。
  - "this"ポインタは **最後に** スタックにPUSHされる。
  - cdeclと同様、 **関数を呼ぶ(caller)側でスタッククリーンアップする。**

x64:
- 可変長引数の使用有無に関わらず、x64での呼び出し規約が適用される。
- rcxにthisポインタが渡されるのはx86と同様

補足: "/Wall" で以下のwarningが出たのですが、今回については影響無いのと、C4514 が大量に出てしまうので `/wd4820`, `/wd4514` で個別に警告を抑制しました。

```
thiscall.cpp(10): warning C4820: 'foo': '4' バイトのパディングを データ メンバー 'foo::mf' の後に追加しました。
thiscall.cpp(49): warning C4820: 'foo': '4' バイトのパディングを データ メンバー 'foo::ms' の後に追加しました。
(...) warning C4514: '(string.h中の関数名)': 参照されていないインライン関数は削除されました。
```

## naked(x86のみ)

- 通常は関数の前後に自動生成される prolog/epilog が付かなくなる。
- 「生」の処理ブロックを書けるようになるので、デバイスドライバやOS開発に活用される。
- prolog/epilogが付かなくなる分、様々な制限が課される。詳細は下記Microsoftドキュメントを参照。
- **x64では使えない**
- 参考:
  - https://docs.microsoft.com/ja-jp/cpp/cpp/naked-function-calls
    - C++側のドキュメントが一番くわしい。
  - https://docs.microsoft.com/ja-jp/cpp/c-language/naked-functions
  - https://docs.microsoft.com/ja-jp/cpp/cpp/considerations-for-writing-prolog-epilog-code
    - prolog/epilog 

[サンプルコード : naked_unnaked.c](./naked_unnaked.c)
- naked 関数のサンプル。ビルドしてexeを実行すると、naked 関数に突入してプログラムが異常終了する。

[サンプルコード : naked_unnaked_custom.c](./naked_unnaked_custom.c)
- 以下を参考に、naked関数にインラインアセンブラで prolog/epilog を挿入し、正常に関数として呼べるようにしてみた。
- https://docs.microsoft.com/ja-jp/cpp/cpp/considerations-for-writing-prolog-epilog-code

## おまけ : 可能な限り不要なコードを除去するための cl コマンドオプション調整

- 例外処理やスタックのチェックなど、セキュリティや品質を向上させるさまざまな追加機能がある。
- しかし、呼び出し規約がどう実装されるのか、原則的な部分を確認するためには、そうした追加機能は邪魔なので可能な限りOFFにしてみた。

```
/nolog : 著作権非表示
/Wall : 全ての警告を出力
/Od : 全ての最適化をOFF

/GS (Buffer Security Check)
: デフォルトONなので無効化 -> "/GS-"

/guard (Enable Control Flow Guard)
: デフォルトOFFだが明示的に無効化 -> "/guard:cf-"

/GS (Enable Exception Handling), /EH (Exception Handling Model)
: コマンドラインツールから使う時はデフォルトOFFだが、念の為明示的に無効化するため "/EHs-c-" を指定。
(実は試しに "/GX-" をしてみたところ、"cl : コマンド ライン warning D9036 : 'EHs-c-' を使用してください ('GX-' は使用不可)" と表示されたのでそれに従った。)

/Oi (Generate Intrinsic Functions)
: 明示的に無効化 -> /Oi-

/RTC, /GZ : 明示的に無効化するオプションが無かったため、無視。

/TC or /TP : C or C++を明示

/FAs : アセンブルコードと元のソースを保存。
/Fa(ファイル名) : アセンブルソースのファイル名をカスタマイズ。
/Fo(ファイル名) : .obj ファイル名をカスタマイズ。
/Fe(ファイル名) : .exe ファイル名をカスタマイズ。
```

