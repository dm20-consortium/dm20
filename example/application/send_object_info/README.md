# 物標情報送信サンプルプログラム

本サンプルは、DM2.0 Platform の DBシステム に対して物標情報 (`object_info_0_8_1`) を送信する、アプリケーション開発者向けのサンプルプログラムです。

一定周期で Tuple データを生成し、DM2.0 のストリームデータとして送信します。

---

# サンプルコード

```cpp
#include <is/DmManager.h>

#include <iostream>
#include <string>
#include <time.h>
#include <thread>

using namespace std;

int main(int argc, const char* argv[])
{
    string ip = "127.0.0.1";
    int port = 9002;
    string dbUser = "dm2sampleuser";
    string dbPass = "dm2samplepassword";
    string stream_name ="object_info_0_8_1";

    // Tuple の列数
    int tuple_col_size = 88;

    // 送信間隔(ms)
    int sleepMillis = 1000;

    // UDP送信オブジェクトの取得
    DatagramSocket* sendSock =
        DmManager::getDatagramSocket(
            ip,
            port,
            dbUser,
            dbPass);

    int value = 0;

    while (true)
    {
        value++;

        vector<Tuple> tuples;

        Tuple tuple(tuple_col_size);

        long now = DmUtil::getTimeMillisec();

        // 全列へ値を設定
        for (int i = 0; i < tuple_col_size; i++)
        {
            tuple.setValue(
                i,
                to_string(value),
                now);
        }

        tuples.push_back(tuple);

        // ストリームデータ送信
        sendSock->sendStreamData(
            stream_name,
            tuples);

        // 周期待ち
        this_thread::sleep_for(
            chrono::milliseconds(sleepMillis));
    }

    delete sendSock;

    return 0;
}
```

---

# プログラム概要

本プログラムは以下の流れで動作します。

1. UDP送信ソケット生成
2. Tuple データ生成
3. Tuple へ値を設定
4. DM2.0 ストリームデータとして送信
5. 一定時間待機
6. 繰り返し送信

---

# 設定パラメータ

| パラメータ | 説明 | 設定値 | 補足 |
|---|---|---|---|
| `ip` | DM2サーバのIPアドレス | 127.0.0.1 | ローカル端末以外を指定する事も可能です |
| `port` | UDP受信ポート番号 | 9002 | 固定値になります |
| `dbUser` | 接続ユーザ名 | dm2sampleuser | dm2インストール時に設定する値です |
| `dbPass` | 接続パスワード | dm2samplepassword | dm2インストール時に設定する値です |
| `stream_name` | 送信対象ストリーム名 | object_info_0_8_1 | 別のストリームに変更したい場合は、[こちら](../../../dm2/conf/schema/)から選択可能です。新たに追加することもできます |
| `tuple_col_size` | 物標情報のTuple列数 | 88 | 送信対象ストリーム名の列数をセットします。列定義は[こちら](../../../dm2/conf/schema/object_info_0_8_1.xml) |
| `sleepMillis` | データ送信周期(ms) | 1000 | 例）LiDARの送信周期を擬似的に再現したい場合は、100 |

---

# 主な処理説明

## DatagramSocket の生成

```cpp
DatagramSocket* sendSock =
    DmManager::getDatagramSocket(
        ip,
        port,
        dbUser,
        dbPass);
```

DBシステム へ UDP データ送信を行うためのソケットを生成します。

---

## Tuple の生成

```cpp
Tuple tuple(tuple_col_size);
```

Tuple オブジェクトを生成します。

`tuple_col_size` には送信対象ストリーム名の列数を指定します。

---

## Tuple への値設定

```cpp
tuple.setValue(
    i,
    to_string(value),
    now);
```

Tuple の各列へ値を設定します。

本サンプルでは、全列へ同一値を設定しています。

```text
1,1,1,1,1,1,1,...
```

サンプルにつき、`to_string(value)`で文字列変換しています。

通常は、[列定義](../../../dm2/conf/schema/object_info_0_8_1.xml)に合わせたC++の型を設定します。any型につき、型変換不要で追加する事が可能です。

| 列定義の<type>での表記 | C++の型 |
|---|---|
| `int` | `int` |
| `long` | `long` |
| `double` | `double` |
| `string` | `string` |
| `ulong` | `unsigned long long` |
| `vector(int)` | `vector <int>` |
| `vector(ulong)` | `vector <unsigned long long>` |
| 他、省略 | |

---

## ストリームデータ送信

```cpp
sendSock->sendStreamData(
    stream_name,
    tuples);
```

生成した Tuples を DM2.0の ストリームデータとして送信します。

---

# 実行イメージ

本サンプルでは、以下のようなデータが周期送信されます。

| 送信回数 | 設定値 |
|---|---|
| 1回目 | 1 |
| 2回目 | 2 |
| 3回目 | 3 |

各 Tuple の全列に同一値が設定されます。

---

# カスタマイズ例

## 送信周期変更

```cpp
int sleepMillis = 100;
```

100ms周期送信へ変更できます。

## タプル数変更

```cpp
tuples.push_back(tuple);
```

本サンプルでは、1行のみpush_backしていますが、複数のtupleをpush_backする事も可能です。

例えば、LiDAR等で100ミリ秒で検出した数だけ、push_backさせて、受信側のクエリで、`rows 50`と指定すれば、100ミリ秒間で最大50件受信する流れになります。

---

# API仕様書

各クラス・メソッドの詳細については、以下の仕様書を参照してください。

- [DBシステムのAPI仕様（DMライブラリ）](https://www.nces.i.nagoya-u.ac.jp/dm2/DMLib_API.pdf)

主に以下の API を利用しています。

| クラス | 主なメソッド |
|---|---|
| `DmManager` | `getDatagramSocket()` |
| `DatagramSocket` | `sendStreamData()` |
| `Tuple` | `setValue()` |
| `DmUtil` | `getTimeMillisec()` |

---

# 注意事項

- 本サンプルは無限ループで送信を行います。
- 停止する場合は `Ctrl + C` を実行してください。
- Tuple列数とストリーム定義が一致している必要があります。
- 実運用時は適切な送信周期を設定してください。
```