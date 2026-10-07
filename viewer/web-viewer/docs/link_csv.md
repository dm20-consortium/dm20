# 緯度・経度を保存したCSVデータをWebビューアで可視化する

---

## 手順

### 1. DM Web Viewerの起動

[クイックスタート](../README.md#1-地図タイルサーバの設定)の「1. 地図タイルサーバの設定」から「3. Webブラウザの起動」まで実施します。

ただし、[クイックスタートの2.Dockerコンテナの起動](../README.md#2-dockerコンテナの起動)では、`scale`オプションを追加します。

```bash
mkdir -p /tmp/dm2-web-viewer/csv
docker run --rm -it \
    -p 33013:33013 \
    -v /tmp/dm2-web-viewer/csv:/data/csv \
    ghcr.io/dm20-consortium/dm2-web-viewer:latest \
    --tile-url ${TILE_URL}$ \
    --attribution ${TILE_COPYRIGHT}
    --scale 1
```

### 2. リプレイツールを使って、サンプルCSVファイルを読み込み、Webブラウザに表示させる

リポジトリのルートディレクトリ/viewer/web-viewer/csv/sample/上にあるリプレイツールを使って、同ディレクトリにあるサンプルファイル`time_lat_lon_sample1.csv`を取り込みます。

出力先は、[クイックスタートの2.Dockerコンテナの起動](../README.md#2-dockerコンテナの起動)で指定したパスを指定します。

```bash
bash replay_csv_realtime.sh time_lat_lon_sample1.csv 1 its stdout >> /tmp/dm2-web-viewer/csv/viewer_input.csv 
```

### 3. Webブラウザで確認

サンプルファイル`time_lat_lon_sample1.csv`に記録された生成時刻の時間軸、および緯度・経度の地点に沿って、物標がリプレイ表示される様子が確認できます。

## サンプルCSVファイルの解説

### a.生成時刻,緯度,経度の場合

サンプルファイル`time_lat_lon_sample1.csv`は、生成時刻（単位：ミリ秒）,緯度,経度の列から構成されます。

Webビューアの緯度・経度の単位は0.1 マイクロ度が標準ですが、[Webビューア起動](#1-dm-web-viewerの起動)時の`scale`オプションを1にする事で、1度単位への変更が可能です。

```text
0,35.1539050,136.9660830
1000,35.1540010,136.9661530
2000,35.1541040,136.9662510
3000,35.1541940,136.9663310
4000,35.1543000,136.9664080
```

### b.生成時刻が Date 型の場合

生成時刻が Date 型で記述されていても、可視化する事が可能です。

以下は、サンプルファイル`time_lat_lon_sample2.csv`になります。

```text
2026-10-07 00:00:00.001,35.1539050,136.9660830
2026-10-07 00:00:01.001,35.1540010,136.9661530
2026-10-07 00:00:02.001,35.1541040,136.9662510
2026-10-07 00:00:03.001,35.1541940,136.9663310
2026-10-07 00:00:04.001,35.1543000,136.9664080
```

2026-10-07 00:00:00, 2026-10-07T00:00:00.001, 2026/10/07 00:00:00.001の書式でも問題ありません。

[リプレイツール](#2-リプレイツールを使ってサンプルcsvファイルを読み込みwebブラウザに表示させる)を使う際に、引数を`date`に変える事で可視化する事ができます。

```bash
bash replay_csv_realtime.sh time_lat_lon_sample2.csv 1 date stdout >> /tmp/dm2-web-viewer/csv/viewer_input.csv 
```

