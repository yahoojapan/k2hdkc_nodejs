---
layout: contents
language: en-us
title: Build
short_desc: K2HDKC Node.js - K2HDKC addon library for Node.js
lang_opp_file: buildja.html
lang_opp_word: To Japanese
prev_url: usage.html
prev_string: Usage
top_url: index.html
top_string: TOP
next_url: developer.html
next_string: Developer
---

# Building
The build method for K2HDKC Node.js addon library is explained below.

## 1. Install prerequisites before compiling
- For recent Debian-based Linux distributions users, follow the steps below:
```
$ sudo aptitude update
$ sudo aptitude git gcc g++ make gdb dh-make fakeroot dpkg-dev
$ sudo aptitude install nodejs npm
```
- For users who use supported Fedora other than latest version, follow the steps below:
```
$ sudo dnf install git autoconf automake gcc libstdc++-devel gcc-c++ make libtool
$ sudo dnf install nodejs npm
```
- For other recent RPM-based Linux distributions users, follow the steps below:
```
$ sudo yum install git autoconf automake gcc libstdc++-devel gcc-c++ make libtool
$ sudo yum install nodejs npm
```

## 2. K2HDKC library
In advance, you need to install the developer package of [K2HDKC](https://k2hdkc.antpick.ax/) library(See [here](https://k2hdkc.antpick.ax/usage.html) for installation).  
Without installing this package, you can also build the [K2HDKC](https://k2hdkc.antpick.ax/) library(See [here](https://k2hdkc.antpick.ax/build.html) for the build).

## 3. Clone source codes from Github
```
$ git clone git@github.com:yahoojapan/k2hdkc_nodejs.gif
$ cd k2hdkc_nodejs
```

## 4. Building and Testing
Run the build and test using the [npm](https://www.npmjs.com/get-npm) command.
```
$ sudo npm cache clean
$ npm update
$ npm run build
$ npm run test
```
If the building and testing succeed, you can find **k2hdkc.node** file in the following path. (And a symbolic link **build** under the top directory.)
```
$ ls -la build
lrwxrwxrwx 1 guest users     43 Nov 30 14:00 build -> /home/guest/k2hdkc_nodejs/src/build

$ ls -l src/build/Release/*.node
total 996
-rwxr-xr-x 1 guest users 454613 Nov 30 14:00 k2hdkc.node
```
