---
layout: contents
language: ja
title: Developer
short_desc: K2HDKC Node.js - K2HDKC addon library for Node.js
lang_opp_file: developer.html
lang_opp_word: To English
prev_url: buildja.html
prev_string: Build
top_url: indexja.html
top_string: TOP
next_url: environmentsja.html
next_string: Environments
---

<!-- -----------------------------------------------------------　-->
# 開発者向け

## [共通](#COMMON)
[同期と非同期](#ASYNCHRONOUS)  
[提供されるクラス](#ABOUTCLASS)  
[永続とワンタイム接続](#PERMANENT-ONETIME)  

## [k2hdkcクラス](#K2HDKCCLASS)
[k2hdkc::Init()](#K2HDKC-INIT)  
[k2hdkc::Clean()](#K2HDKC-CLEAN)  
[k2hdkc::IsPermanent()](#K2HDKC-ISPERMANENT)  
[k2hdkc::GetValue()](#K2HDKC-GETVALUE)  
[k2hdkc::GetSubkeys()](#K2HDKC-GETSUBKEYS)  
[k2hdkc::GetAttrs()](#K2HDKC-GETSUBKEYS)  
[k2hdkc::SetValue()](#K2HDKC-SETVALUE)  
[k2hdkc::SetSubkeys()](#K2HDKC-SETSUBKEYS)  
[k2hdkc::SetAll()](#K2HDKC-SETALL)  
[k2hdkc::Remove()](#K2HDKC-REMOVE)  
[k2hdkc::Rename()](#K2HDKC-RENAME)  
[k2hdkc::QueuePush()](#K2HDKC-QUEUEPUSH)  
[k2hdkc::QueuePop()](#K2HDKC-QUEUEPOP)  
[k2hdkc::QueueRemove()](#K2HDKC-QUEUEREMOVE)  
[k2hdkc::CasInit()](#K2HDKC-CASINIT)  
[k2hdkc::CasGet()](#K2HDKC-CASGET)  
[k2hdkc::CasSet()](#K2HDKC-CASSET)  
[k2hdkc::CasIncDec()](#K2HDKC-CASINCDEC)  
[k2hdkc::PrintVersion()](#K2HDKC-PRINTVERSION)  

<!-- -----------------------------------------------------------　-->
***

## <a name="COMMON"> 共通
### <a name="ASYNCHRONOUS"> 同期と非同期
K2HDKC Node.js アドオンライブラリのクラスのメソッドは、同期処理とCallback関数を指定できる非同期処理をサポートしています。  
非同期処理をサポートしているメソッドは、callback関数の引数を受け取ることができます。  
また、非同期処理として**on**や**onXXXXX**でイベントハンドラーを指定することもできます。  
callback関数もしくはイベントハンドラーの指定をすることで非同期処理を行うことができます。  

callback関数の引数を指定しない場合やイベントハンドラーの指定を指定しない場合、各々のメソッドは同期処理として動作します。

### <a name="ABOUTCLASS"> 提供されるクラス
K2HDKC Node.js アドオンライブラリは、**k2hdkc**クラスを提供します。  
このクラスを使い[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)への接続とデータ操作ができます。

### <a name="PERMANENT-ONETIME"> 永続とワンタイム接続
K2HDKC Node.js アドオンライブラリを利用して[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)への接続する方法は、永続的な接続とデータ操作毎の接続（ワンタイム接続）の2つの方法が提供されています。

#### 永続接続
K2HDKC Node.js アドオンライブラリのメインクラス（**k2hdkc**）オブジェクトを初期化するとき、[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)への接続を行います。  
初期化以降の[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)への操作はすべてこの接続を利用します。  
永続接続の方法により、[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)のデータ操作毎の接続オーバーヘッドを低減できます。

#### ワンタイム接続（データ操作毎の接続）
K2HDKC Node.js アドオンライブラリのメインクラス（**k2hdkc**）オブジェクトを初期化するときに[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)へ接続せず、データ操作ごとに毎回接続処理します。  
データ操作ごとに毎回接続処理を行うため、メインクラス（**k2hdkc**）オブジェクトの使いまわしが可能です。  
ワンタイム接続は、[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)のデータ操作毎に接続オーバーヘッドが発生しますが、ひとつの操作処理中に別の操作を行うことができます。

#### 同時利用
永続接続とワンタイム接続の区別は、ひとつのメインクラス（**k2hdkc**）オブジェクトに対してのみであり、永続接続用とワンタイム接続用のそれぞれにオブジェクトを準備することで、永続接続とワンタイム接続を同時に利用することができます。

<!-- -----------------------------------------------------------　-->
***

## <a name="K2HDKCCLASS"> k2hdkcクラス
K2HDKC Node.js アドオンライブラリの提供するメインクラスです。  
このクラスにより、K2HDKCとの接続、データ操作を行うことができます。

クラスを生成するには以下のように指定します。
- 永続接続  
  ```
  var dkcobj = new require('k2hdkc')('slave.ini', 8031, null, true, false);
  ```
- ワンタイム接続  
  ```
  var dkcobj = new require('k2hdkc')();
  ```

以下に、K2HDKCクラスのメソッドとその説明をします。

### <a name="K2HDKC-INIT"> k2hdkc::Init()
k2hdkcオブジェクトを初期化します。  
このメソッドは、k2hdkcオブジェクトを永続接続用、ワンタイム接続用のいずれかに初期化します。

#### 書式
```
bool Init(String   conf = null,
          int      port = -1,
          String   cuk  = null,
          bool     auto_rejoin = false,
          bool     no_giveup_rejoin = false,
          Callback cbfunc = null
)
```

#### 引数
- conf  
  永続接続用に初期化する場合には、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。  
  ワンタイム接続用に初期化する場合は、nullもしくは未指定とします。
- port  
  永続接続用に初期化する場合には、CHMPXスレーブノードの制御ポート番号を指定します。  
  ワンタイム接続用に初期化する場合は、-1もしくは未指定とします。
- cuk  
  永続接続用に初期化する場合には、CHMPXスレーブノードのCUK文字列を指定します。  
  ワンタイム接続用に初期化する場合は、nullもしくは未指定とします。
- auto_rejoin  
  ワンタイム接続用に初期化する場合は、falseもしくは未指定とします。  
  永続接続用に初期化する場合には、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化する場合は、falseもしくは未指定とします。  
  永続接続用に初期化する場合には、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false, function(error){
      if(null !== error){
          console_log('failed initializing');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init(function(error){
      if(null !== error){
          console_log('failed initializing');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('init', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.on('init', function(error){
      if(null !== error){
          console_log('failed initializing');
      }
      dkcobj.clean();
  });
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  ```
- on('', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.on('init', function(error){
      if(null !== error){
          console_log('failed initializing');
      }
      dkcobj.clean();
  });
  
  dkcobj.init();
  ```
- onInit(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.onInit(function(error){
      if(null !== error){
          console_log('failed initializing');
      }
      dkcobj.clean();
  });
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  ```
- onInit(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.onInit(function(error){
      if(null !== error){
          console_log('failed initializing');
      }
      dkcobj.clean();
  });
  
  dkcobj.init();
  ```

### <a name="K2HDKC-CLEAN"> k2hdkc::Clean()
k2hdkcオブジェクトの終了処理を行い、オブジェクトを破棄します。

#### 書式
```
bool Clean(void)
```

#### 引数
なし

#### 返り値
常に**true**を返します。

#### サンプル
- 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.clean();
  ```
- ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.clean();
  ```

### <a name="K2HDKC-ISPERMANENT"> k2hdkc::IsPermanent()
k2hdkcオブジェクトが永続接続として初期化されているか否かを確認します。

#### 書式
```
bool IsPermanent(void)
```

#### 引数
なし

#### 返り値
k2hdkcオブジェクトが永続接続として初期化されている場合は、**true**を返します。それ以外は**false**を返します。

#### サンプル
- 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.IsPermanent();
  
  dkcobj.clean();
  ```
- ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.IsPermanent();
  
  dkcobj.clean();
  ```

### <a name="K2HDKC-GETVALUE"> k2hdkc::GetValue()
K2HDKCからキーおよびサブキーを指定して値を取得します。

#### 書式
- 永続接続  
  ```
  [bool/String] GetValue(String   key,
                         String   subkey = "",
                         bool     attrcheck = true,
                         String   pass = "",
                         Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  [bool/String] GetValue(String   conf,
                         int      port,
                         String   cuk,
                         bool     auto_rejoin,
                         bool     no_giveup_rejoin,
                         String   key,
                         String   subkey = "",
                         bool     attrcheck = true,
                         String   pass = "",
                         Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  読み出すキーを指定します。  
  サブキーが指定されている場合には、この値はサブキーの親キーを示します。
- subkey  
  サブキーの値を読み出す場合に指定します。  
  親キー以下にサブキーが存在しない場合にはエラーとなります。
- attrcheck  
  K2HDKCの組み込み属性のチェックを行う場合、trueに設定してください。  
  組み込み属性とは、暗号化、履歴、有効期限のことです。
- pass  
  キーが暗号化された値を持つ場合には、復号するためのパスフレーズを指定してください。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, String value)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
同期処理の場合、正常終了したときには読み出した値を返し、エラーの場合は、**null**が返されます。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var parentval = dkcobj.GetValue('key', null, true, null);
  var subkeyval = dkcobj.GetValue('key', 'subkey', true, null);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var parentval = dkcobj.GetValue('slave.ini', 8031, null, true, false, 'key', null, true, null);
  var subkeyval = dkcobj.GetValue('slave.ini', 8031, null, true, false, 'key', 'subkey', true, null);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.GetValue('key', null, true, null, function(error, value){
      if(null !== error){
          console_log('failed getting value');
          dkcobj.clean();
      }else{
          dkcobj.GetValue('key', 'subkey', true, null, function(error, value){
              if(null !== error){
                  console_log('failed getting value');
              }
              dkcobj.clean();
          });
     }
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.GetValue('slave.ini', 8031, null, true, false, 'key', null, true, null, function(error, value){
      if(null !== error){
          console_log('failed getting value');
          dkcobj.clean();
      }else{
          dkcobj.GetValue('slave.ini', 8031, null, true, false, 'key', 'subkey', true, null, function(error, value){
              if(null !== error){
                  console_log('failed getting value');
              }
              dkcobj.clean();
          });
     }
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('get', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('get', function(error, value){
      if(null !== error){
          console_log('failed getting value');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetValue('key', 'subkey', true, null);
  ```
- on('get', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('get', function(error, value){
      if(null !== error){
          console_log('failed getting value');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetValue('slave.ini', 8031, null, true, false, 'key', 'subkey', true, null);
  ```
- onGet(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onGet(function(error, value){
      if(null !== error){
          console_log('failed getting value');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetValue('key', 'subkey', true, null);
  ```
- onGet(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onGet(function(error, value){
      if(null !== error){
          console_log('failed getting value');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetValue('slave.ini', 8031, null, true, false, 'key', 'subkey', true, null);
  ```

#### 注意
本メソッドはキーとサブキーを同時に指定することができます。  
通常、キーの値の読み出しであれば、キー名を指定するだけで読み出すことができます。  
キーとサブキーを同時に指定するという意味は、サブキーがキーに紐付けられていることが条件となります。  
もし、サブキーがキーに紐づいていない場合にはこのメソッドは値を読み出さず、エラーを返します。

### <a name="K2HDKC-GETSUBKEYS"> k2hdkc::GetSubkeys()
K2HDKCからキーに紐付けられたサブキーのリストを取得します。

#### 書式
- 永続接続  
  ```
  [bool/Array] GetSubkeys(String   key,
                          bool     attrcheck = true,
                          Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  [bool/Array] GetSubkeys(String   conf,
                          int      port,
                          String   cuk,
                          bool     auto_rejoin,
                          bool     no_giveup_rejoin,
                          String   key,
                          bool     attrcheck = true,
                          Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  サブキーのリストを読み出すキーを指定します。
- attrcheck  
  K2HDKCの組み込み属性のチェックを行う場合、trueに設定してください。  
  組み込み属性とは、暗号化、履歴、有効期限のことです。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, Array subkeys)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
同期処理の場合、正常終了したときには読み出したサブキーのリストを**Array**で返し、エラーの場合は、**null**が返されます。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var subkeys = dkcobj.GetSubkeys('key', true);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var subkeys = dkcobj.GetSubkeys('slave.ini', 8031, null, true, false, 'key', true);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.GetSubkeys('key', true, function(error, subkeys){
      if(null !== error){
          console_log('failed getting subkey list');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.GetSubkeys('slave.ini', 8031, null, true, false, 'key', true, function(error, subkeys){
      if(null !== error){
          console_log('failed getting subkey list');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('getSubkeys', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('getSubkeys', function(error, subkeys){
      if(null !== error){
          console_log('failed getting subkey list');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetSubkeys('key', true);
  ```
- on('getSubkeys', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('getSubkeys', function(error, subkeys){
      if(null !== error){
          console_log('failed getting subkey list');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetSubkeys('slave.ini', 8031, null, true, false, 'key', true);
  ```
- onGetSubkeys(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onGetSubkeys(function(error, subkeys){
      if(null !== error){
          console_log('failed getting subkey list');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetSubkeys('key', true);
  ```
- onGetSubkeys(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onGetSubkeys(function(error, subkeys){
      if(null !== error){
          console_log('failed getting subkey list');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetSubkeys('slave.ini', 8031, null, true, false, 'key', true);
  ```

### <a name="K2HDKC-GETSUBKEYS"> k2hdkc::GetAttrs()
K2HDKCからキーに設定されている属性名のリストを取得します。

#### 書式
- 永続接続  
  ```
  [bool/Array] GetAttrs(String   key,
                        Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  [bool/Array] GetAttrs(String   conf,
                        int      port,
                        String   cuk,
                        bool     auto_rejoin,
                        bool     no_giveup_rejoin,
                        String   key,
                        Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  属性のリストを読み出すキーを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, Array attrs)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
同期処理の場合、正常終了したときには読み出した属性のリストを**Array**で返し、エラーの場合は、**null**が返されます。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var attrs = dkcobj.GetAttrs('key');
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var attrs = dkcobj.GetAttrs('slave.ini', 8031, null, true, false, 'key');
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.GetAttrs('key', function(error, attrs){
      if(null !== error){
          console_log('failed getting attribute name list');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.GetAttrs('slave.ini', 8031, null, true, false, 'key', function(error, attrs){
      if(null !== error){
          console_log('failed getting attribute name list');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('getAttrs', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('getAttrs', function(error, attrs){
      if(null !== error){
          console_log('failed getting attribute name list');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetAttrs('key');
  ```
- on('getAttrs', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('getAttrs', function(error, attrs){
      if(null !== error){
          console_log('failed getting attribute name list');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetAttrs('slave.ini', 8031, null, true, false, 'key');
  ```
- onGetAttrs(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onGetAttrs(function(error, attrs){
      if(null !== error){
          console_log('failed getting attribute name list');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetAttrs('key');
  ```
- onGetAttrs(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onGetAttrs(function(error, attrs){
      if(null !== error){
          console_log('failed getting attribute name list');
      }
      dkcobj.clean();
  });
  
  dkcobj.GetAttrs('slave.ini', 8031, null, true, false, 'key');
  ```

### <a name="K2HDKC-SETVALUE"> k2hdkc::SetValue()
K2HDKCへキーおよびサブキーを指定して値を設定します。

#### 書式
- 永続接続  
  ```
  bool SetValue(String   key,
                String   value ="",
                String   subkey = "",
                String   pass = "",
                int      expire = 0,
                Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool SetValue(String   conf,
                int      port,
                String   cuk,
                bool     auto_rejoin,
                bool     no_giveup_rejoin,
                String   key,
                String   value ="",
                String   subkey = "",
                String   pass = "",
                int      expire = 0,
                Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  値を設定するキーを指定します。  
  サブキーが指定されている場合には、この値はサブキーの親キーを示します。
- value  
  設定する値を指定します。
- subkey  
  このキーが指定された場合には、この値はサブキー名を示し、サブキーは親キーに紐付けられ、値はサブキーに設定されます。
- pass  
  キー（値）を暗号化する場合にパスフレーズを指定します。
- expire  
  キー（値）に有効期限を設定する場合には、この値に有効期限秒を指定します。  
  0を指定した場合には、有効期限は設定されません。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.SetValue('key', 'val', null, null, 0);
  dkcobj.SetValue('key', 'val', 'subkey', null, 0);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.SetValue('slave.ini', 8031, null, true, false, 'key', 'val', null, null, 0);
  dkcobj.SetValue('slave.ini', 8031, null, true, false, 'key', 'val', 'subkey', null, 0);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.SetValue('key', 'val', null, null, 0, function(error){
      if(null !== error){
          console_log('failed setting value');
          dkcobj.clean();
      }else{
          dkcobj.SetValue('key', 'val', 'subkey', null, 0, function(error){
              if(null !== error){
                  console_log('failed setting value');
              }
              dkcobj.clean();
          });
     }
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.SetValue('slave.ini', 8031, null, true, false, 'key', 'val', null, null, 0, function(error){
      if(null !== error){
          console_log('failed setting value');
          dkcobj.clean();
      }else{
          dkcobj.SetValue('slave.ini', 8031, null, true, false, 'key', 'val', 'subkey', null, 0, function(error){
              if(null !== error){
                  console_log('failed setting value');
              }
              dkcobj.clean();
          });
     }
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('set', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('set', function(error){
      if(null !== error){
          console_log('failed setting value');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetValue('key', 'val', 'subkey', null, 0);
  ```
- on('set', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('set', function(error){
      if(null !== error){
          console_log('failed setting value');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetValue('slave.ini', 8031, null, true, false, 'key', 'val', 'subkey', null, 0);
  ```
- onSet(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onSet(function(error){
      if(null !== error){
          console_log('failed setting value');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetValue('key', 'val', 'subkey', null, 0);
  ```
- onSet(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onSet(function(error){
      if(null !== error){
          console_log('failed setting value');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetValue('slave.ini', 8031, null, true, false, 'key', 'val', 'subkey', null, 0);
  ```

### <a name="K2HDKC-SETSUBKEYS"> k2hdkc::SetSubkeys()
キーに紐づくサブキーのリストを設定（上書き）、クリアします。

#### 書式
- 永続接続  
  ```
  bool SetSubkeys(String   key,
                  Array    subkeys = null,
                  Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool SetSubkeys(String   conf,
                  int      port,
                  String   cuk,
                  bool     auto_rejoin,
                  bool     no_giveup_rejoin,
                  String   key,
                  Array    subkeys = null,
                  Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  サブキーを追加する親キーを指定します。
- subkeys  
  親キーのサブキーリストを上書きするサブキーリストの配列を指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.SetSubkeys('key', ['subkey1', 'subkey2']);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.SetSubkeys('slave.ini', 8031, null, true, false, 'key', ['subkey1', 'subkey2']);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.SetSubkeys('key', ['subkey1', 'subkey2'], function(error){
      if(null !== error){
          console_log('failed setting subkey list');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.SetSubkeys('slave.ini', 8031, null, true, false, 'key', ['subkey1', 'subkey2'], function(error){
      if(null !== error){
          console_log('failed setting subkey list');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('setSubkeys', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('setSubkeys', function(error){
      if(null !== error){
          console_log('failed setting subkey list');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetSubkeys('key', ['subkey1', 'subkey2']);
  ```
- on('setSubkeys', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('setSubkeys', function(error){
      if(null !== error){
          console_log('failed setting subkey list');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetSubkeys('slave.ini', 8031, null, true, false, 'key', ['subkey1', 'subkey2']);
  ```
- onSetSubkeys(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onSetSubkeys(function(error){
      if(null !== error){
          console_log('failed setting subkey list');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetSubkeys('key', ['subkey1', 'subkey2']);
  ```
- onSetSubkeys(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onSetSubkeys(function(error){
      if(null !== error){
          console_log('failed setting subkey list');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetSubkeys('slave.ini', 8031, null, true, false, 'key', ['subkey1', 'subkey2']);
  ```

### <a name="K2HDKC-SETALL"> k2hdkc::SetAll()
キーに対して、値、サブキーのリストを一括で設定します。

#### 書式
- 永続接続  
  ```
  bool SetAll(String   key,
              String   value ="",
              Array    subkeys = null,
              String   pass = "",
              int      expire = 0,
              Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool SetAll(String   conf,
              int      port,
              String   cuk,
              bool     auto_rejoin,
              bool     no_giveup_rejoin,
              String   key,
              String   value ="",
              Array    subkeys = null,
              String   pass = "",
              int      expire = 0,
              Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  値を設定するキーを指定します。  
- value  
  設定する値を指定します。
- subkeys  
  キーのサブキーリストを上書きするサブキーリストの配列を指定します。
- pass  
  キー（値）を暗号化する場合にパスフレーズを指定します。
- expire  
  キー（値）に有効期限を設定する場合には、この値に有効期限秒を指定します。  
  0を指定した場合には、有効期限は設定されません。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.SetAll('key', 'val', ['subkey1', 'subkey2'], null, 0);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.SetAll('slave.ini', 8031, null, true, false, 'key', 'val', ['subkey1', 'subkey2'], null, 0);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.SetAll('key', 'val', ['subkey1', 'subkey2'], null, 0, function(error){
      if(null !== error){
          console_log('failed setting all');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.SetAll('slave.ini', 8031, null, true, false, 'key', 'val', ['subkey1', 'subkey2'], null, 0, function(error){
      if(null !== error){
          console_log('failed setting all');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('setAll', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('setAll', function(error){
      if(null !== error){
          console_log('failed setting all');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetAll('key', 'val', ['subkey1', 'subkey2'], null, 0);
  ```
- on('setAll', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('setAll', function(error){
      if(null !== error){
          console_log('failed setting all');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetAll('slave.ini', 8031, null, true, false, 'key', 'val', ['subkey1', 'subkey2'], null, 0);
  ```
- onSetAll(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onSetAll(function(error){
      if(null !== error){
          console_log('failed setting all');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetAll('key', 'val', ['subkey1', 'subkey2'], null, 0);
  ```
- onSetAll(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onSetAll(function(error){
      if(null !== error){
          console_log('failed setting all');
      }
      dkcobj.clean();
  });
  
  dkcobj.SetAll('slave.ini', 8031, null, true, false, 'key', 'val', ['subkey1', 'subkey2'], null, 0);
  ```

### <a name="K2HDKC-REMOVE"> k2hdkc::Remove()
キーを削除します。

#### 書式
- 永続接続  
  ```
  bool Remove(String   key,
              bool     is_del_subkeys = false,
              Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool Remove(String   conf,
              int      port,
              String   cuk,
              bool     auto_rejoin,
              bool     no_giveup_rejoin,
              String   key,
              bool     is_del_subkeys = false,
              Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  値を設定するキーを指定します。  
- is_del_subkeys  
  キーに紐づくサブキーが存在する場合、そのサブキーも削除するか否かを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.Remove('key', true);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.Remove('slave.ini', 8031, null, true, false, 'key', true);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.Remove('key', true, function(error){
      if(null !== error){
          console_log('failed removing');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.Remove('slave.ini', 8031, null, true, false, 'key', true, function(error){
      if(null !== error){
          console_log('failed removing');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('remove', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('remove', function(error){
      if(null !== error){
          console_log('failed removing');
      }
      dkcobj.clean();
  });
  
  dkcobj.Remove('key', true);
  ```
- on('remove', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('remove', function(error){
      if(null !== error){
          console_log('failed removing');
      }
      dkcobj.clean();
  });
  
  dkcobj.Remove('slave.ini', 8031, null, true, false, 'key', true);
  ```
- onRemove(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onRemove(function(error){
      if(null !== error){
          console_log('failed removing');
      }
      dkcobj.clean();
  });
  
  dkcobj.Remove('key', true);
  ```
- onRemove(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onRemove(function(error){
      if(null !== error){
          console_log('failed removing');
      }
      dkcobj.clean();
  });
  
  dkcobj.Remove('slave.ini', 8031, null, true, false, 'key', true);
  ```

### <a name="K2HDKC-RENAME"> k2hdkc::Rename()
キー名を変更します。

#### 書式
- 永続接続  
  ```
  bool Rename(String   oldkey,
              String   newkey,
              String   parentkey = "",
              bool     attrcheck = true,
              String   pass = "",
              int      expire = 0,
              Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool Rename(String   conf,
              int      port,
              String   cuk,
              bool     auto_rejoin,
              bool     no_giveup_rejoin,
              String   oldkey,
              String   newkey,
              String   parentkey = "",
              bool     attrcheck = true,
              String   pass = "",
              int      expire = 0,
              Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- oldkey  
  変更前のキー名を指定します。
- newkey  
  変更後のキー名を指定します。
- parentkey  
  変更するキーの親キーが存在する場合、この親キー名を指定すると親キーのサブキーリストの中のキー名も変更します。
- attrcheck  
  K2HDKCの組み込み属性のチェックを行う場合、trueに設定してください。  
  組み込み属性とは、暗号化、履歴、有効期限のことです。
- pass  
  変更後のキー（値）を暗号化する場合にパスフレーズを指定します。
- expire  
  変更後のキー（値）に有効期限を設定する場合には、この値に有効期限秒を指定します。  
  0を指定した場合には、有効期限は設定されません。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.Rename('oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.Rename('slave.ini', 8031, null, true, false, 'oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.Rename('oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000, function(error){
      if(null !== error){
          console_log('failed renaming');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.Rename('slave.ini', 8031, null, true, false, 'oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000, function(error){
      if(null !== error){
          console_log('failed renaming');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('rename', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('rename', function(error){
      if(null !== error){
          console_log('failed renaming');
      }
      dkcobj.clean();
  });
  
  dkcobj.Rename('oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000);
  ```
- on('rename', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('rename', function(error){
      if(null !== error){
          console_log('failed renaming');
      }
      dkcobj.clean();
  });
  
  dkcobj.Rename('slave.ini', 8031, null, true, false, 'oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000);
  ```
- onRename(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onRename(function(error){
      if(null !== error){
          console_log('failed renaming');
      }
      dkcobj.clean();
  });
  
  dkcobj.Rename('oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000);
  ```
- onRename(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onRename(function(error){
      if(null !== error){
          console_log('failed renaming');
      }
      dkcobj.clean();
  });
  
  dkcobj.Rename('slave.ini', 8031, null, true, false, 'oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000);
  ```

### <a name="K2HDKC-QUEUEPUSH"> k2hdkc::QueuePush()
キューにデータをプッシュします。

#### 書式
- 永続接続  
  ```
  bool QueuePush(String   prefix,
                 String   key = "",,
                 String   val,
                 bool     is_fifo = true,
                 bool     attrcheck = true,
                 String   pass = "",
                 int      expire = 0,
                 Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool QueuePush(String   conf,
                 int      port,
                 String   cuk,
                 bool     auto_rejoin,
                 bool     no_giveup_rejoin,
                 String   prefix,
                 String   key = "",
                 String   val,
                 bool     is_fifo = true,
                 bool     attrcheck = true,
                 String   pass = "",
                 int      expire = 0,
                 Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- prefix  
  キュー名のプレフィックスを指定します。
- key  
  対象のキューがKEYキュー（K2HDKCにはキューとKEYキューがあります）の場合、キューにプッシュするキー名を指定します。
- val  
  キューにプッシュする値を指定します。
- is_fifo  
  キューにプッシュする方向（FIFO、LIFO）を指定します。
- attrcheck  
  K2HDKCの組み込み属性のチェックを行う場合、trueに設定してください。  
  組み込み属性とは、暗号化、履歴、有効期限のことです。
- pass  
  プッシュする値を暗号化する場合にパスフレーズを指定します。
- expire  
  プッシュする値に有効期限を設定する場合には、この値に有効期限秒を指定します。  
  0を指定した場合には、有効期限は設定されません。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.QueuePush('queue_', null,'data', true, true, 'mypass', 1000);
  dkcobj.QueuePush('keyqueue_', 'key', 'data', false, true, null, 0);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.QueuePush('slave.ini', 8031, null, true, false, 'queue_', null,'data', true, true, 'mypass', 1000);
  dkcobj.QueuePush('slave.ini', 8031, null, true, false, 'keyqueue_', 'key', 'data', false, true, null, 0);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.QueuePush('queue_', null,'data', true, true, 'mypass', 1000, function(error){
      if(null !== error){
          console_log('failed queue pushing');
          dkcobj.clean();
      }else{
          dkcobj.QueuePush('keyqueue_', 'key', 'data', false, true, null, 0, function(error){
              if(null !== error){
                  console_log('failed queue pushing');
              }
              dkcobj.clean();
          });
      }
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.QueuePush('slave.ini', 8031, null, true, false, 'queue_', null,'data', true, true, 'mypass', 1000, function(error){
      if(null !== error){
          console_log('failed queue pushing');
          dkcobj.clean();
      }else{
          dkcobj.QueuePush('slave.ini', 8031, null, true, false, 'keyqueue_', 'key', 'data', false, true, null, 0, function(error){
              if(null !== error){
                  console_log('failed queue pushing');
              }
              dkcobj.clean();
          });
      }
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('queuePush', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('queuePush', function(error){
      if(null !== error){
          console_log('failed queue pushing');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueuePush('queue_', null,'data', true, true, 'mypass', 1000);
  ```
- on('queuePush', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('queuePush', function(error){
      if(null !== error){
          console_log('failed queue pushing');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueuePush('slave.ini', 8031, null, true, false, 'queue_', null,'data', true, true, 'mypass', 1000);
  ```
- onQueuePush(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onQueuePush(function(error){
      if(null !== error){
          console_log('failed queue pushing');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueuePush('queue_', null,'data', true, true, 'mypass', 1000);
  ```
- onQueuePush(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onQueuePush(function(error){
      if(null !== error){
          console_log('failed queue pushing');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueuePush('slave.ini', 8031, null, true, false, 'queue_', null,'data', true, true, 'mypass', 1000);
  ```

### <a name="K2HDKC-QUEUEPOP"> k2hdkc::QueuePop()
キューからデータをポップします。

#### 書式
- 永続接続  
  ```
  [bool/String/Array] QueuePop(String   prefix,
                               bool     is_fifo = true,
                               bool     is_key_queue = false,
                               String   pass = "",
                               Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  [bool/String/Array] QueuePop(String   conf,
                               int      port,
                               String   cuk,
                               bool     auto_rejoin,
                               bool     no_giveup_rejoin,
                               String   prefix,
                               bool     is_fifo = true,
                               bool     is_key_queue = false,
                               String   pass = "",
                               Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- prefix  
  キュー名のプレフィックスを指定します。
- is_fifo  
  キューにプッシュする方向（FIFO、LIFO）を指定します。
- is_key_queue  
  キューのポップのときに、対象のキューをKEYキューとして取り扱う場合にはtrueを設定します。
- pass  
  ポップする値を復号する場合にパスフレーズを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下のいずれかになります。  
  ```
  function(Error error, String key, String value)
  function(Error error, String value)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了時、キューであればポップされた値を返します。KEYキューの場合にはキー名と値をセットとした**Array**を返します。  
エラーが発生した場合には、**null**を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var value    = dkcobj.QueuePop('queue_', true, false, 'mypass');
  var valarray = dkcobj.QueuePop('keyqueue_', false, true, null);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var value    = dkcobj.QueuePop('slave.ini', 8031, null, true, false, 'queue_', true, false, 'mypass');
  var valarray = dkcobj.QueuePop('slave.ini', 8031, null, true, false, 'keyqueue_', false, true, null);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.QueuePop('queue_', true, false, 'mypass', function(error, value){
      if(null !== error){
          console_log('failed queue popping');
          dkcobj.clean();
      }else{
          dkcobj.QueuePop('keyqueue_', false, true, null, function(error, key, value){
              if(null !== error){
                  console_log('failed queue popping');
              }
              dkcobj.clean();
          });
      }
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.QueuePop('slave.ini', 8031, null, true, false, 'queue_', true, false, 'mypass', function(error, value){
      if(null !== error){
          console_log('failed queue popping');
          dkcobj.clean();
      }else{
          dkcobj.QueuePop('slave.ini', 8031, null, true, false, 'keyqueue_', false, true, null, function(error, key, value){
              if(null !== error){
                  console_log('failed queue popping');
              }
              dkcobj.clean();
          });
      }
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('queuePop', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('queuePop', function(error, key, value){
      if(null !== error){
          console_log('failed queue popping');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueuePop('keyqueue_', false, true, null);
  ```
- on('queuePop', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('queuePop', function(error, key, value){
      if(null !== error){
          console_log('failed queue popping');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueuePop('slave.ini', 8031, null, true, false, 'keyqueue_', false, true, null);
  ```
- onQueuePop(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onQueuePop(function(error, key, value){
      if(null !== error){
          console_log('failed queue popping');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueuePop('keyqueue_', false, true, null);
  ```
- onQueuePop(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onQueuePop(function(error, key, value){
      if(null !== error){
          console_log('failed queue popping');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueuePop('slave.ini', 8031, null, true, false, 'keyqueue_', false, true, null);
  ```

### <a name="K2HDKC-QUEUEREMOVE"> k2hdkc::QueueRemove()
キューからデータを削除します。

#### 書式
- 永続接続  
  ```
  bool QueueRemove(String   prefix,
                   int      count = 0,
                   bool     is_fifo = true,
                   bool     is_key_queue = false,
                   String   pass = "",
                   Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool QueueRemove(String   conf,
                   int      port,
                   String   cuk,
                   bool     auto_rejoin,
                   bool     no_giveup_rejoin,
                   String   prefix,
                   int      count = 0,
                   bool     is_fifo = true,
                   bool     is_key_queue = false,
                   String   pass = "",
                   Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- prefix  
  キュー名のプレフィックスを指定します。
- count  
  キューから削除するデータ数を指定します。
- is_fifo  
  削除するキューの方向（FIFO、LIFO）を指定します。
- is_key_queue  
  削除するキューをKEYキューとして取り扱う場合にはtrueを設定します。
- pass  
  削除するキューが暗号化されている場合にパスフレーズを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.QueueRemove('queue_', 1, true, false, 'mypass');
  dkcobj.QueueRemove('keyqueue_', 1, false, true, null);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.QueueRemove('slave.ini', 8031, null, true, false, 'queue_', 1, true, false, 'mypass');
  dkcobj.QueueRemove('slave.ini', 8031, null, true, false, 'keyqueue_', 1, false, true, null);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.QueueRemove('queue_', 1, true, false, 'mypass', function(error){
      if(null !== error){
          console_log('failed queue removing');
          dkcobj.clean();
      }else{
          dkcobj.QueueRemove('keyqueue_', 1, false, true, null, function(error){
              if(null !== error){
                  console_log('failed queue removing');
              }
              dkcobj.clean();
          });
      }
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.QueueRemove('slave.ini', 8031, null, true, false, 'queue_', 1, true, false, 'mypass', function(error){
      if(null !== error){
          console_log('failed queue removing');
          dkcobj.clean();
      }else{
          dkcobj.QueueRemove('slave.ini', 8031, null, true, false, 'keyqueue_', 1, false, true, null, function(error){
              if(null !== error){
                  console_log('failed queue removing');
              }
              dkcobj.clean();
          });
      }
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('queueRemove', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('queueRemove', function(error){
      if(null !== error){
          console_log('failed queue removing');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueueRemove('keyqueue_', 1, false, true, null);
  ```
- on('queueRemove', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('queueRemove', function(error){
      if(null !== error){
          console_log('failed queue removing');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueueRemove('slave.ini', 8031, null, true, false, 'keyqueue_', 1, false, true, null);
  ```
- onQueueRemove(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onQueueRemove(function(error){
      if(null !== error){
          console_log('failed queue removing');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueueRemove('keyqueue_', 1, false, true, null);
  ```
- onQueueRemove(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onQueueRemove(function(error){
      if(null !== error){
          console_log('failed queue removing');
      }
      dkcobj.clean();
  });
  
  dkcobj.QueueRemove('slave.ini', 8031, null, true, false, 'keyqueue_', 1, false, true, null);
  ```

### <a name="K2HDKC-CASINIT"> k2hdkc::CasInit()
CAS（Compare and Swap）としてキー、値を初期化します。

#### 書式
- 永続接続  
  ```
  bool CasInit(String   key,
               uint     value = 0,
               String   pass = "",
               int      expire = 0,
               Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool CasInit(String   conf,
               int      port,
               String   cuk,
               bool     auto_rejoin,
               bool     no_giveup_rejoin,
               String   key,
               uint     value = 0,
               String   pass = "",
               int      expire = 0,
               Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  CASとして初期化、設定するキー名を指定します。
- value  
  CASとして設定するキーの初期値を設定します。
- pass  
  CASとして設定するキーを暗号化する場合にパスフレーズを指定します。
- expire  
  CASとして設定するキーに有効期限を設定する場合には、この値に有効期限秒を指定します。  
  0を指定した場合には、有効期限は設定されません。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasInit('caskey', 1, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasInit('slave.ini', 8031, null, true, false, 'caskey', 1, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasInit('caskey', 1, 'mypass', 1000, function(error){
      if(null !== error){
          console_log('failed initializing CAS');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasInit('slave.ini', 8031, null, true, false, 'caskey', 1, 'mypass', 1000, function(error){
      if(null !== error){
          console_log('failed initializing CAS');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('casInit', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('casInit', function(error){
      if(null !== error){
          console_log('failed initializing CAS');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasInit('caskey', 1, 'mypass', 1000);
  ```
- on('casInit', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('casInit', function(error){
      if(null !== error){
          console_log('failed initializing CAS');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasInit('slave.ini', 8031, null, true, false, 'caskey', 1, 'mypass', 1000);
  ```
- onCasInit(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onCasInit(function(error){
      if(null !== error){
          console_log('failed initializing CAS');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasInit('caskey', 1, 'mypass', 1000);
  ```
- onCasInit(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onCasInit(function(error){
      if(null !== error){
          console_log('failed initializing CAS');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasInit('slave.ini', 8031, null, true, false, 'caskey', 1, 'mypass', 1000);
  ```

### <a name="K2HDKC-CASGET"> k2hdkc::CasGet()
CAS（Compare and Swap）のキーの値を取得します。

#### 書式
- 永続接続  
  ```
  int CasGet(String   key,
             String   pass = "",
             Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  int CasGet(String   conf,
             int      port,
             String   cuk,
             bool     auto_rejoin,
             bool     no_giveup_rejoin,
             String   key,
             String   pass = "",
             Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  CASのキー名を指定します。
- pass  
  CASとして指定したキーを復号するためのパスフレーズを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, int value)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了時には値（**int32**）を返します。キーが存在していないなどエラー発生時には**undefined**を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var value = dkcobj.CasGet('caskey', 'mypass');
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var value = dkcobj.CasGet('slave.ini', 8031, null, true, false, 'caskey', 'mypass');
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasGet('caskey', 'mypass', function(error, value){
      if(null !== error){
          console_log('failed getting CAS value');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasGet('slave.ini', 8031, null, true, false, 'caskey', 'mypass', function(error, value){
      if(null !== error){
          console_log('failed getting CAS value');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('casGet', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('casGet', function(error, value){
      if(null !== error){
          console_log('failed getting CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasGet('caskey', 'mypass');
  ```
- on('casGet', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('casGet', function(error, value){
      if(null !== error){
          console_log('failed getting CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasGet('slave.ini', 8031, null, true, false, 'caskey', 'mypass');
  ```
- onCasGet(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onCasGet(function(error, value){
      if(null !== error){
          console_log('failed getting CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasGet('caskey', 'mypass');
  ```
- onCasGet(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onCasGet(function(error, value){
      if(null !== error){
          console_log('failed getting CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasGet('slave.ini', 8031, null, true, false, 'caskey', 'mypass');
  ```

### <a name="K2HDKC-CASSET"> k2hdkc::CasSet()
CAS（Compare and Swap）のキーに値を設定します。

#### 書式
- 永続接続  
  ```
  bool CasSet(String   key,
              uint     oldval = 0,
              uint     newval = 0,
              String   pass = "",
              int      expire = 0,
              Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool CasSet(String   conf,
              int      port,
              String   cuk,
              bool     auto_rejoin,
              bool     no_giveup_rejoin,
              String   key,
              uint     oldval = 0,
              uint     newval = 0,
              String   pass = "",
              int      expire = 0,
              Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  CASとして設定されているキー名を指定します。
- oldval  
  設定するCASキーの元の値を指定します。
- newval  
  設定する新しい値を指定します。
- pass  
  CASとして設定するキーを暗号化する場合にパスフレーズを指定します。
- expire  
  CASとして設定するキーに有効期限を設定する場合には、この値に有効期限秒を指定します。  
  0を指定した場合には、有効期限は設定されません。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasSet('caskey', 1, 5, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasSet('slave.ini', 8031, null, true, false, 'caskey', 1, 5, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasSet('caskey', 1, 5, 'mypass', 1000, function(error){
      if(null !== error){
          console_log('failed setting CAS value');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasSet('slave.ini', 8031, null, true, false, 'caskey', 1, 5, 'mypass', 1000, function(error){
      if(null !== error){
          console_log('failed setting CAS value');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('casSet', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('casSet', function(error){
      if(null !== error){
          console_log('failed setting CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasSet('caskey', 1, 5, 'mypass', 1000);
  ```
- on('casSet', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('casSet', function(error){
      if(null !== error){
          console_log('failed setting CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasSet('slave.ini', 8031, null, true, false, 'caskey', 1, 5, 'mypass', 1000);
  ```
- onCasSet(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onCasSet(function(error){
      if(null !== error){
          console_log('failed setting CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasSet('caskey', 1, 5, 'mypass', 1000);
  ```
- onCasSet(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onCasSet(function(error){
      if(null !== error){
          console_log('failed setting CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasSet('slave.ini', 8031, null, true, false, 'caskey', 1, 5, 'mypass', 1000);
  ```

### <a name="K2HDKC-CASINCDEC"> k2hdkc::CasIncDec()
CAS（Compare and Swap）のキーの値をインクリメントもしくはデクリメントします。

#### 書式
- 永続接続  
  ```
  bool CasIncDec(String   key,
                 bool     is_increment = true,
                 String   pass = "",
                 int      expire = 0,
                 Callback cbfunc = null
  )
  ```
- ワンタイム接続  
  ```
  bool CasIncDec(String   conf,
                 int      port,
                 String   cuk,
                 bool     auto_rejoin,
                 bool     no_giveup_rejoin,
                 String   key,
                 bool     is_increment = true,
                 String   pass = "",
                 int      expire = 0,
                 Callback cbfunc = null
  )
  ```

#### 引数
- conf  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノード起動時に指定した設定ファイル（*.ini）を指定します。
- port  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードの制御ポート番号を指定します。
- cuk  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、CHMPXスレーブノードのCUK文字列を指定します。
- auto_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの接続が切断された場合、再接続を自動的に実行します。  
  falseの場合には、再接続は行われなくなり、再接続されるまでデータ操作などエラーとなります。
- no_giveup_rejoin  
  ワンタイム接続用に初期化されたk2hdkcオブジェクトの場合、boolean値を指定します。  
  この値がtrueの場合には、CHMPXスレーブノードとの再接続できるまで処理を実行します。  
  falseの場合には、CHMPXスレーブノードに設定されている再接続上限回数まで再接続を試行します。
- key  
  CASとして設定されているキー名を指定します。
- is_increment  
  CASの値をインクリメントする場合にtrueを指定します。
- pass  
  CASとして設定するキーが暗号化されている場合にパスフレーズを指定します。
- expire  
  CASとして設定するキーに有効期限を設定する場合には、この値に有効期限秒を指定します。  
  0を指定した場合には、有効期限は設定されません。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasIncDec('caskey', true, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- 同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasIncDec('slave.ini', 8031, null, true, false, 'caskey', true, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- 非同期 - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasIncDec('caskey', true, 'mypass', 1000, function(error){
      if(null !== error){
          console_log('failed increment/decrement CAS value');
      }
      dkcobj.clean();
  });
  ```
- 非同期 - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasIncDec('slave.ini', 8031, null, true, false, 'caskey', true, 'mypass', 1000, function(error){
      if(null !== error){
          console_log('failed increment/decrement CAS value');
      }
      dkcobj.clean();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2タイプ（4例）の方法で非同期処理を記述できます。
- on('casIncDec', Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.on('casIncDec', function(error){
      if(null !== error){
          console_log('failed increment/decrement CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasIncDec('caskey', true, 'mypass', 1000);
  ```
- on('casIncDec', Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.on('casIncDec', function(error){
      if(null !== error){
          console_log('failed increment/decrement CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasIncDec('slave.ini', 8031, null, true, false, 'caskey', true, 'mypass', 1000);
  ```
- onCasIncDec(Callback cbfunc) - 永続接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.onCasIncDec(function(error){
      if(null !== error){
          console_log('failed increment/decrement CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasIncDec('caskey', true, 'mypass', 1000);
  ```
- onCasIncDec(Callback cbfunc) - ワンタイム接続  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.onCasIncDec(function(error){
      if(null !== error){
          console_log('failed increment/decrement CAS value');
      }
      dkcobj.clean();
  });
  
  dkcobj.CasIncDec('slave.ini', 8031, null, true, false, 'caskey', true, 'mypass', 1000);
  ```

### <a name="K2HDKC-PRINTVERSION"> k2hdkc::PrintVersion()
K2HDKC Node.jsアドオンライブラリのバージョンを出力します。

#### 書式
```
bool PrintVersion(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。

#### サンプル
```
var k2hdkc = require('bindings')('k2hdkc');
var dkcobj = new k2hdkc();

dkcobj.PrintVersion();
```
