# ローカル地図タイルサーバの構築

## 概要

DM Web Viewerでは、外部の地図タイルサービスに加えて、ローカル環境に構築した地図タイルサーバを利用できます。

ローカル地図タイルサーバを利用することで、公開タイルサービスへのアクセスを減らし、高いZoomレベルまで拡大する事が可能です。

本書では、以下のソフトウェアを利用します。

- [Planetiler](https://github.com/onthegomap/planetiler)：OpenStreetMapデータから地図タイルを生成
- [TileServer GL](https://github.com/maptiler/tileserver-gl)：生成した地図タイルを配信

ローカル環境にタイルを保存して利用する場合も、データのライセンス条件が免除されるわけではありません。

利用規約、ライセンス、および帰属表示等の条件を遵守してください。

## 1. 地図タイルの生成

作業用ディレクトリを作成します。

```bash
mkdir -p tileserver/data
cd tileserver
```

Planetilerを利用して日本の地図データを生成します。

```bash
docker run --rm \
    -e JAVA_TOOL_OPTIONS="-Xmx4g" \
    -v "$(pwd)/data:/data" \
    ghcr.io/onthegomap/planetiler:latest \
    --download --area=japan
```

生成されるタイルファイルの名前と形式は、使用するPlanetilerのバージョンおよびオプションによって異なります。

生成後、以下でGBサイズの地図データがダウンロードできている事を確認してください。

```bash
ls -lh data/
```

日本全域のタイル生成には、相応のディスク容量、メモリ、処理時間が必要です。実行環境に応じてメモリ設定や生成範囲を調整してください。

## 2. TileServer GLの起動

生成したタイルファイルをTileServer GLで配信します。

作業用ディレクトリ上で実行します。

```bash
cd tileserver
```

以下は生成物が `output.mbtiles` の場合の例です。

```bash
docker run --rm -it \
    --name tileserver-gl \
    -p 8080:8080 \
    -v "$(pwd)/data:/data:ro" \
    maptiler/tileserver-gl:latest \
    --file output.mbtiles
```

ブラウザで以下にアクセスし、タイルサーバの起動を確認します。

```text
http://localhost:8080
```

## 3. DM Web Viewerへの設定

DM Web Viewerの地図タイルの外部設定を以下のように変更します。

```bash
TILE_URL="http://localhost:8080/styles/basic/{z}/{x}/{y}.png"
TILE_COPYRIGHT='&copy; <a href="https://openmaptiles.org/">OpenMapTiles</a> &copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap contributors</a>'
```

DM Web ViewerのDockerコンテナ起動時に、これらを引数として渡します。

```bash
docker run --pull always --rm -it \
    -p 33013:33013 \
    -p 8765:8765 \
    -p 33333:33333/udp \
    -v /tmp:/tmp \
    ghcr.io/dm20-consortium/dm2-web-viewer:latest \
    --tile-url "${TILE_URL}" \
    --attribution "${TILE_COPYRIGHT}" \
    --tile-max-zoom 20
```

`--tile-max-zoom` オプションを設定することで、高ズームレベルの表示を可能にしています。


