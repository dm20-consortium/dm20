# Cool4向けリアルタイムビューア 利用手順書

## 1 概要

下記機能を提供する

- DM2.0PFが出力する物標情報、信号情報、フリースペース情報をリアルタイムに可視化する

## 2 動作確認環境と前提条件
- Ubuntu 20.04 ~ 24.04
- Python 3.8, 3.10

- DM2.0PFの稼働マシンと可視化ツール動作のマシンが別に存在する場合（例. 車載DMに流れている情報を遠隔で監視したいなど）であっても本ツールは機能する。その場合を踏まえて本手順内では、DM2.0PFが稼働するマシン側を[送信マシン]、可視化し表示するマシンを[受信マシン]と呼び区別する。<br>
なお、[送信マシン]と[受信マシン]間にはPOSTリクエストを使用したデータ連携が発生する。両マシンを接続する方法については環境によって手法が異なるため割愛する。

- DM2.0PFに流れる物標情報および信号情報が出力されたファイルから情報を得て可視化する。そのため送信マシンにはDM2.0PFのインストールおよびDM2.0PFメッセンジャーアプリを利用したログのファイル出力が必要である。受信マシンにはDM2.0PFをインストールする必要はない。

## 3 インストール方法
### 3.1 仮想環境の導入
Python3のバージョンを調べて下さい。
```bash
python3 -V
```

Python3のバージョンによって、仮想環境の導入方法が分かれます。

