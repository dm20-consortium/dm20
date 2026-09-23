# 第三者ソフトウェアのライセンス

dm2では、以下のオープンソースソフトウェア（OSS）およびOSSライブラリを利用しています。

各ソフトウェアには、それぞれのライセンスが適用されます。詳細なライセンス条件については、各ソフトウェアの公式サイトおよびライセンス文を参照してください。

| ソフトウェア / ライブラリ   | ライセンス                                       |
| ---------------- | ------------------------------------------- |
| PostgreSQL       | PostgreSQL License                          |
| PostGIS          | GPL-2.0-or-later                            |
| OpenSSL          | OpenSSL/SSLeay License / Apache License 2.0 |
| libpqxx          | BSD License                                 |
| GEOS             | LGPL                                        |
| Xerces-C++       | Apache License 2.0                          |
| Apache log4cxx   | Apache License 2.0                          |
| zlib             | zlib License                                |
| Zstandard        | BSD License                                 |
| Protocol Buffers | BSD 3-Clause License                        |
| yaml-cpp         | MIT License                                 |

## Dockerイメージについて

dm2では、GitHub Container Registry（GHCR）を通じて、以下のDockerイメージを提供しています。

* `dm2-is`
* `dm2-cs`
* `dm2-mes`
* `dm2-rdb`

Dockerイメージには、dm2本体の依存ライブラリに加えて、ベースイメージ、OSパッケージ、およびそれらの依存パッケージが含まれています。

そのため、本一覧はDockerイメージに含まれるすべてのソフトウェアのライセンスを網羅するものではありません。

Dockerイメージを再配布する場合は、各イメージに含まれるソフトウェアのライセンス条件も確認してください。

## 注意事項

本ページは、dm2が利用する主な第三者ソフトウェアのライセンスを示すものです。

各ソフトウェアの正式なライセンス条件については、それぞれのソフトウェアに付属するライセンス文および公式サイトを参照してください。
