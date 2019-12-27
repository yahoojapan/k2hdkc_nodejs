---
layout: contents
language: en-us
title: Developer
short_desc: K2HDKC Node.js - K2HDKC addon library for Node.js
lang_opp_file: developerja.html
lang_opp_word: To Japanese
prev_url: build.html
prev_string: Build
top_url: index.html
top_string: TOP
next_url: environments.html
next_string: Environments
---

<!-- -----------------------------------------------------------　-->
# For developer

## [Common](#COMMON)
[Synchronous and Asynchronous](#ASYNCHRONOUS)  
[About Classes](#ABOUTCLASS)  
[Permanent and Onetime Connection](#PERMANENT-ONETIME)  

## [k2hdkc Class](#K2HDKCCLASS)
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

## <a name="COMMON"> Common
### <a name="ASYNCHRONOUS"> Synchronous and Asynchronous
The methods of the classes provided by K2HDKC Node.js addon library support synchronous and asynchronous processing by specifying the callback function.  
Methods that support asynchronous processing can accept the arguments of the callback function.  
In addition, these can specify event handlers as **on** or **onXXXXX** as asynchronous processing.  
Developers can perform asynchronous processing using callback functions or event handlers.  
Callback function arguments and event handlers are not specified, these methods act as synchronization processes.

### <a name="ABOUTCLASS"> About Classes
K2HDKC Node.js addon library provides **k2hdkc** class.  
This class provides connection and data manipulation to the [K2HDKC](https://k2hdkc.antpick.ax/).

### <a name="PERMANENT-ONETIME"> Permanent and Onetime Connection
To connect to [K2HDKC](https://k2hdkc.antpick.ax/) cluster using K2HDKC Node.js addon library, there are two methods of permanent connection and connection for each data operation(onetime connection).

#### Permanent Connection
When initializing main class **k2hdkc** object of K2HDKC Node.js addon library, establish connection to [K2HDKC](https://k2hdkc.antpick.ax/) cluster.  
All data manipulation of the [K2HDKC](https://k2hdkc.antpick.ax/) cluster using this object is executed through the connection set up at initialization.  
Permanent connection reduces the overhead for the connection to the [K2HDKC](https://k2hdkc.antpick.ax/) cluster per data operation.

#### Onetime Connection (for each data operation)
Onetime Connection method connects to the [K2HDKC](https://k2hdkc.antpick.ax/) cluster every operation of data and disconnects after completion of the processing.  
Therefore, when initializing main class **k2hdkc** object of K2HDKC Node.js addon library, it does not connect to the [K2HDKC](https://k2hdkc.antpick.ax/) cluster.  
Onetime Connection overhead occurs for each data operation of the [K2HDKC](https://k2hdkc.antpick.ax/) cluster.

#### Using Permanent and Onetime connection at same time
The setting of Permanent Connection and Onetime connection is each **k2hdkc** object and has no influence on each other.  
Therefore, you can use these at the same time by creating **k2hdkc** objects for persistent connection and one time connection.

<!-- -----------------------------------------------------------　-->
***

## <a name="K2HDKCCLASS"> k2hdkc Class
K2HDKC Node.js addon library provides this **k2hdkc** class. This class is the only class of K2HDKC Node.js addon library.  
You can use this class to connect to [K2HDKC](https://k2hdkc.antpick.ax/) cluster and manipulate data int it.

A sample to create this class object is shown below.
- Permanent Connection  
  ```
  var dkcobj = new require('k2hdkc')('slave.ini', 8031, null, true, false);
  ```
- Onetime Connection  
  ```
  var dkcobj = new require('k2hdkc')();
  ```

Below are the methods of the **k2hdkc** class and those explanation.

### <a name="K2HDKC-INIT"> k2hdkc::Init()
This method initializes the **k2hdkc** object for Permanent Connection or Onetime Connection.

#### Format
```
bool Init(String   conf = null,
          int      port = -1,
          String   cuk  = null,
          bool     auto_rejoin = false,
          bool     no_giveup_rejoin = false,
          Callback cbfunc = null
)
```

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.  
  To initialize for Onetime Connection, null or unspecified.
- port  
  To initialize for Permanent Connection, specify the setting control port number for CHMPX slave node.  
  To initialize for Onetime Connection, -1 or unspecified.
- cuk  
  To initialize for Permanent Connection, specify the setting CUK string for CHMPX slave node.  
  To initialize for Onetime Connection, null or unspecified.
- auto_rejoin  
  To initialize for Permanent Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.  
  To initialize for Onetime Connection, false or unspecified.
- no_giveup_rejoin  
  To initialize for Permanent Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.  
  To initialize for Onetime Connection, false or unspecified.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('init', Callback cbfunc) - Permanent Connection  
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
- on('', Callback cbfunc) - Onetime Connection  
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
- onInit(Callback cbfunc) - Permanent Connection  
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
- onInit(Callback cbfunc) - Onetime Connection  
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
This method performs the termination processing of k2hdkc object and destroys the object.

#### Format
```
bool Clean(void)
```

#### Arguments
n/a

#### Return Values
Always return **true**.

#### Examples
- Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.clean();
  ```
- Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.clean();
  ```

### <a name="K2HDKC-ISPERMANENT"> k2hdkc::IsPermanent()
This methods checks whether the k2hdkc object is initialized as a Permanent Connection.

#### Format
```
bool IsPermanent(void)
```

#### Arguments
n/a

#### Return Values
If the k2hdkc object is initialized as a Permanent Connection, it returns **true**. Otherwise it returns **false**.

#### Examples
- Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.IsPermanent();
  
  dkcobj.clean();
  ```
- Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.IsPermanent();
  
  dkcobj.clean();
  ```

### <a name="K2HDKC-GETVALUE"> k2hdkc::GetValue()
This method gets the key and subkey values from the K2HDKC cluster.

#### Format
- Permanent Connection  
  ```
  [bool/String] GetValue(String   key,
                         String   subkey = "",
                         bool     attrcheck = true,
                         String   pass = "",
                         Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Onetime Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.  
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.  
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name.  
  If a subkey is specified, this value indicates the parent key of the subkey.
- subkey  
  To get the value of subkey, specify this value.
  An error occurs if the subkey does not exist under the parent key.
- attrcheck  
  Set to true to check the built-in attributes(encryption, history, expiration date) of K2HDKC.
- pass  
  If the key has an encrypted value, please specify passphrase to decrypt.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error, String value)
  ```
  If an error occurs, error is not null.

#### Return Values
In the case of synchronous processing, it returns the read value when it completes normally, and **null** if it is an error.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var parentval = dkcobj.GetValue('key', null, true, null);
  var subkeyval = dkcobj.GetValue('key', 'subkey', true, null);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var parentval = dkcobj.GetValue('slave.ini', 8031, null, true, false, 'key', null, true, null);
  var subkeyval = dkcobj.GetValue('slave.ini', 8031, null, true, false, 'key', 'subkey', true, null);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('get', Callback cbfunc) - Permanent Connection  
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
- on('get', Callback cbfunc) - Onetime Connection  
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
- onGet(Callback cbfunc) - Permanent Connection  
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
- onGet(Callback cbfunc) - Onetime Connection  
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

#### Notes
This method can specify keys and subkeys at the same time.  
Normally, if you read the value of the key, you can read it by just specifying the key name.  
The meaning of specifying the key and subkey at the same time is that the subkey must be associated with the key.  
If the subkey is not tied to a key, this method does not read the value and returns an error.

### <a name="K2HDKC-GETSUBKEYS"> k2hdkc::GetSubkeys()
This method gets the list of subkeys tied to the key.

#### Format
- Permanent Connection  
  ```
  [bool/Array] GetSubkeys(String   key,
                          bool     attrcheck = true,
                          Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name.
- attrcheck  
  Set to true to check the built-in attributes(encryption, history, expiration date) of K2HDKC.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error, Array subkeys)
  ```
  If an error occurs, error is not null.

#### Return Values
In the case of synchronous processing, when it completes normally, **Array** is returned as the list of subkeys, and **null** is returned in case of error.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var subkeys = dkcobj.GetSubkeys('key', true);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var subkeys = dkcobj.GetSubkeys('slave.ini', 8031, null, true, false, 'key', true);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('getSubkeys', Callback cbfunc) - Permanent Connection  
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
- on('getSubkeys', Callback cbfunc) - Onetime Connection  
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
- onGetSubkeys(Callback cbfunc) - Permanent Connection  
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
- onGetSubkeys(Callback cbfunc) - Onetime Connection  
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
This method gets a list of attribute names set in the key.

#### Format
- Permanent Connection  
  ```
  [bool/Array] GetAttrs(String   key,
                        Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error, Array attrs)
  ```
  If an error occurs, error is not null.

#### Return Values
In the case of synchronous processing, when it completes normally, **Array** is returned as the list of read attribute names, **null** is returned in case of error.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var attrs = dkcobj.GetAttrs('key');
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var attrs = dkcobj.GetAttrs('slave.ini', 8031, null, true, false, 'key');
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('getAttrs', Callback cbfunc) - Permanent Connection  
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
- on('getAttrs', Callback cbfunc) - Onetime Connection  
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
- onGetAttrs(Callback cbfunc) - Permanent Connection  
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
- onGetAttrs(Callback cbfunc) - Onetime Connection  
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
This method sets values for keys or subkeys under key.

#### Format
- Permanent Connection  
  ```
  bool SetValue(String   key,
                String   value ="",
                String   subkey = "",
                String   pass = "",
                int      expire = 0,
                Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name.  
  If a subkey is specified, this value indicates the parent key of the subkey.
- value  
  Specify the value.
- subkey  
  If this argument is specified, it indicates the subkey name.  
  This subkey is tied to the key and the value is set to the subkey.
- pass  
  Specify passphrase when encrypting key(value).
- expire  
  To set the expiration date for the key, specify the expiration time second for this value.  
  If 0 is specified, the expiration date is not set.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.SetValue('key', 'val', null, null, 0);
  dkcobj.SetValue('key', 'val', 'subkey', null, 0);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.SetValue('slave.ini', 8031, null, true, false, 'key', 'val', null, null, 0);
  dkcobj.SetValue('slave.ini', 8031, null, true, false, 'key', 'val', 'subkey', null, 0);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('set', Callback cbfunc) - Permanent Connection  
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
- on('set', Callback cbfunc) - Onetime Connection  
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
- onSet(Callback cbfunc) - Permanent Connection  
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
- onSet(Callback cbfunc) - Onetime Connection  
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
This method sets(overwrites) or clears the list of subkeys associated with the key.

#### Format
- Permanent Connection  
  ```
  bool SetSubkeys(String   key,
                  Array    subkeys = null,
                  Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name.
- subkeys  
  Specify the subkey list to be overwritten with **Array**.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.SetSubkeys('key', ['subkey1', 'subkey2']);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.SetSubkeys('slave.ini', 8031, null, true, false, 'key', ['subkey1', 'subkey2']);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('setSubkeys', Callback cbfunc) - Permanent Connection  
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
- on('setSubkeys', Callback cbfunc) - Onetime Connection  
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
- onSetSubkeys(Callback cbfunc) - Permanent Connection  
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
- onSetSubkeys(Callback cbfunc) - Onetime Connection  
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
This method sets the key value and subkey list at once.

#### Format
- Permanent Connection  
  ```
  bool SetAll(String   key,
              String   value ="",
              Array    subkeys = null,
              String   pass = "",
              int      expire = 0,
              Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name.
- value  
  Specify the value.
- subkeys  
  Specify an array of subkey lists to be overwritten.
- pass  
  Specify passphrase when encrypting key(value).
- expire  
  To set the expiration date for the key, specify the expiration time second for this value.  
  If 0 is specified, the expiration date is not set.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.SetAll('key', 'val', ['subkey1', 'subkey2'], null, 0);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.SetAll('slave.ini', 8031, null, true, false, 'key', 'val', ['subkey1', 'subkey2'], null, 0);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('setAll', Callback cbfunc) - Permanent Connection  
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
- on('setAll', Callback cbfunc) - Onetime Connection  
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
- onSetAll(Callback cbfunc) - Permanent Connection  
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
- onSetAll(Callback cbfunc) - Onetime Connection  
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
This method removes the key.

#### Format
- Permanent Connection  
  ```
  bool Remove(String   key,
              bool     is_del_subkeys = false,
              Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name.
- is_del_subkeys  
  If the key has a subkey, it specifies whether or not to remove that subkey.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.Remove('key', true);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.Remove('slave.ini', 8031, null, true, false, 'key', true);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('remove', Callback cbfunc) - Permanent Connection  
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
- on('remove', Callback cbfunc) - Onetime Connection  
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
- onRemove(Callback cbfunc) - Permanent Connection  
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
- onRemove(Callback cbfunc) - Onetime Connection  
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
This method changes the key name.

#### Format
- Permanent Connection  
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
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- oldkey  
  Specify the key name before change.
- newkey  
  Specify the key name after change.
- parentkey  
  If there is a parent key associated with the key, specifying this parent key name also changes the key name in the parent key's subkey list.
- attrcheck  
  Set to true to check the built-in attributes(encryption, history, expiration date) of K2HDKC.
- pass  
  Specify passphrase when encrypting key(value).
- expire  
  To set the expiration date for the key, specify the expiration time second for this value.  
  If 0 is specified, the expiration date is not set.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.Rename('oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.Rename('slave.ini', 8031, null, true, false, 'oldkey', 'newkey', 'parentkey', true, 'passphrase', 1000);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('rename', Callback cbfunc) - Permanent Connection  
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
- on('rename', Callback cbfunc) - Onetime Connection  
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
- onRename(Callback cbfunc) - Permanent Connection  
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
- onRename(Callback cbfunc) - Onetime Connection  
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
This method pushes the data to K2HDKC queue.

#### Format
- Permanent Connection  
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
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- prefix  
  Specify the queue name prefix.
- key  
  If the target queue is the KeyQueue, specify the key name to push to the KeyQueue.  
  The queue of K2HDKC has types of Queue and KeyQueue.
- val  
  Specify the value to push to the Queue.  
  In the case of KeyQueue, it is the value of the key to be pushed.
- is_fifo  
  Specify the direction(FIFO or LIFO) to push to the queue.
- attrcheck  
  Set to true to check the built-in attributes(encryption, history, expiration date) of K2HDKC.
- pass  
  Specify passphrase when encrypting value to be pushed.
- expire  
  To set the expiration date for the key, specify the expiration time second for this value.  
  If 0 is specified, the expiration date is not set.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.QueuePush('queue_', null,'data', true, true, 'mypass', 1000);
  dkcobj.QueuePush('keyqueue_', 'key', 'data', false, true, null, 0);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.QueuePush('slave.ini', 8031, null, true, false, 'queue_', null,'data', true, true, 'mypass', 1000);
  dkcobj.QueuePush('slave.ini', 8031, null, true, false, 'keyqueue_', 'key', 'data', false, true, null, 0);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('queuePush', Callback cbfunc) - Permanent Connection  
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
- on('queuePush', Callback cbfunc) - Onetime Connection  
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
- onQueuePush(Callback cbfunc) - Permanent Connection  
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
- onQueuePush(Callback cbfunc) - Onetime Connection  
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
This method pops the data from K2HDKC queue.

#### Format
- Permanent Connection  
  ```
  [bool/String/Array] QueuePop(String   prefix,
                               bool     is_fifo = true,
                               bool     is_key_queue = false,
                               String   pass = "",
                               Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- prefix  
  Specify the queue name prefix.
- is_fifo  
  Specify the direction(FIFO or LIFO) to push to the queue.
- is_key_queue  
  Set it to true if the target queue is treated as a KeyQueue.
- pass  
  When decrypting the value to be popped, specify the passphrase.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, String key, String value)
  function(Error error, String value)
  ```
  If an error occurs, error is not null.

#### Return Values
On successful completion, when it is Queue type, it returns the popped value.  
For KeyQueue, set the key name and value to **Array** and return it.  
If an error occurs, **null** is returned.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var value    = dkcobj.QueuePop('queue_', true, false, 'mypass');
  var valarray = dkcobj.QueuePop('keyqueue_', false, true, null);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var value    = dkcobj.QueuePop('slave.ini', 8031, null, true, false, 'queue_', true, false, 'mypass');
  var valarray = dkcobj.QueuePop('slave.ini', 8031, null, true, false, 'keyqueue_', false, true, null);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('queuePop', Callback cbfunc) - Permanent Connection  
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
- on('queuePop', Callback cbfunc) - Onetime Connection  
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
- onQueuePop(Callback cbfunc) - Permanent Connection  
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
- onQueuePop(Callback cbfunc) - Onetime Connection  
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
This method removes the data from K2HDKC queue without reading the value.

#### Format
- Permanent Connection  
  ```
  bool QueueRemove(String   prefix,
                   int      count = 0,
                   bool     is_fifo = true,
                   bool     is_key_queue = false,
                   String   pass = "",
                   Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- prefix  
  Specify the queue name prefix.
- count  
  Specify the count of queue for removing.
- is_fifo  
  Specify the direction(FIFO or LIFO) to push to the queue.
- is_key_queue  
  Remove it to true if the target queue is treated as a KeyQueue.
- pass  
  When decrypting the value to be removed, specify the passphrase.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.QueueRemove('queue_', 1, true, false, 'mypass');
  dkcobj.QueueRemove('keyqueue_', 1, false, true, null);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.QueueRemove('slave.ini', 8031, null, true, false, 'queue_', 1, true, false, 'mypass');
  dkcobj.QueueRemove('slave.ini', 8031, null, true, false, 'keyqueue_', 1, false, true, null);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('queueRemove', Callback cbfunc) - Permanent Connection  
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
- on('queueRemove', Callback cbfunc) - Onetime Connection  
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
- onQueueRemove(Callback cbfunc) - Permanent Connection  
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
- onQueueRemove(Callback cbfunc) - Onetime Connection  
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
This method initializes the key and value as CAS(Compare and Swap).

#### Format
- Permanent Connection  
  ```
  bool CasInit(String   key,
               uint     value = 0,
               String   pass = "",
               int      expire = 0,
               Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name for CAS.
- value  
  Specify the initial value for CAS.
- pass  
  Specify passphrase when encrypting value to be set.
- expire  
  To set the expiration date for the key, specify the expiration time second for this value.  
  If 0 is specified, the expiration date is not set.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasInit('caskey', 1, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasInit('slave.ini', 8031, null, true, false, 'caskey', 1, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('casInit', Callback cbfunc) - Permanent Connection  
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
- on('casInit', Callback cbfunc) - Onetime Connection  
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
- onCasInit(Callback cbfunc) - Permanent Connection  
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
- onCasInit(Callback cbfunc) - Onetime Connection  
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
This method gets the value by key as CAS(Compare and Swap).

#### Format
- Permanent Connection  
  ```
  int CasGet(String   key,
             String   pass = "",
             Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name for CAS.
- pass  
  When decrypting the value to be got, specify the passphrase.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error, int value)
  ```
  If an error occurs, error is not null.

#### Return Values
It returns the value(**int32**) at normal termination. **undefined** is returned when an error such as the absence of the key exists.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  var value = dkcobj.CasGet('caskey', 'mypass');
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  var value = dkcobj.CasGet('slave.ini', 8031, null, true, false, 'caskey', 'mypass');
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('casGet', Callback cbfunc) - Permanent Connection  
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
- on('casGet', Callback cbfunc) - Onetime Connection  
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
- onCasGet(Callback cbfunc) - Permanent Connection  
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
- onCasGet(Callback cbfunc) - Onetime Connection  
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
This method sets the value to the key as CAS(Compare and Swap).

#### Format
- Permanent Connection  
  ```
  bool CasSet(String   key,
              uint     oldval = 0,
              uint     newval = 0,
              String   pass = "",
              int      expire = 0,
              Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.*
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name for CAS.
- oldval  
  Specify the value before change.
- newval  
  Specify new value after change.
- pass  
  Specify passphrase when encrypting value to be set.
- expire  
  To set the expiration date for the key, specify the expiration time second for this value.  
  If 0 is specified, the expiration date is not set.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasSet('caskey', 1, 5, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasSet('slave.ini', 8031, null, true, false, 'caskey', 1, 5, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('casSet', Callback cbfunc) - Permanent Connection  
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
- on('casSet', Callback cbfunc) - Onetime Connection  
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
- onCasSet(Callback cbfunc) - Permanent Connection  
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
- onCasSet(Callback cbfunc) - Onetime Connection  
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
This method increments/decrements the value for the key as CAS(Compare and Swap).

#### Format
- Permanent Connection  
  ```
  bool CasIncDec(String   key,
                 bool     is_increment = true,
                 String   pass = "",
                 int      expire = 0,
                 Callback cbfunc = null
  )
  ```
- Onetime Connection  
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

#### Arguments
- conf  
  To initialize for Permanent Connection, specify the setting file(*.ini etc) specified when starting CHMPX slave node.
- port  
  To initialize for Onetime Connection, specify the setting control port number for CHMPX slave node.  
- cuk  
  To initialize for Onetime Connection, specify the setting CUK string for CHMPX slave node.
- auto_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, if the connection with the CHMPX slave node is disconnected, reconnection is automatically executed.  
  If false is specified for this value, reconnecting is not done and operations such as data manipulation will result in errors until reconnected.
- no_giveup_rejoin  
  To initialize for Onetime Connection, specify boolean value.  
  If this value is set to true, data operation will be blocked until it can reconnect with the CHMPX slave node.  
  If false is specified, attempts to reconnect to the upper limit for reconnection set for CHMPX slave node.
- key  
  Specify the key name for CAS.
- is_increment  
  Specify true for increment.
- pass  
  Specify passphrase when encrypting value.
- expire  
  To set the expiration date for the key, specify the expiration time second for this value.  
  If 0 is specified, the expiration date is not set.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.   
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous - Permanent Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init('slave.ini', 8031, null, true, false);
  
  dkcobj.CasIncDec('caskey', true, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- Synchronous - Onetime Connection  
  ```
  var k2hdkc = require('bindings')('k2hdkc');
  var dkcobj = new k2hdkc();
  
  dkcobj.init();
  
  dkcobj.CasIncDec('slave.ini', 8031, null, true, false, 'caskey', true, 'mypass', 1000);
  
  dkcobj.clean();
  ```
- Asynchronous - Permanent Connection  
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
- Asynchronous - Onetime Connection  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two type(four case) implementations.
- on('casIncDec', Callback cbfunc) - Permanent Connection  
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
- on('casIncDec', Callback cbfunc) - Onetime Connection  
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
- onCasIncDec(Callback cbfunc) - Permanent Connection  
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
- onCasIncDec(Callback cbfunc) - Onetime Connection  
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
This method puts the version about K2HDKC Node.js addon library.

#### Format
```
bool PrintVersion(int fd = null)
```

#### Arguments
- fd  
  Specify output stream.

#### Return Values
This method returns success(**true**) or failure(**false**).

#### Examples
```
var k2hdkc = require('bindings')('k2hdkc');
var dkcobj = new k2hdkc();

dkcobj.PrintVersion();
```
