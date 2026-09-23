# GHCRで公開しているDockerイメージを利用する

---

## 動作確認環境

- Docker, k3sが利用可能な環境

- 開発者は、主にUbuntuやWindows上のWSL2環境で動作を確認しています。

## GHCRで公開しているパッケージ

dm2では、主要なコンポーネントについてGitHub Container Registry (GHCR) にDockerイメージを公開しています。

| コンポーネント | GHCRパッケージ |
| ------- | --------- |
| MES (DMメッセンジャー)    | `dm2-mes` |
| IS (DBシステム)      | `dm2-is`  |
| RDB (IS内で利用するRDB)    | `dm2-rdb` |
| CS (通信部)     | `dm2-cs`  |

## Dockerイメージの取得方法

以下の取得方法があります。

* **A-1. GitHubリポジトリの`deploy/k3s/components`にあるk3s用マニフェストを利用する**
* **A-2. docker pullコマンドで取得する**

### A-1. GitHubリポジトリの`deploy/k3s/components`にあるk3s用マニフェストを利用する

公式ドキュメントに従って、k3sをインストールします。

インストール後、下記コマンドでPodを生成します。`PROJECT_DIR`は、適宜、書き換えて下さい。

```bash
sudo mkdir -p /var/dm2
sudo mkdir -p /var/dm2/data
sudo cp -r ${PROJECT_DIR}/deploy/k3s/conf /var/dm2/
cd ${PROJECT_DIR}/dm20/deploy/k3s/components
kubectl apply -f .
```

各コンポーネントのPodが起動したことを確認します。

```bash
kubectl get pods
```

IS, CS-recv, CS-send, MES, RDBのPodが起動できていれば成功です。

```
NAME             READY   STATUS    RESTARTS   AGE
is-xxxxx         1/1     Running   1          xxxx
cs-recv-xxxxx    1/1     Running   1          xxxx
cs-send-xxxxx    1/1     Running   1          xxxx
mes-xxxxx        1/1     Running   1          xxxx
rdb-xxxxx        1/1     Running   1          xxxx
```

サービスの状態は以下で確認できます。

```bash
kubectl get svc
```

IS、CS-recv、cs-sendは、k3s内で常駐するため、[dm2の実行例](../../example/command/README.md)に記載されているIS・CSに関わるコマンドの起動は不要となります。

`dm2mes`に関するコマンドのみ、下記の通り、Podにログインし、Pod内で、用途に応じた引数を指定して使用することになります。用途に応じてマニフェストをご自由に編集して下さい。

```bash
kubectl exec -it deployment/mes -- bash
```

具体的な実行例については、[dm2の実行例](../../example/command/README.md)を参照してください。

### A-2. docker pullコマンドで取得する

公式ドキュメントに従って、dockerをインストールします。

インストール後、下記コマンドでDockerイメージを取得します。

```bash
docker pull ghcr.io/dm20-consortium/dm2-cs:v1.2.0
docker pull ghcr.io/dm20-consortium/dm2-is:v1.2.0
docker pull ghcr.io/dm20-consortium/dm2-rdb:v1.2.0
docker pull ghcr.io/dm20-consortium/dm2-mes:v1.2.0
```

- イメージ構築後は、docker runコマンドを使って試すことができますが、[dm2の実行例](../../example/README.md)はホスト上でソースコードからビルドした後のコマンド例となるため、~/.bashrcに下記の関数を登録しておくことで、差異を無くす事ができます。`-v`は、「コンテナ間で設定ファイルやログ、FDファイルを共有するためのオプション」です。`PROJECT_DIR`は、適宜、書き換えて下さい。

```bash
PROJECT_DIR=~/dm20
function dm2cs_send () {
  docker run -it --init --rm --net host --name cs_send -v ${PROJECT_DIR}/dm2/conf:/tmp/conf ghcr.io/dm20-consortium/dm2-cs:v1.2.0 dm2cs_send -d /tmp/conf;
}
function dm2cs_recv () {
  docker run -it --init --rm --net host --name cs_recv -v ${PROJECT_DIR}/dm2/conf:/tmp/conf ghcr.io/dm20-consortium/dm2-cs:v1.2.0 dm2cs_recv -d /tmp/conf;
}
function dm2is () {
  docker run --init --rm --net host --name rdb -e POSTGRES_PASSWORD=postgres ghcr.io/dm20-consortium/dm2-rdb:v1.2.0 > /dev/null 2>&1 &
  docker run -it --init --rm --net host --name is -v ${PROJECT_DIR}/dm2/conf:/tmp/conf ghcr.io/dm20-consortium/dm2-is:v1.2.0 dm2is -d /tmp/conf
}
function dm2mes () {
  docker run -i --rm --net host ghcr.io/dm20-consortium/dm2-mes:v1.2.0 stdbuf -oL dm2mes "$@";
}
```
