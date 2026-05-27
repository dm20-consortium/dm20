---

# 物標情報受信サンプルプログラム

本サンプルは、DM2.0 Platform の DBシステム に登録されたストリームデータを受信し、受信データを表示するアプリケーション開発者向けサンプルです。

`object_info_0_8_1` ストリームに対してクエリを登録し、受信データをコールバック関数で処理します。

---

# サンプルコード

```cpp
#include <is/DmManager.h>

#include <iostream>
#include <string>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

volatile sig_atomic_t eflag = 0;

// シグナルハンドラ
void handler(int signum)
{
    eflag = 1;
}

class CallBackListenerClass : public CallBackListener
{
public:

    // データ受信時に呼ばれる
    void callBack(ResultSet rs)
    {
        ResultSetMetaData rsmd =
            rs.getResultSetMetaData();

        int colNum = rsmd.getColumnSize();

        while (rs.next())
        {
            string line = "";

            // 受信データ表示
            for (int i = 0; i < colNum; i++)
            {
                if (i != 0)
                {
                    line += ",";
                }

                line += rs.getString(i);
            }

            cout << line << endl;
        }
    }
};

int main(int argc, const char* argv[])
{
    // Ctrl+C(SIGINT) ハンドラ登録
    if (signal(SIGINT, handler) == SIG_ERR)
    {
        cerr << "Signal Error" << endl;
        return -1;
    }

    string ip = "127.0.0.1";
    int port = 9001;

    string dbUser = "dm2sampleuser";
    string dbPass = "dm2samplepassword";

    // 実行Query
    string query =
        "MASTER object_info_0_8_1 "
        "SELECT * FROM object_info_0_8_1 [rows 1]";

    // サーバ接続
    Connection* con =
        DmManager::getDBConnection(
            ip,
            port,
            dbUser,
            dbPass);

    unsigned int mngId = 0;

    try
    {
        CallBackListenerClass* cblc =
            new CallBackListenerClass();

        // Query登録
        mngId =
            con->registerQuery(
                query,
                cblc);
    }
    catch (SQLException &e)
    {
        cout << "ERROR catch:"
             << e.getMessage()
             << endl;
    }

    // 待機ループ
    while (true)
    {
        sleep(1);

        if (eflag == 1)
        {
            break;
        }
    }

    // Query解除
    con->cancelQuery(mngId);

    // 切断
    con->disconnect();

    return 0;
}
```

---

# プログラム概要

本プログラムは以下の流れで動作します。

1. DM2.0 DBシステムへ接続
2. クエリ登録
3. ストリームデータ受信
4. コールバック関数呼び出し
5. 受信データ表示
6. Ctrl+C により終了
7. Query解除
8. 接続切断

---

# 設定パラメータ

| パラメータ | 説明 | 設定値 | 補足 |
|---|---|---|---|
| `ip` | DM2サーバのIPアドレス | 127.0.0.1 | ローカル端末以外を指定する事も可能です |
| `port` | UDP受信ポート番号 | 9002 | 固定値になります |
| `dbUser` | 接続ユーザ名 | dm2sampleuser | dm2インストール時に設定する値です |
| `dbPass` | 接続パスワード | dm2samplepassword | dm2インストール時に設定する値です |
| `query` | 実行するDM2.0のクエリ | 下記参照 |  |

---

# クエリ説明

本サンプルでは、以下のクエリを実行しています。

```sql
MASTER object_info_0_8_1
SELECT * FROM object_info_0_8_1 [rows 1]
```

## クエリ内容

| 構文 | 説明 |
|---|---|
| `MASTER object_info_0_8_1` | 入力ストリーム指定 |
| `SELECT *` | 全列取得 |
| `[rows 1]` | 1件ずつ処理 |

---

# 主な処理説明

## サーバ接続

```cpp
Connection* con =
    DmManager::getDBConnection(
        ip,
        port,
        dbUser,
        dbPass);
```

DBシステムへ接続します。

---

## クエリ登録

```cpp
mngId =
    con->registerQuery(
        query,
        cblc);
```

クエリを登録します。

登録後、ストリームデータ受信時にコールバック関数が呼ばれます。

---

## コールバック関数

```cpp
void callBack(ResultSet rs)
```

データ受信時に自動実行されます。

---

## ResultSet からデータ取得

```cpp
rs.getString(i);
```

受信データを文字列として取得します。

---

## クエリ解除

```cpp
con->cancelQuery(mngId);
```

登録済みクエリを解除します。

---

## 接続切断

```cpp
con->disconnect();
```

DM2 サーバとの接続を切断します。

---

# 実行イメージ

送信プログラムと同時実行すると、以下のように受信データが表示されます。

```text
1,1,1,1,1,1,1,...
2,2,2,2,2,2,2,...
3,3,3,3,3,3,3,...
```

---

# 終了方法

Ctrl + C を入力してください。

SIGINT を受信すると Query を解除し、接続を切断して終了します。

---

# カスタマイズ例

## コールバック処理の標準出力をCSV形式保存に置き換える

```cpp
ofstream ofs("output.csv", ios::app);
ofs << line << endl;
```

---

## 別のクエリに置き換える。

```cpp
string query =
    "MASTER object_info_0_8_1 "
    "SELECT * FROM object_info_0_8_1 [rows 1]";
```

クエリ言語仕様を理解すれば置き換え可能です。クエリ言語仕様については、[こちら](https://www.nces.i.nagoya-u.ac.jp/admobi-dm2/images/dm2_query_language_specification_20260327.pdf)


---

# API仕様書

各クラス・メソッドの詳細については、以下の仕様書を参照してください。

- [DBシステムのAPI仕様（DMライブラリ）](https://www.nces.i.nagoya-u.ac.jp/dm2/DMLib_API.pdf)

主に以下の API を利用しています。

| クラス | 主なメソッド |
|---|---|
| `DmManager` | `getDBConnection()` |
| `Connection` | `registerQuery()` / `cancelQuery()` / `disconnect()` |
| `ResultSet` | `next()` / `getString()` |
| `ResultSetMetaData` | `getColumnSize()` |
| `CallBackListener` | `callBack()` |

---

# 注意事項

- クエリ登録中はコールバック関数が継続的に呼ばれます。
- コールバック内で重い処理を行う場合は性能へ注意してください。
- Query解除を行わず終了すると、セッションが残る場合があります。
- 実運用では例外処理を追加してください。