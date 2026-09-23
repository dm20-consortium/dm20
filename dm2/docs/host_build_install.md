# C. ホスト上でソースコードからビルドする

---

## 動作確認環境

- Ubuntu 20.04, Ubuntu 22.04, Ubuntu 24.04, Ubuntu 26.04

- DMIが、Ubuntu 26.04未対応につき、DMI、dm2両方をホスト上でビルドしたい場合は、Ubuntu 20.04～24.04を推奨します。

## インストール概要

- Ubuntu LTS環境で、依存ライブラリをインストールした上で、makeを使用してビルドを行います。
- dm2とROS2を合わせて利用する方は、[事前に知っておくべき事](ros2_install.md)を参照下さい。

## 依存ライブラリ

### Ubuntu バージョン別の依存ライブラリ一覧表
| ライブラリ           | 20.04  | 22.04  | 24.04  | 26.04  |
| --------------- | ------ | ------ | ------ | ------ |
| g++             | 9.3.0  | 11.2.0 | 13.2.0 | 15.2.0 |
| build-essential | 12.8   | 12.9   | 12.10  | 12.12  |
| PostgreSQL      | 12.12  | 14.5   | 16.2   | 18.6   |
| PostGIS         | 3.0.0  | 3.2.0  | 3.4.2  | 3.6.2  |
| OpenSSL         | 1.1.1f | 3.0.2  | 3.0.13 | 3.5.5  |
| libpqxx         | 6.4    | 6.4    | 7.8.1  | 7.10.0 |
| GEOS            | 3.8.0  | 3.11.0 | 3.12.1 | 3.14.1 |
| Xerces          | 3.2.2  | 3.2.3  | 3.2.4  | 3.2.4  |
| log4cxx         | 0.10.0 | 0.12.1 | 1.1.0  | 1.5.0  |
| zlib            | 1.2.11 | 1.2.11 | 1.3    | 1.3    |
| Zstandard       | 1.4.4  | 1.4.8  | 1.5.5  | 1.5.7  |
| Protobuf        | 21.12  | 21.12  | 21.12  | 21.12  |
| yaml-cpp        | 0.6.2  | 0.7.0  | 0.8.0  | 0.8.0  |


## 依存ライブラリのインストール

### Ubuntu 20.04 / 22.04 / 24.04 / 26.04 共通

```bash
sudo apt update

sudo apt install -y \
  g++ \
  build-essential \
  postgresql postgresql-contrib \
  postgis \
  libproj-dev \
  libpq-dev libpqxx-dev \
  liblog4cxx-dev liblog4cxx-doc \
  libssl-dev \
  zlib1g-dev libzstd-dev \
  libyaml-cpp-dev
```
### Ubuntu 26.04のみ

```bash
sudo apt install -y \
  postgresql-18-postgis-3 \
  libpqxx-7.10 \
  libgeos++-dev libgeos-c1t64 libgeos-dev \
  libxerces-c3.2t64 libxerces-c-dev \
  protobuf-compiler libprotobuf-dev
sudo ldconfig
```

### Ubuntu 24.04のみ

```bash
sudo apt install -y \
  postgresql-16-postgis-3 \
  libpqxx-7.8t64 \
  libgeos++-dev libgeos-c1t64 libgeos-dev \
  libxerces-c3.2t64 libxerces-c-dev \
  protobuf-compiler libprotobuf-dev
sudo ldconfig
```

### Ubuntu 22.04のみ

```bash
sudo apt install -y \
  postgresql-14-postgis-3 \
  libpqxx-6.4 \
  libgeos++-dev libgeos-c1v5 libgeos-dev \
  libxerces-c3.2 libxerces-c-dev \
  libboost-dev cmake wget
  
wget https://download.osgeo.org/geos/geos-3.11.0.tar.bz2
tar xvf geos-3.11.0.tar.bz2
cd geos-3.11.0
mkdir _build && cd _build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j$(nproc)
sudo make install
cd ../..

wget https://github.com/protocolbuffers/protobuf/releases/download/v21.12/protobuf-cpp-3.21.12.tar.gz
tar zxvf protobuf-cpp-3.21.12.tar.gz
cd protobuf-3.21.12
./configure
make -j$(nproc)
sudo make install
sudo ldconfig
```

### Ubuntu 20.04のみ

```bash
sudo apt install -y \
  postgresql-12-postgis-3 \
  libpqxx-6.4 \
  libgeos++-dev libgeos-3.8.0 libgeos-dev \
  libxerces-c3.2 libxerces-c-dev wget

wget https://github.com/protocolbuffers/protobuf/releases/download/v21.12/protobuf-cpp-3.21.12.tar.gz
tar zxvf protobuf-cpp-3.21.12.tar.gz
cd protobuf-3.21.12
./configure
make -j$(nproc)
sudo make install
sudo ldconfig
```

## 依存ライブラリインストール後の手動設定変更 (Ubuntu 22.04のみ)

### log4cxx

- std::shared_mutexからboost::shared_mutexへの切り替えのため、log4cxxの設定ファイル`/usr/include/log4cxx/boost-std-configuration.h`の修正が必要です。

```cpp
#define STD_SHARED_MUTEX_FOUND 1
#define Boost_SHARED_MUTEX_FOUND 0
```

下記のように変更します。
※変更前にバックアップを取得してください。パッケージ更新で上書きされる可能性があります。
```cpp
#define STD_SHARED_MUTEX_FOUND 0
#define Boost_SHARED_MUTEX_FOUND 1
```

## ビルド

リポジトリのルートディレクトリ/dm2上で下記のコマンドを実行して下さい。

```bash
make all
sudo make install
sudo ldconfig
```

## RDBの設定

- RDB (postgreSQL) にDM2.0 Platform専用のデータベースを設定します。[init.sh](rdb/init.sh)内部で、IDに`dm2sampleuser`、パスワードに`dm2samplepassword`を設定しています。

```bash
sudo -u postgres bash rdb/init.sh 
```
