# UDPのサンプルデータ生成ツールを使って、DM2.0 Platformとの連携を確認する
---
## 1. 概要
---

UDP端末（UDPデータを送信/受信する端末）とDM端末（DM2.0をインストールした端末）との連携動作を確認できます。

対応するバイナリ型のUDPデータについては、[UDP_DMIの概要](../../dmi/udptcp/udp_dmi/README.md#概要)を参照して下さい。

![入出力の仕組み](images/udpdmi.png)

---

## 2 DM2.0 Platformの動作確認環境

Ubuntu 20.04, Ubuntu 22.04, Ubuntu 24.04

### Docker環境
- [dmiの動作確認環境](../../dmi/README.md#動作確認環境)を参照

## 3 導入手順

### 3.1 DM端末へdmiをインストール

- [dmiのインストール](../../dmi/README.md#dockerイメージの構築)を参照


### 3.2 DBシステム・DMIの起動

下記２パターンで起動方法が分かれます。

- [a. DBシステム・DMIをDockerイメージで構築した場合](#32a-dockerイメージで構築した場合の-dbシステムdmiの起動方法)
- [b. 手動インストールした場合（非Dockerの場合）](#32b-手動インストールでdbシステムとudp_dmiをインストールした場合の-dbシステムdmiの起動方法)

### 3.2.a Dockerイメージで構築した場合の DBシステム・DMIの起動方法
- [リポジトリのルートディレクトリ/dm2/conf/dmiConf.yml](../../dm2/conf/dmiConf.yml)を編集します。

 例えば、物標情報をUDPデータで流す場合は下記の通り、udp の receiver / sender の`object_info_0_8_1`に関する部分のコメントアウトを外し、`targetIpAddr`にUDP端末（受信側）のIPアドレスを設定して下さい。
 
 複数の情報を同時に流す場合は、各情報毎、コメントアウトしている部分を外していきます。

```text
 udp:
   receiver:
     object_info:
       targetTable: object_info_0_8_1
       receptionPort: 54345
   sender:
     object_info_0_8_1:
       targetIpAddr: <UDP端末（受信側）のIPアドレス>
       targetPort: 44345
       frameRateMaxMilsec: 33
       dataNumPerFrame: 6
       query: master sysTimer100msec select * from object_info_0_8_1 [range 300 msec]
```

DM2.0 PlatformのDBシステムを起動します。Dockerイメージで構築した場合のUDP_DMIは、DBシステム内から動的ライブラリとして呼び出されるため、起動コマンドはありません。

```bash
dm2is 
```
### 3.2.b 手動インストールでDBシステムとUDP_DMIをインストールした場合の DBシステム・DMIの起動方法

DM2.0 PlatformのDBシステムを起動します。引数にはリポジトリのルートディレクトリ/dm2/confディレクトリを指定して下さい。

```bash
dm2is -d ~/dm20/dm2/conf
```

別ターミナルでUDP_DMIのUDP-Sender（DM2.0-downloader）を起動します。下記は物標情報の起動例になります。

`dm_user`と`dm_pass`は、[dm2インストール時の初期設定](../../dm2/README.md#rdbの設定)の値です。

```bash
udp_dmi_sender_object_info --dm_user dm2sampleuser --dm_pass dm2samplepassword --target_port 44345 --target_ipaddr <UDP端末（受信側）のIPアドレス>
```

別ターミナルでUDP_DMIのUDP-Receiver（DM2.0-uploader）を起動します。下記は物標情報の起動例になります。

```bash
udp_dmi_receiver_object_info --dm_user dm2sampleuser --dm_pass dm2samplepassword --receive_port 54345
```

物標情報以外の sender / receiver 起動コマンドは、下記になります。
| API仕様上での情報名 | UDP受信プログラム | UDP送信プラグラム | デフォルトで使用するポート番号（変更可） |
| ---- | ---- | ---- | ---- |
| 物標情報 | udp_dmi_receiver_object_info | udp_dmi_sender_object_info | 54345 |
| フリースペース情報 | udp_dmi_receiver_freespace_info | udp_dmi_sender_freespace_info | 54346 |
| 信号情報 | udp_dmi_receiver_signal_info | udp_dmi_sender_signal_info | 54347 |
| センサー情報 | udp_dmi_receiver_sensor_info | udp_dmi_sender_sensor_info | 54348 |


### 3.3 UDPデータ生成・送信（UDP端末 - 送信側）

事前に下記ツールをインストールします。

```bash
pip install scapy
```

UDPデータを生成・送信する[サンプルスクリプト](python)をUDP端末（送信側）にコピーして、起動します。

下記は物標情報の例です。

```bash
python3 udp_sender.py  --port 54345 --format ../../../docs/yamls/object_info.yaml --mode sample --interval 1.0 --ip <DM端末のIPアドレス>
```

信号情報を送信したい場合は、下記の通りになります。

```bash
python3 udp_sender.py  --port 54345 --format ../../../docs/yamls/signal_info.yaml --mode sample --interval 1.0 --ip <DM端末のIPアドレス>
```

物標情報と信号情報は、`--mode`に`csv`を指定し、CSVファイルを入力値とする事も可能です。

```bash
python3 udp_sender.py  --port 54345 --format ../../../docs/yamls/object_info.yaml --mode csv --value_csv object_info_value.csv --ip <DM端末のIPアドレス>
python3 udp_sender.py  --port 54345 --format ../../../docs/yamls/signal_info.yaml --mode csv --value_csv signal_info_value.csv --ip <DM端末のIPアドレス>
```

フリースペース情報を送信したい場合は、下記の通りになります。可変長配列を扱うため、可読性の観点から yaml ファイルを入力値としたスクリプトを使います。

```bash
python3 udp_sender_yaml.py  --port 54345 --format ../../../docs/yamls/freespace_info.yaml --value freespace_info_value.yaml --ip <DM端末のIPアドレス>
```

センサー情報を送信したい場合は、下記の通りになります。センサー情報は2次元可変長配列を扱う事から、専用のスクリプトを使います。

```bash
python3 udp_sender_yaml_sensor_info.py --port 54345 --format ../../../docs/yamls/sensor_info.yaml --values sensor_info_value.yaml --ip <DM端末のIPアドレス>
```

### 3.4 UDPデータ受信確認（DM端末側）

DM端末側で、受信できている事を確認します。

`-S`で指定しているのは物標情報のデータストリーム名になります。

```bash
dm2mes -r -S object_info_0_8_1
```

物標情報以外のデータストリーム名は、下記になります。

| API仕様上での情報名 | データストリーム名 |
| ---- | ---- |
| 物標情報 | object_info_0_8_1 |
| フリースペース情報 | freespace_info_0_8_1 |
| 信号情報 | signal_info |
| センサー情報 | sensor_info_0_8_1 |

### 3.5 UDPデータの受信確認（UDP端末 - 受信側）

事前に下記ツールをインストールします。

```bash
pip install scapy
```

UDPデータを受信する[サンプルスクリプト](python)をUDP端末（受信側）にコピーして、起動します。

下記は物標情報の例です。

```bash
python3 udp_receiver.py --port 54345 --format ../../../docs/yamls/object_info.yaml --output_csv received_object_info.csv 
```

信号情報を送信したい場合は、下記の通りになります。

```bash
python3 udp_receiver.py --port 54345 --format ../../../docs/yamls/signal_info.yaml --output_csv received_signal_info.csv 
```

フリースペース情報を送信したい場合は、下記の通りになります。

```bash
python3 udp_receiver_yaml.py --port 54345 --format ../../../docs/yamls/freespace_info.yaml --output_yaml received_freespace_info.yaml
```

センサー情報を送信したい場合は、下記の通りになります。

```bash
python3 udp_receiver_yaml_sensor_info.py --port 54345 --format ../../../docs/yamls/sensor_info.yaml --output_yaml received_sensor_info.yaml
```

### 4 複数台のDMを利用した構成

- DM端末を2台以上用意することで、例えば、V2Xの実用的な構成（例：道路インフラ装置内のUDPデータを車両側へ連携させる構成）が可能です。
- 2台のDM端末間の通信動作例については、[こちら](../../example/README.md)