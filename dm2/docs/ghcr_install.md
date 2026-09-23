# GHCRで公開しているDockerイメージを利用する

---

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

サービスの状態は以下で確認できます。

```bash
kubectl get services
```

k3s環境では、MES用のPodを起動しただけでは`dm2mes`プロセスは起動しません。

MESのPodにログインし、Pod内で、用途に応じて`dm2mes`を送信モードまたは受信モードで起動して使用します。

```bash
kubectl exec -it deployment/mes -- bash
```

具体的な実行例については、[こちら](../../example/command/01_dm2is_to_dm2mes/README.md)を参照してください。

### A-2. docker pullコマンドで取得する

公式ドキュメントに従って、dockerをインストールします。

インストール後、下記コマンドでDockerイメージを取得します。

```bash
docker pull ghcr.io/dm20-consortium/dm2-cs:v1.2.0
docker pull ghcr.io/dm20-consortium/dm2-is:v1.2.0
docker pull ghcr.io/dm20-consortium/dm2-rdb:v1.2.0
docker pull ghcr.io/dm20-consortium/dm2-mes:v1.2.0
```

- イメージ構築後は、docker runコマンドを使って試すことができますが、[使用例](../../example/README.md)は手動インストール後のコマンド例となるため、~/.bashrcに下記の関数を登録しておくことで、手動インストールとの差異を無くす事ができます。`-v`は、「コンテナ間で設定ファイルやログ、FDファイルを共有するためのオプション」です。`PROJECT_DIR`は、適宜、書き換えて下さい。

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
