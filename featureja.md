---
layout: contents
language: ja
title: Feature
short_desc: K2HDKC Node.js - K2HDKC addon library for Node.js
lang_opp_file: feature.html
lang_opp_word: To English
prev_url: homeja.html
prev_string: Overview
top_url: indexja.html
top_string: TOP
next_url: usageja.html
next_string: Usage
---

# 特徴
K2HDKC Node.jsは、Yahoo! JAPANがオープンソースとして公開する分散KVSである[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)のライブラリをサーバーサイドJavaScript環境 [Node.js](https://nodejs.org/ja/)のアドオンライブラリとして実装しています。  
K2HDKC Node.jsを使って、利用者は[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)に接続し、データの操作を行うことができます。  
利用者は、一般的なNode.jsのアドオンライブラリやJavascriptライブラリと同様にK2HDKC Node.jsライブラリを利用することができます。  

## 同期と非同期
K2HDKC Node.jsは、[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)の機能を同期・非同期で提供します。  
利用者は、同期・非同期を自由に選択し、利用できます。

## 永続とワンタイム接続
K2HDKC Node.jsは、分散KVS [K2HDKC](https://k2hdkc.antpick.ax/indexja.html)のクライアントプログラムから使用されます。  
K2HDKC Node.jsが [K2HDKC](https://k2hdkc.antpick.ax/indexja.html)サーバー群に接続するとき、利用者は永続的な接続と、データ操作毎のワンタイム接続を選択できます。

## K2HDKCの特徴
K2HDKC Node.jsは、[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)ライブラリのNode.js アドオンライブラリです。  
K2HDKC Node.jsは、NoSQL Key Value Store(KVS) として[K2HDKC](https://k2hdkc.antpick.ax/indexja.html)の[特徴](https://k2hdkc.antpick.ax/featureja.html)を継承しています。  
以下の特徴・機能の詳細については[K2HDKC](https://k2hdkc.antpick.ax/featureja.html)を参照してください。
- 自動マージ
- 自動スケーリング
- キーとサブキー
- データ暗号化
- データ有効期限
- トランザクションプラグイン
- キュー
- CAS（Compare And Swap）
