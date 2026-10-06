# dm2とROS2を合わせて利用する方向けに事前に知っておくべき事

---

## 1. Ubuntu 22.04を使用している場合の、dm2とROS2のビルドの流れ

dm2をビルドする前に、[ROS2 Humble](https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debs.html) をインストールしておく必要があります。

ROS2 Humbleおよびcolcon buildではC++17が使用されるため、dm2およびdmiもC++17でビルドする必要があります。

dm2のmakefileやdmiのCMakeLists.txtにて、ROSのディストリビューションを検出し、C++17ビルドに切り替えるように設定しています。
そのため、setup.bashを読み込んでからのビルドをお願いします。

```bash
source /opt/ros/humble/setup.bash
```
上記後に[ビルド](#ビルド)

- もし既にROS2をビルドする前にdm2をビルドしている場合は、C++14で作られたdm2のビルド資源を`make clean`でクリーンした後、改めて、/opt/ros/humble/setup.bashを読み込み、dm2をビルドする流れになります。

## 2. Protocol Buffers のバージョン競合について

Ubuntu 22.04以前の環境でROS 2を使用している場合は、システムにインストールされている Protocol Buffers (`protoc`) と dm2 が必要とするバージョンが異なる場合があります。

dm2では **Protocol Buffers 3.21.12** を使用します。

既存の `protoc` を置き換えると、ROS 2や他のソフトウェアに影響する可能性があるため、dm2では **Protocol Buffers 3.21.12を別のディレクトリにインストールし、システムのprotobufと共存させる方法を推奨します。**

#### 2.1. protocのバージョン確認

まず最初に、システムにインストールされている `protoc` のバージョンを確認します。
```bash
protoc --version
```

下記の通り、既に3.21.12がインストールされていれば、以降のコマンドは必要ありません。

```text
libprotoc 3.21.12
```

古いバージョンが表示された場合は、別のディレクトリにインストールする手順へと進みます。

#### 2.2. システムディレクトリとは別のディレクトリにprotocをインストール

[dm2のインストールの依存ライブラリのインストール](README.md#依存ライブラリのインストール)に記載されている、以下の部分について

```bash
./configure
make -j$(nproc)
sudo make install
sudo ldconfig
```

下記のように変更します。`$HOME/protoc`は例であって、別のディレクトリを指定しても問題ありません。

```bash
./configure --prefix=$HOME/protoc
make -j$(nproc)
sudo make install
sudo ldconfig
```

実行後は、以下のようにインストールされた事が確認できます。

```text
$HOME/protoc/
├── bin/
│   └── protoc
├── include/
│   └── google/protobuf/
└── lib/
    └── libprotobuf.*
```

インストールした `protoc` のバージョンを確認します。

```bash
$HOME/protoc/bin/protoc --version
```

以下のように表示されればOKです。

```text
libprotoc 3.21.12
```

#### 2.3. dm2のビルド

`PROTO_PATH` に、Protocol Buffersを `./configure --prefix` でインストールした場所を指定してビルドします。

例えば、$HOME/protocにインストールする例を挙げます。

```bash
./configure --prefix=$HOME/protoc
```

上記パスにProtocol Buffersをインストールした場合は、

```bash
make all PROTO_PATH=$HOME/protoc
sudo make install PROTO_PATH=$HOME/protoc
sudo ldconfig
```

としてdm2をビルドします。

正常にビルドが完了すれば、通常のビルド手順である[RDBの設定](README.md#rdbの設定)を行います。

#### 2.4. dmiのビルドと実行

2.3と同様にProtocol Buffersのインストールした場所を指定して、dmiをビルドします。

```bash
source /opt/ros/<ros_distro>/setup.bash
colcon build --symlink-install --cmake-args -DDM2_PROTO_PATH=$HOME/protoc
```

ros2_dmi以外のCMakeパッケージにも、`DM2_PROTO_PATH`変数を渡すため、下記の警告が出力されますが、無視して下さい。

```text
CMake Warning:
  Manually-specified variables were not used by the project:

    DM2_PROTO_PATH
```

実行時には、`LD_LIBRARY_PATH`にProtocol Buffersのパスを追加した後に`ros2 launch`でdmiを起動します。

```bash
source ~/dm20/dmi/ros2/install/setup.bash 
export LD_LIBRARY_PATH=$HOME/protoc/lib:$LD_LIBRARY_PATH
ros2 launch ros2_dmi downloader.launch.py dm_user:=dm2sampleuser pass:=dm2samplepassword ip_addr:=127.0.0.1 
```

