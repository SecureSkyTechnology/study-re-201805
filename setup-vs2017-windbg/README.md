# Windows 10 (64bbit) 環境に Visual Studio 2017 (Community Edition) と WinDbg をインストールする

セキュリティ対策技術向上を目的として C/C++プログラミングおよびアプリケーションのデバッグ方法を学ぶために、Visual Studio 2017 (Community Edition) と WinDbg をインストールする手順を整理した。

（説明は 2018 年 4月時点のURLや内容を元にしている）

Visual Studio は 本日時点での最新版が Visual Studio 2017 となっていて、無償エディションとして Community Edition が利用できる。
- https://www.microsoft.com/ja-jp/dev/products-visual-studio

Windows SDK については 以下から単体でDLできるが、「最新のVisual Studio 2017 RTM」のリンクからDLするVS2017に、Windows 10 SDK のコピーが既に収録済み。 **「Visual Studio インストーラーでユニバーサル Windows プラットフォーム開発作業ストリームを有効にする」がポイントっぽい。** 
- https://developer.microsoft.com/ja-jp/windows/downloads/windows-10-sdk

なお最初に上げたVS2017のページから Community Edition でDLしたインストーラと、「最新のVisual Studio 2017 RTM」のリンクからDLしたインストーラは sha1sum 同一。要するに、エディションやSDKの有無でインストーラを分けているのではなく、共通のインストーラにした上で、インストール途中にエディションやらインストールするSDKの内容を調整しているものと思われる。

## Visual Studio 2017 (Community Edition) のインストール

1. https://developer.microsoft.com/ja-jp/windows/downloads/windows-10-sdk から「最新の Visual Studio 2017 RTM」のリンクをクリックする。
   - [URL画像](./vs2017-ce-installer/vs2017-ce-installer-00a.png) 
   - "Visual Studio Community" の「無料でダウンロード」リンクをクリックすると、Visual Studio のインストーラがDLできるのでそれを実行する。
   - [URL画像](./vs2017-ce-installer/vs2017-ce-installer-00b.png)
2. [インストーラで「ユニバーサル Windows プラットフォーム開発」を選択する。他に典型的な学習用のをいくつか選択。](./vs2017-ce-installer/vs2017-ce-installer-01.png)
3. [さらに「個別のコンポーネント」から、念の為「MFC と ATL のサポート (x86 と x64)」を追加。](./vs2017-ce-installer/vs2017-ce-installer-02-add-mfc-atl-support.png)
4. [さらに念の為「Windows 10 SDK (一番新しいバージョン)」を追加。](./vs2017-ce-installer/vs2017-ce-installer-03-add-win10-sdk-latest.png)
5. インストール中... **ディスクサイズが23GB近く使う。また、4GBほどのインターネットダウンロードが発生する。**
6. [インストール完了。](./vs2017-ce-installer/vs2017-ce-installer-04.png)
7. [「サインイン」画面が表示されるが、「後で行う」を選択して問題ない。](./vs2017-ce-installer/vs2017-ce-installer-05-skip-sign-in.png)
8. [開発設定は「全般」を選択して、そのまま「Visual Studio の開始」をクリックする。](./vs2017-ce-installer/vs2017-ce-installer-06.png)

インストール場所：（選択したコンポーネントによって、変わる可能性あり）

```
# なぜか Azure 関連が入ってた。
C:\Program Files\Microsoft SDKs

# なんか入ってた。ローカル開発用？
C:\Program Files\Microsoft SQL Server

# なぜか x86 側に入ってるが、.NET 関係やNuGet, TypeScript などで VC 系は入ってない。
C:\Program Files (x86)\Microsoft SDKs

# こちらには開発用のライブラリが入ってる。
C:\Program Files (x86)\Microsoft SQL Server

# これが本命。なぜ x86 に・・・。
C:\Program Files (x86)\Microsoft Visual Studio\2017\Community

# Cコンパイラ・リンカ・dumpbin などはこちら。
C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\bin
```

### コマンドプロンプトからの実行

スタートメニュー -> "Visual Studio 2017" -> "VS 2017 用 x64 Native Tools コマンドプロンプト" を開くと、コンパイラやリンカにPATHが通ったコマンドプロンプトが開く。

## Windows Driver Kit (WDK) バンドル経由で WinDbg をインストール

VS2017 をインストールすれば、そのまま Windows Driver Kit (WDK) をインストールすることで、WinDbgも一緒にインストールできる。

1. https://docs.microsoft.com/ja-jp/windows-hardware/drivers/download-the-wdk にアクセスし、STEP2 の "Download WDK for Windows 10, version 1709" リンクをクリックしてインストーラ (`wdksetup.exe`) をDLし、実行する。
   - [URL画像](./wdksetup/wdksetup-00.png) 
2. ["Install the Windows Driver Kit - ..." を選択して "Next"](./wdksetup/wdksetup-01.png) 
3. [利用データの送信はお任せで "Next"](./wdksetup/wdksetup-02.png) 
4. [インストール中...](./wdksetup/wdksetup-03.png) 
5. ["Install Windows Driver Kit Visual Studio extension" を選択して "Close"](./wdksetup/wdksetup-04.png) 
6. [VSIX インストーラが起動し、WDK用の拡張をインストールします。](./wdksetup/wdksetup-05.png) 
7. [拡張のインストールが終わったら「閉じる」](./wdksetup/wdksetup-06.png) 
8. [インストールが完了すると、スタートメニューのアプリケーション一覧 -> "Windows Kits" の下に WinDbg アイコンが追加されます。](./wdksetup/wdksetup-07.png) 

WinDbgのインストール先：

```
C:\Program Files (x86)\Windows Kits\10\Debuggers\x64
C:\Program Files (x86)\Windows Kits\10\Debuggers\x86
```

チュートリアル : 
- https://docs.microsoft.com/ja-jp/windows-hardware/drivers/debugger/getting-started-with-windbg

## スタンドアローンで WinDbg をインストール

VS2017もインストールせず、WDKのインストーラから WinDbg のみを単独(スタンドアローン)でインストールする方法。

1. https://docs.microsoft.com/ja-jp/windows-hardware/drivers/debugger/ にアクセスし、 "3 ways to get Debugging Tools for Windows" の "As a standalone tool set" の指示に従い "install Windows SDK" リンクをクリック
   - [URL画像](./install-windbg-standalone/install-windbg-standalone-00a.png)
2. https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk が表示されるので、そのまま "Download the .EXE" リンクをクリックして `winsdksetup.exe` をダウンロード。
   - [URL画像](./install-windbg-standalone/install-windbg-standalone-00b.png)
3. `winsdksetup.exe` を実行するとインストーラが起動する。 ["Install the Windows Software Development Kit - ..." を選択して "Next" クリック](./install-windbg-standalone/install-windbg-standalone-01.png)
4. ["Windows Kits Privacy" については適切に選択して "Next" クリック](./install-windbg-standalone/install-windbg-standalone-02.png)
5. ["License Agreement" で "Accept" クリック](./install-windbg-standalone/install-windbg-standalone-03.png)
6. [インストールするコンポーネントで "Debugging Tools for Windows" にだけチェックを入れて、 "Install" クリック](./install-windbg-standalone/install-windbg-standalone-04.png)
7. [インストール中...](./install-windbg-standalone/install-windbg-standalone-05.png)
8. [インストール完了。"Close"クリック](./install-windbg-standalone/install-windbg-standalone-06.png)

インストール先はWDK経由でインストールした時と同じです。
