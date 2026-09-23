# B. Dockerイメージを構築して利用する

---

## 動作確認環境

- Dockerが利用可能な環境

- 開発者は、主にUbuntuやWindows上のWSL2環境で動作を確認しています。

## Dockerイメージの構築

- 公式ドキュメントに従って、dockerをインストールします。

- インストール後、リポジトリのルートディレクトリ/dm2上で下記のコマンドを実行して下さい。

```bash
make without_dmi -f makefile_docker
```

- DMI、dm2両方のDockerイメージを構築したい場合は、リポジトリのルートディレクトリ上で下記のコマンドを実行して下さい。約 20GB 程の空き容量が必要です。

```bash
bash build.bash
```

- イメージ構築後は、docker runコマンドを使って試すことができますが、[使用例](../example/README.md)は手動インストール後のコマンド例となるため、~/.bashrcに下記の関数を登録しておくことで、手動インストールとの差異を無くす事ができます。`-v`は、「コンテナ間で設定ファイルやログ、FDファイルを共有するためのオプション」です。`PROJECT_DIR`は、適宜、書き換えて下さい。

```bash
PROJECT_DIR=~/dm20
function dm2cs_send () {
  docker run -it --init --rm --net host --name cs_send -v ${PROJECT_DIR}/dm2/conf:/tmp/conf dm2/cs:20.04 dm2cs_send -d /tmp/conf;
}
function dm2cs_recv () {
  docker run -it --init --rm --net host --name cs_recv -v ${PROJECT_DIR}/dm2/conf:/tmp/conf dm2/cs:20.04 dm2cs_recv -d /tmp/conf;
}
function dm2is () {
  docker run --init --rm --net host --name rdb -e POSTGRES_PASSWORD=postgres dm2/rdb:20.04 > /dev/null 2>&1 &
  docker run -it --init --rm --net host --name is -v ${PROJECT_DIR}/dm2/conf:/tmp/conf dm2/is:20.04 dm2is -d /tmp/conf
}
function dm2mes () {
  docker run -i --rm --net host dm2/dm2mes:20.04 stdbuf -oL dm2mes "$@";
}
```

