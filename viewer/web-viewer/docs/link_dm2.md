# DM2.0PFのDBシステム上にある物標情報をWebビューアで可視化する

---

## 手順

### 1. dm2のインストール（まだインストールしていない場合）

[dm2のインストール](../../dm2/README.md)を実施します。

### 2. DM Web Viewerの起動

[クイックスタート](../README.md#1-地図タイルサーバの設定)の「1. 地図タイルサーバの設定」から「3. Webブラウザの起動」まで実施します。

### 3. dm2のDBシステムの実行

DBシステムを起動します。引数にはリポジトリのルートディレクトリ/dm2/confディレクトリを指定して下さい。

```bash
dm2is -d ~/dm20/dm2/conf
```

### 4. DBシステムのログを出力

DBシステムのログを出力します。出力先は、[クイックスタートの2.Dockerコンテナの起動](../README.md#2-dockerコンテナの起動)で指定したパスを指定します。

```bash
dm2mes -r -S object_info_0_8_1 > /tmp/dm2-web-viewer/csv/object_info_0_8_1_`date +"%Y%m%d"`.csv 
```

### 5. サンプルCSVファイルをDBシステムへ取り込む

リポジトリのルートディレクトリ/viewer/web-viewer/csv/sample/上で、サンプルファイル`object_info_0_8_1_20260929_sample.csv`をDBシステムのインプットとして取り込みます。

```bash
dm2mes -S object_info_0_8_1 -f object_info_0_8_1_sample1.csv -A 2 -a
```

### 6. Webブラウザで確認

サンプルファイル`object_info_0_8_1_sample1.csv`に記録された生成時刻の時間軸、および緯度・経度の地点に沿って、物標がリプレイ表示される様子が確認できます。
