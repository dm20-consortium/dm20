# dm2の実行例
---

dm2のコマンドによる操作方法、クエリの記述方法、アプリケーションの開発方法に関するドキュメントが含まれています。

## dm2のコマンドによる基本操作
---
1. [1端末上でDBシステムの入出力の動作を確認する](command/01_dm2is_to_dm2mes/README.md)
2. [2端末上でUDP(暗号化なし)通信を使って、ストリームデータを送受信する](command/02_dm2is_to_dm2cs/README.md)

## セキュリティに関する設定・操作
1. [セキュリティの仕組みを理解する](../docs/security.md)
2. [DTLS通信を使う](security/dtls.md)
3. [UDP(暗号化あり)通信を使う](security/udp_cipher.md)
4. [UDP(暗号化あり・署名あり)通信を使う](security/udp_pki.md)

## ROS2トピックとDM2.0 Platformの連携 ([DMIのインストール](../dmi/README.md)が必要です)
---
1. [物標情報のROSトピックを生成して、DM2.0 Platformを通してsubscribe/publishする](ros2/object_info.md)

## UDPデータとDM2.0 Platformの連携 ([DMIのインストール](../dmi/README.md)が必要です)
---
1. [UDPのサンプルデータ生成ツールを使って、DM2.0 Platformとの連携を確認する](udp/sample.md)

## クエリの記述方法について
---
1. [クエリ言語仕様について理解する](https://www.nces.i.nagoya-u.ac.jp/admobi-dm2/images/dm2_query_language_specification_20260327.pdf)

## DM2.0 Platform内のアプリケーションの開発方法について
---
1. [サンプルアプリケーションを使う](application/README.md)
2. [DBシステムのAPI仕様について理解する](https://www.nces.i.nagoya-u.ac.jp/dm2/DMLib_API.pdf)