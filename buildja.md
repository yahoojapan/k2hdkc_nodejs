---
layout: contents
language: ja
title: Build
short_desc: K2HDKC Node.js - K2HDKC addon library for Node.js
lang_opp_file: build.html
lang_opp_word: To English
prev_url: usageja.html
prev_string: Usage
top_url: indexja.html
top_string: TOP
next_url: developerja.html
next_string: Developer
---

# ビルド方法
K2HDKC Node.js アドオンライブラリをビルドする方法を説明します。

## 1. 事前環境
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
インストールするNode.jsの環境は個々の環境で整えてください。

## 2. K2HDKCライブラリ
[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)ライブラリを事前にパッケージ（開発者パッケージ）インストール、もしくはご自身でビルド・インストールしてください。  
[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)ライブラリのインストール方法は[こちら](https://k2hdkc.antpick.ax/usageja.html)、ビルド方法は、[こちら](https://k2hdkc.antpick.ax/buildja.html)を参照してください。

## 3. clone
```
$ git clone git@github.com:yahoojapan/k2hdkc_nodejs.gif
$ cd k2hdkc_nodejs
```

## 4. ビルド・テスト
[npm](https://www.npmjs.com/get-npm)コマンドを使ってビルドとテストを実行します。  
```
$ sudo npm cache clean
$ npm update
$ npm run build
$ npm run test
```
ビルド、テストが成功した場合には、以下のパスにk2hdkc.nodeファイルができています。（トップディレクトリ直下にbuildというシンボリックリンクもできています。）
```
$ ls -la build
lrwxrwxrwx 1 guest users     43 Nov 30 14:00 build -> /home/guest/k2hdkc_nodejs/src/build

$ ls -l src/build/Release/*.node
total 996
-rwxr-xr-x 1 guest users 454613 Nov 30 14:00 k2hdkc.node
```

