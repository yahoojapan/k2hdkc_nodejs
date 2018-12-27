---
layout: contents
language: ja
title: Usage
short_desc: K2HDKC Node.js - K2HDKC addon library for Node.js
lang_opp_file: usage.html
lang_opp_word: To English
prev_url: featureja.html
prev_string: Feature
top_url: indexja.html
top_string: TOP
next_url: buildja.html
next_string: Build
---

# 使い方
K2HDKC Node.jsをビルドした後で、動作確認をしてみます。

## 1. 実行環境の構築
まず、実行環境を構築します。  
構築は、パッケージから構築する方法と、k2hdkc nodejs addonライブラリをビルドして利用する方法があります。

### 1.1 パッケージを利用して環境を構築
パッケージを利用して実行環境を構築する方法を説明します。  
k2hdkc nodejs addonライブラリのパッケージは、[npm](https://www.npmjs.com/package/k2hdkc)にあります。  

このパッケージをインストールする前に、K2HDKCの開発者パッケージが必要となりますので、K2HDKCの[ドキュメント](https://k2hdkc.antpick.ax/indexja.html)を参照してください。  
K2HDKCをインストールする方法も、パッケージからのインストールと、K2HDKCを自身でビルドする方法がありますので、[使い方](https://k2hdkc.antpick.ax/usageja.html)もしくは[ビルド](https://k2hdkc.antpick.ax/buildja.html)のドキュメントを参照してください。  

事前にK2HDKCをインストールした後、k2hdkc nodejs addonライブラリのnpmパッケージは、以下のようにインストールします。  
npmコマンドの詳細については[こちら](https://docs.npmjs.com/misc/index.html#npm1)を参照してください。  
```
$ npm install k2hdkc
```
### 1.2 ビルドして環境を構築
ソースコードからk2hdkc nodejs addonライブラリをビルドすることができます。  
ソースコードのgithubリポジトリをcloneし、ビルドする方法は[こちら](https://nodejs.k2hdkc.antpick.ax/buildja.html)を参照してください。

## 2. K2HDKCサーバーノードプロセスとCHMPXプロセスの起動
K2HDKC Node.jsが使う[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)サーバーノードプロセスが必要となります。  
K2HDKCサーバーノードプロセスとクライアントプログラムの通信には、通信ミドルウエアである[CHMPX](https://chmpx.antpick.ax/indexja.html)のサーバーノード、スレーブノードも必要となります。  
これらのプロセスの起動方法についての詳細は、[K2HDKC 使い方](https://k2hdkc.antpick.ax/usageja.html)と[CHMPX 使い方](https://chmpx.antpick.ax/usageja.html)を参照してください。  

テストのために起動するこれらのプロセスは以下のようにして起動することができます。  
```
$ cd k2hdkc_nodejs
$ test/run_process_helper.sh start_all
```
**run_process_helper.sh**スクリプトは、テストで使用されているヘルパスクリプトであり、ローカルホスト上にCHMPXサーバーノード×2、K2HDKCプロセス×2、CHMPXスレーブノード×1を起動します。  
以下に示すようにプロセスの起動を確認できます。  
```
$ ps ax
3156 pts/1    Sl     0:00 chmpx -conf /home/guest/k2hdkc_nodejs/test/test_server.ini -ctlport 8021
3162 pts/1    Sl     0:00 chmpx -conf /home/guest/k2hdkc_nodejs/test/test_server.ini -ctlport 8023
3168 pts/1    Sl     0:00 k2hdkc -conf /home/guest/k2hdkc_nodejs/test/test_server.ini -ctlport 8021
3176 pts/1    Sl     0:00 k2hdkc -conf /home/guest/k2hdkc_nodejs/test/test_server.ini -ctlport 8023
3185 pts/1    Sl     0:00 chmpx -conf /home/guest/k2hdkc_nodejs/test/test_slave.ini
```
これらのプロセスを終了する場合は以下に示すようにします。  
```
$ test/run_process_helper.sh stop_all
```
これらの詳細については、`test`ディレクトリのrun_process_helper.shスクリプトと設定ファイル（*.ini）を確認してください。

## 3. サンプル
以下のようなファイルを作成し、ファイル作成、キーと値の書き込み、読み出しができるか確認してみてください。  
```
var k2hdkc = require('k2hdkc');                                 // import k2hdkc nodejs addon library
var	dkcobj = new k2hdkc('test_slave.ini', 8031, true, false);   // initialize for permanent connecting

dkcobj.setValue('key', 'value', 'subkey', null, 0);             // test for writing

dkcobj.getValue('subkey', null, true, null);                    // test for reading by subkey directly
dkcobj.getValue('key', 'subkey', true, null);                   // test for reading with key and subkey

dkcobj.clean();                                                 // cleanup
```

## 4. ロード・実行
基本として、JavaScript言語でライブラリを読み込むための`require`でK2HDKC Node.js アドオンライブラリを読み込みます。  
その後は、K2HDKC Node.js アドオンライブラリが提供するクラス・関数・メソッドを呼び出してください。  
具体的には、環境や言語（TypeScriptなど）に応じて利用してください。

## 4. その他のテスト  
K2HDKC Node.jsでは、[Mocha](https://github.com/mochajs/mocha)と[Chai](https://github.com/chaijs/chai)を使ったテストを実行できます。  
以下のように[npm](https://www.npmjs.com/get-npm)コマンドを使い、作成したK2HDKC Node.jsをテストすることができます。  
```
$ npm run test
```
