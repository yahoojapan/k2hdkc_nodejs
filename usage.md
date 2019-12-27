---
layout: contents
language: en-us
title: Usage
short_desc: K2HDKC Node.js - K2HDKC addon library for Node.js
lang_opp_file: usageja.html
lang_opp_word: To Japanese
prev_url: feature.html
prev_string: Feature
top_url: index.html
top_string: TOP
next_url: build.html
next_string: Build
---

# Usage
Try to check its operation after building K2HDKC Node.js addon library.

## 1. Construction of execution environment
First of all, you need to build an execution environment.  
There are two ways to prepare for the environment.
One way is to install the package.
The other way is to build the k2hdkc nodejs addon library yourself.

### 1.1 Creating an environment using packages
This is a way to create an execution environment using packages.  
The package of k2hdkc nodejs addon library is in [npm](https://www.npmjs.com/package/k2hdkc).  

Before installing this package, you will need the K2HDKC developer package, so please refer to the [K2HDKC documentation](https://k2hdkc.antpick.ax/).  
There are two ways to install K2HDKC.
The first is installing from the package.(Please refer to [K2HDKC usage document](https://k2hdkc.antpick.ax/usage.html).)
The other way is to build K2HDKC by yourself.(Please refer to [K2HDKC build document](https://k2hdkc.antpick.ax/build.html).)  
Please create an environment that can use K2HDKC in any way.  

After installing the K2HDKC in advance, install the npm package of the k2hdkc nodejs addon library as follows.    
For details of the npm command, please refer to [here](https://docs.npmjs.com/misc/index.html#npm1).
```
$ npm install k2hdkc
```

### 1.2 Build to create an environment
You can build the k2hdkc nodejs addon library from the [source code](https://github.com/yahoojapan/k2hdkc_nodejs).  
See [here](https://nodejs.k2hdkc.antpick.ax/build.html) for how to clone and build the source code github repository.

## 2. Run K2HDKC server node processes and CHMPX processes
The [K2HDKC](https://k2hdkc.antpick.ax/) server node processes used by K2HDKC Node.js is required.  
For communication between the [K2HDKC](https://k2hdkc.antpick.ax/) server node processes and the client program, a server node and a slave node of communication middleware [CHMPX](https://chmpx.antpick.ax/) are also required.  
For details on how to start these processes, see [K2HDKC Usage](https://k2hdkc.antpick.ax/usage.html) and [CHMPX Usage](https://chmpx.antpick.ax/usage.html).  

These processes that start up for testing can be started as follows.  
```
$ cd k2hdkc_nodejs
$ test/run_process_helper.sh start_all
```
The **run_process_helper.sh** script is a helper script used in the test for this product and starts up two CHMPX server nodes, two K2HDKC processes, and one CHMPX slave node on your local host.  
You can check the process startup as shown below.  
```
$ ps ax
3156 pts/1    Sl     0:00 chmpx -conf /home/guest/k2hdkc_nodejs/test/test_server.ini -ctlport 8021
3162 pts/1    Sl     0:00 chmpx -conf /home/guest/k2hdkc_nodejs/test/test_server.ini -ctlport 8023
3168 pts/1    Sl     0:00 k2hdkc -conf /home/guest/k2hdkc_nodejs/test/test_server.ini -ctlport 8021
3176 pts/1    Sl     0:00 k2hdkc -conf /home/guest/k2hdkc_nodejs/test/test_server.ini -ctlport 8023
3185 pts/1    Sl     0:00 chmpx -conf /home/guest/k2hdkc_nodejs/test/test_slave.ini
```
To terminate these processes, proceed as follows.  
```
$ test/run_process_helper.sh stop_all
```
For details on these, please check the **run_process_helper.sh** script and configuration file(** *.ini**) in the `test` directory.

## 3. Examples
Please create the following file and check whether file creation, key and value writing and reading are possible.  
```
var k2hdkc = require('k2hdkc');                                     // import k2hdkc nodejs addon library
var	dkcobj = new k2hdkc('test_slave.ini', 8031, null, true, false); // initialize for permanent connecting

dkcobj.setValue('key', 'value', 'subkey', null, 0);                 // test for writing

dkcobj.getValue('subkey', null, true, null);                        // test for reading by subkey directly
dkcobj.getValue('key', 'subkey', true, null);                       // test for reading with key and subkey

dkcobj.clean();                                                     // cleanup
```

## 4. Importing and Executing
First, import the K2HDKC Node.js addon library with `require` to read the library in the JavaScript language.
After that, you can call the class, function, and method provided by K2HDKC Node.js addon library.
Please implement these depending on your environment and language(TypeScript etc.).

## 5. Other test
K2HDKC Node.js addon library provides unit testing using [Mocha](https://github.com/mochajs/mocha) and [Chai](https://github.com/chaijs/chai).
You can test K2HDKC Node.js addon library built by using the [npm](https://www.npmjs.com/get-npm) command as shown below.
```
$ npm run test
```