- [Python3のバージョンが3.10以下の場合](#31a-仮想環境の導入)
- Python3のバージョンが3.11以上の場合

### 3.1.a Python3.10以下の場合の仮想環境の導入方法

```bash
sudo apt install python3.8-venv
python3 -m venv env
source env/bin/activate
```

### 3.1.b Python3.11以上の場合の仮想環境の導入方法

3.11以上だとstreamlitライブラリのインストールにエラーが発生するケースがあるため、
pyenvを用いて3.8~3.10のバージョン環境を作成することが必要になります。

```bash
git clone https://github.com/pyenv/pyenv.git ~/.pyenv
echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.bashrc
echo 'command -v pyenv >/dev/null || export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.bashrc
echo 'eval "$(pyenv init -)"' >> ~/.bashrc
git clone https://github.com/pyenv/pyenv-virtualenv.git ~/.pyenv/plugins/pyenv-virtualenv
echo 'eval "$(pyenv virtualenv-init -)"' >> ~/.bashrc
source ~/.bashrc
pyenv install 3.8.18
```

Ubuntu24 WSLの場合、pyenv install 3.8.18時点で、パッケージが不足しているとなる（インストール自体は成功）
pyenv uninstall 3.8.18して、下記を行うと解消される。
```bash

sudo apt update
sudo apt install -y \
    libncurses5-dev \
    libncursesw5-dev \
    libreadline-dev \
    zlib1g-dev \
    libbz2-dev \
    libffi-dev \
    libssl-dev \
    libsqlite3-dev \
    liblzma-dev \
    tk-dev \
    libgdbm-dev \
    libnss3-dev \
    uuid-dev
```

インストールされたか確認する

```bash
 pyenv versions
```

```test
* system (set by /home/dm2/.pyenv/version)
  3.8.18
  3.8.18/envs/myenv
```

作成後に環境を起動する
```bash
pyenv activate myenv
```



### 3.1 ライブラリのインストール[送信マシン][受信マシン]
- 仮想環境を立ち上げている状態で以下コマンドを実行
```bash
pip install -r requirement.txt
```

### 3.2 マップタイルサーバの準備(任意)
  - Open Street Mapなどのグローバルサーバを利用する場合には不要。
  ローカル、クラウドインスタンス、オンプレミス環境などにタイルサーバを設置する場合、”planetiler”を使用する。
  https://github.com/onthegomap/planetiler
    ```bash
    docker run -e JAVA_TOOL_OPTIONS="-Xmx1g" -v "$(pwd)/data":/data ghcr.io/onthegomap/planetiler:latest --download --area=japan
    ```
- Lanelet2をベースにしたGeoJson形式のベクターファイルをタイルマップにしたい場合
    ```bash
    docker run -it -v $(pwd):/data -p 8080:8080 wifidb/tileserver-gl -p 8080 --mbtiles  "GeoJson記述ファイル　例AS.mbtiles" --restart always
    ```
　　上記2つのコンテナを同じマシンで動かしたい場合はポート指定を変更する(例. 8081:8081)

## 4 使用手順
### 4.0 パラメータ設定(初回時のみ設定)
#### 4.0-1. send_udp.pyへID関連の情報を登録する。[送信マシン]
可視化したい交差点の情報を書き加える。<br>

例として"aaa"交差点を可視化するための設定を記載する。条件は以下の通り<br> 
- 物標情報、信号情報、フリースペース情報を送信
- 物標情報情報源リスト:[111111]
- 交差点ID:1,　信号灯器IDのリスト:"[1,2,3]","[4,5,6]"
- フリースペース情報源リストID:[22222]

```ini
[send_udp.py]
rec_list = []
~~ ここに任意の名前で、交差点ごとに情報格納用リストを作る ~~
position = []

例：
rec_list = []

aaa_obj = []
aaa_sig = []
aaa_free = []

position = []
```

```ini
[send_udp.py]
sig_dict = {"交差点ID":["信号灯器IDのリスト"(カンマ(,)をパイプ(|)に置き換えること)],}

例：sig_dict = {"1":["[1|2|3]","[4|5|6]"]}
```

```ini
[send_udp.py]
if "rec_sig" in rec_dict.keys():
  for line in rec_dict["rec_sig"]:
    sid,signal,_= line.split(",",2)
    ~~以下に交差点IDとの一致if文を追加~~
    例:
    if sid == "1":
      if signal in sig_dict[sid]:
        aaa_sig.append(line)
        sig_dict[sid].remove(signal)
```

```ini
[send_udp.py]
if "rec_obj" in rec_dict.keys():
  for line in rec_dict["rec_obj"]:
    col = line.split(",")
    _objid = col[0]
    sid = col[-1]
    if objid == _objid:
        continue
    objid = _objid
    ~~以下に物標情報情報源リストとの一致if文を追加~~
    例:
    if sid == "[111111]":
      aaa_obj.append(line)
```

```ini
[send_udp.py]
if "rec_free" in rec_dict.keys():
  free_id = ""
  for line in rec_dict["rec_free"]:
    col = line.split(",")
    sid = col[-2]
    ~~以下にフリースペース情報源リストIDとの一致if文を追加~~
    例:
    if sid == "[22222]":
      aaa_free.append(line)
```

```ini
[send_udp.py]
sig_message = bytes(json.dumps({
  "pos":position,
  ~~以下に交差点名称のdictを追加~~
  例:
  "aaa":{
    "obj":aaa_obj,
    "sig":aaa_sig,
    "free":aaa_free
  }}), 'utf-8')
```

#### 4.0-2. 描画用設定ファイルを登録する。[受信マシン]
可視化したい交差点の例として4.0-1と同条件の交差点を想定する。<br>
- setting.yamlを複製し、交差点名にリネームしたyamlファイルをsettingディレクトリ内に設置する。記述方法はファイル内のコメントを参照。

- setting/tracking.yamlの内容を変更する。こちらもファイル内のコメントを参照。

- app.pyとdata_collector.pyを編集する。
  ```ini
  [app.py]
  choice = st.radio(~~~,
    options=("position", "all","交差点名"),
    ~~~
  )
  ```

  ```ini
  [data_collector.py]
  def calc_distance(position, tracking_dic):
    stops = ["","交差点名",""] #バスのように巡回するケースで利用したため、通過順に記載する

    ~~~

  def udp_collector(queue: Queue, host="0.0.0.0", port={受信マシンのポート指定}):
    ~~~
      if rsu == "交差点名": # 追加
        cnt = document["rsu_point"]["交差点名"] # 追加
      else:
        cnt = document["map_center"]
        zoom = 16
    ~~~

  ```

### 4.1 各マシンでの起動手順
#### 4.1-1. scriptフォルダ内シェルスクリプト記載のファイルパスを、現在メッセンジャーアプリを利用したログ出力保存しているファイルに変更する。[送信マシン]
```ini
[例 script/signal_check.sh]
tail -n 100 ${HOME}/dm2/scheduler/backup_tool/${SCHEMA}/${SCHEMA}_${CHECK_DAY}.csv  | ...
```

#### 4.1-2. 情報送信用のPythonスクリプトにて受信マシンのIPとポートを設定する。[送信マシン]
```ini
[send_udp.py]
server_address = "{受信マシンのIP}"
server_port = "{受信マシンのポート指定}"
```
※PORTはウェルノウンポート以外の任意番号を設定

#### 4.1-3. data_collector.pyにて受信用のポートを設定する。[受信マシン]
```ini
[data_collector.py]
def udp_collector(queue: Queue, host="0.0.0.0", port="{4.1-2で指定したポート番号}")
```

#### 4.1-4. 各スクリプトを別ターミナルで起動する。[受信マシン]
各bashファイル(activate_map,activate_ws_server)を参考に起動する。
起動順はws_server.py -> app.py

※app.pyの初回実行時、メールアドレスを求められる場合には空欄のままEnterキーを押す

#### 4.1-5. スクリプトを起動する。[送信マシン]
bashファイル(activate_sendudp)を参考に起動する。

※DM2.0PFが動作するマシン内で可視化する場合は送信マシン、受信マシンの区別なくすべての操作を行う
送信先IPは"localhost"となる

### 4.2 パラメータ変更箇所
#### マップサーバを変更する場合
ラスタータイル(地図画像)を変更する場合
```ini
[app.py]
L.tileLayer('マップサーバへのアクセス記述', {maxZoom:20, attribution:'適切なatribute'}).addTo(map);    
```
3.2 マップタイルサーバの準備(任意)のplanetilerを使用している場合

- アクセス記述：http://{コンテナ起動環境のIP}:8080/styles/basic-preview/512/{z}/{x}/{y}.png

- attribute：&copy; <a href="https://openmaptiles.org/">OpenMapTiles</a> | &copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors


ベクタータイル(GeoJson形式)を変更する場合
```ini
[app.py]
L.vectorGrid.protobuf('マップサーバへのアクセス記述', {vectorTileLayerStyles:"レイヤーのスタイル指定"}).addTo(map);
```
柏の葉地域環境で用いた記述例
- アクセス記述：http://localhost:8080/data/AS/{z}/{x}/{y}.pbf
- レイアウト記述：{"A":{"fill": true,"fillColor":"#696969","color":'transparent'},"S":{"color":"#000000","weight":1,"opacity":0.5}}

※"A"がpolygon,"S"がline部分の指定となっている

## 5 リンク

- お問い合わせ先

  admobi-dm2-conso-sec@nces.i.nagoya-u.ac.jp

- 先進モビリティサービスのための情報通信プラットフォームに関するコンソーシアムについて

  https://www.nces.i.nagoya-u.ac.jp/admobi-dm2/index.html

## 6 更新履歴

| 日時 | 更新者 | 更新内容 |
| ---- | ---- | ---- |
| 2025.03.27 | 竹内 | 新規作成 |
| 2026.03.19 | 竹内 | ツール更新に伴い更新 |

以上
