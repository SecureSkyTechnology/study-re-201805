# "/DYNAMICBASE" リンカオプションの効果について実験してみたメモ

- link コマンドには `/DYNAMICBASE`, `/HIGHENTROPYVA` オプションがあり、デフォルトで有効となっている。(`/HIGHENTROPYVA` については64bitでのみ有効)
- セキュリティ対策技術の向上を目的としたバイナリ解析の知識を深めるために、EXE/DLL それぞれでこのオプションの有り/無しバージョンを作ってみて、実際にアドレスを表示し、効果を実験してみた。
- 参考:
  - "/FIXED (固定ベース アドレス)"
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/fixed-fixed-base-address
  - "/DYNAMICBASE (Use address space layout randomization)"
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/dynamicbase-use-address-space-layout-randomization
  - "/HIGHENTROPYVA (Support 64-Bit ASLR)"
    - https://docs.microsoft.com/ja-jp/cpp/build/reference/highentropyva-support-64-bit-aslr

## DLL側の有り/無しバージョン

- [サンプルコード : dll_lib.c](./dll_lib.c)
- foo(), bar() の2つをエクスポートしている。
- DllMain() が `DLL_PROCESS_ATTACH` で呼ばれた(=DLLがロードされた)時に、DllMain() の HINSTANCE型引数の値を OutputDebugString() で出力させてみた。

ビルド : `/DYNAMICBASE` ON/OFF の両方を生成してみる。(DLLの場合は "/FIXED:NO" がデフォルトなので特に指定しない)

```
> cl /c /W4 dll_lib.c
> link /DLL /DYNAMICBASE    /HIGHENTROPYVA    /OUT:dll_lib_aslr.dll   dll_lib.obj
> link /DLL /DYNAMICBASE:NO /HIGHENTROPYVA:NO /OUT:dll_lib_noaslr.dll dll_lib.obj
```

`/DYNAMICBASE` 指定バージョンのPEヘッダーを見てみると、"DLL characteristics" 中に "Dynamic base" と "High Entropy Virtual Addresses" が見える。

```
> dumpbin /headers dll_lib_aslr.dll
(...)
FILE HEADER VALUES
            8664 machine (x64)
(...)
OPTIONAL HEADER VALUES
             20B magic # (PE32+)
(...)
             160 DLL characteristics
                   High Entropy Virtual Addresses
                   Dynamic base
                   NX compatible
          100000 size of stack reserve
(...)
```

`/DYNAMICBASE:NO` 指定バージョンのPEヘッダーを見てみると、"DLL characteristics" 中に "Dynamic base" と "High Entropy Virtual Addresses" が無い。

```
> dumpbin /headers dll_lib_noaslr.dll
(...)
FILE HEADER VALUES
            8664 machine (x64)
(...)
OPTIONAL HEADER VALUES
             20B magic # (PE32+)
(...)
             100 DLL characteristics
                   NX compatible
          100000 size of stack reserve
(...)
```

## EXE側の有り/無しバージョン

- [サンプルコード : dll_lib.c](./dll_lib.c)
- foo(), bar() の2つをエクスポートしている。
- DllMain() が `DLL_PROCESS_ATTACH` で呼ばれた(=DLLがロードされた)時に、DllMain() の HINSTANCE型引数の値を OutputDebugString() で出力させてみた。

ビルド : `/DYNAMICBASE` ON/OFF の両方を生成してみる。(念の為明示的に `/FIXED:NO` を指定。)

```
> cl /c /W4 /Zi main.c
> link /DEBUG /FIXED:NO /DYNAMICBASE    /HIGHENTROPYVA    /OUT:main_aslr.exe   main.obj
> link /DEBUG /FIXED:NO /DYNAMICBASE:NO /HIGHENTROPYVA:NO /OUT:main_noaslr.exe main.obj
```

dumpbinでPEヘッダーを確認してみると、"DLL characteristics" 中に "Dynamic base" と "High Entropy Virtual Addresses" がそれぞれ有り/無しでDLLと同様に分かれた。(結果記載は省略)

"main_aslr.exe" を少し間を空けて(数分以上)何回か実行してみると、"I'm loaded at ..." のところが、時々変わる。また、"dll_lib_aslr.dll loaded at ..." のところも時々変わる。ただし、"dll_lib_noaslr.dll loaded at ..." のところは何回実行しても変わらない。

```
> main_aslr.exe
I'm loaded at 00007FF728200000
    dll_lib_aslr.dll loaded at 00007FFEDCE20000
  dll_lib_noaslr.dll loaded at 0000000180000000
foo(2, 3) in dll_lib_aslr.dll = 5
bar(2, 3) in dll_lib_noaslr.dll = 6
```

一方で、 "main_noaslr.exe" を何回実行しても、"I'm loaded at ..." のアドレスは変わらない。dll_lib_noaslr.dll も同じ。"dll_lib_aslr.dll loaded at ..." は時々変わる。

```
> main_noaslr.exe
I'm loaded at 0000000140000000
    dll_lib_aslr.dll loaded at 00007FFEDCE20000
  dll_lib_noaslr.dll loaded at 0000000180000000
foo(2, 3) in dll_lib_aslr.dll = 5
bar(2, 3) in dll_lib_noaslr.dll = 6
```

(でも気まぐれで、10分・20分経っても変わらないときがある。)

## editbin コマンドでDLL/EXEの /DYNAMICBASE フラグを変更してみる。

`/DYNAMICBASE` ありのexe/dllを、無しに変更してみる。

```
> editbin /DYNAMICBASE:NO dll_lib_aslr.dll
> editbin /DYNAMICBASE:NO main_aslr.exe
```

逆に なしの exe/dll を、アリに変更してみる。

```
> editbin /DYNAMICBASE /HIGHENTROPYVA dll_lib_noaslr.dll
> editbin /DYNAMICBASE /HIGHENTROPYVA main_noaslr.exe
```

この時、exeが `/FIXED` でビルドされていると、以下の警告が表示される。

```
main_noaslr.exe : warning LNK4259: '/DYNAMICBASE' は '/FIXED' と互換性がありません。イメージを実行できない可能性があります
```

実際にそれぞれ実行してみると、アドレスの変化が逆転したことを確認できた。(EXE/DLLとも)

