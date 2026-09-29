# DM Web Viewer

DM Web Viewerは、DM2.0PFで扱う物標情報をWebブラウザ上で可視化するためのWebベースのビューアです。

> **現在、ドキュメント拡充を進めています**
>
> 現在、主にDM2.0PF上の物標情報を表示する方法をドキュメント化しています。
> 今後、物標情報以外のCSVデータ単位による可視化についてもドキュメント化する予定です。
>
> 近日中にアップデートを予定しています。

## 動作確認環境

Dockerが利用可能な環境

開発者は、主にUbuntuやWindows上のWSL2環境で動作を確認しています。
DockerまたはDocker互換のコンテナ実行環境が利用できる環境であれば実行できます。

## Dockerイメージ

DockerイメージはGitHub Container Registry（GHCR）で公開しています。

```bash
docker pull ghcr.io/dm20-consortium/dm2-web-viewer:latest
```

## クイックスタート

### 1. 地図タイルサーバの設定

本ソフトウェアは地図タイルを同梱しておらず、地図タイルの提供元を外部設定できます。

利用者は、設定した地図タイル提供サービスの利用規約、ライセンス、および帰属表示等の条件を遵守してください。

例えば、下記のようにOpenStreetMapのタイルを利用する場合は、OpenStreetMap FoundationのTile Usage Policyおよびライセンス条件に従ってください。

```bash
TILE_URL="https://tile.openstreetmap.org/{z}/{x}/{y}.png"
TILE_COPYRIGHT="&copy; OpenStreetMap contributors"
```

### 2. Dockerコンテナの起動

以下のコマンドでWeb Viewerを起動します。

```bash
mkdir -p /tmp/dm2-web-viewer/csv
docker run --rm -it \
    -p 33013:33013 \
    -v /tmp/dm2-web-viewer/csv:/data/csv \
    ghcr.io/dm20-consortium/dm2-web-viewer:v1.2.1 \
    --tile-url ${TILE_URL}$ \
    --attribution ${TILE_COPYRIGHT}
```

### 3. Webブラウザの起動

起動後、Webブラウザから以下にアクセスします。

```text
http://localhost:33013
```

### 4. dm2のDBシステムの実行

先に[dm2のインストール](../../dm2/README.md)を済ませておいて下さい。

DBシステムを起動します。引数にはリポジトリのルートディレクトリ/dm2/confディレクトリを指定して下さい。

```bash
dm2is -d ~/dm20/dm2/conf
```

### 5. DBシステムのログを出力

DBシステムのログを出力します。出力先は、[2.Dockerコンテナの起動](#2-dockerコンテナの起動)で指定した出力パスです。

```bash
dm2mes -r -S object_info_0_8_1 >/tmp/dm2-web-viewer/csv/object_info_0_8_1_`date +"%Y%m%d"`.csv 
```

### 6. サンプルCSVファイルをDBシステムへ取り込む

リポジトリのルートディレクトリ/viewer/web-viewer/csv/sample/上で、サンプルファイル`object_info_0_8_1_20260929_sample.csv`をDBシステムのインプットとして取り込みます。

```bash
dm2mes -S object_info_0_8_1 -f object_info_0_8_1_20260929_sample.csv -A 2 -a > /tmp/dm2-web-viewer/csv
```

### 7. Webブラウザで確認

サンプルファイル`object_info_0_8_1_20260929_sample.csv`に記録された生成時刻の時間軸、および緯度・経度の地点に沿って、物標がリプレイ表示される様子が確認できます。

## OSS・ライセンス

DM Web Viewerでは、複数のOSSおよびオープンソースライブラリを使用しています。

### アイコン

Web Viewerで使用している物標アイコンにはFont Awesome Ver7.3.1のアイコンを使用しています。

使用しているアイコンについては、Font Awesomeのライセンス条件に従って利用しています。

アイコンを変更・追加する場合は、使用するアイコンのライセンスおよび利用条件を確認してください。

### Pythonライブラリ

Web Viewerでは、PythonのOSSライブラリを使用しています。

主なライブラリは以下のとおりです。

* Streamlit
* PyYAML
* その他 [requirement.txt](./requirement.txt) に記載されているライブラリ

各ライブラリのライセンスについては、各ライブラリの配布元およびライセンスファイルを確認してください。

各パッケージには間接的な依存関係が存在するため、実際のDockerイメージには上記以外のOSSが含まれる場合があります。各OSSのライセンスおよび著作権表示については、各パッケージの配布元に記載されたライセンス条件を確認してください。

DM Web Viewerを再配布・改変する場合は、各OSSのライセンス条件を遵守してください。

## ソースコード

DM Web Viewerのソースコードは、本リポジトリの以下にあります。

```text
dm2/viewer/web-viewer/
```

Dockerfile、設定ファイル、Web Viewer本体および起動スクリプトを含みます。
