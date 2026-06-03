import argparse
import csv
import random
import socket
import struct
import time
import yaml

from scapy.layers.inet import IP, UDP


# --------------------------
# 型定義
# --------------------------
TYPE_MAP = {
    "uint8":  "!B",
    "int8":   "!b",
    "uint16": "!H",
    "int16":  "!h",
    "uint32": "!I",
    "int32":  "!i",
    "uint64": "!Q",
    "int64":  "!q",
}


# --------------------------
# フォーマットyaml読み込み
# --------------------------
def load_format_yaml(path):
    with open(path, "r") as f:
        y = yaml.safe_load(f)

    header_fields = list(
        y["message"]["header"]["fields"].items()
    )

    data_fields = list(
        y["message"]["body"]["item"]["fields"].items()
    )

    return header_fields, data_fields


# --------------------------
# 値CSV読み込み
# --------------------------
def load_value_csv(path):
    with open(path) as f:
        return list(csv.DictReader(f))


# --------------------------
# 値生成
# --------------------------
def generate_value(field_type):
    if field_type.startswith("uint"):
        bits = int(field_type.replace("uint", ""))
        return random.randint(0, 2**bits - 1)
    elif field_type.startswith("int"):
        bits = int(field_type.replace("int", ""))
        return random.randint(-(2**(bits-1)), 2**(bits-1)-1)
    return 0


# --------------------------
# pack
# --------------------------
def pack_value(field_type, value):
    return struct.pack(TYPE_MAP[field_type], int(value))

# --------------------------
# 上限値防止
# --------------------------
def safe_sample_value(counter, ftype):
    if ftype == "uint8":
        return counter % 256

    if ftype == "int8":
        return (counter % 256) - 128
    return counter

# --------------------------
# サンプルrows生成
# --------------------------
def create_sample_row(header_fields, data_fields, value):
    row = {}

    # ヘッダ
    for name, ftype in header_fields:
        if name == "data_count":
            row[name] = 1
        else:
            row[name] = safe_sample_value(value, ftype)

    # data_count=1固定
    for name, ftype in data_fields:
        row[f"{name}_0"] = safe_sample_value(value, ftype)

    return row

# --------------------------
# パケット生成
# --------------------------
def build_packet(header_fields, data_fields, row):
    packet = b''
    header_values = {}

    # ヘッダ
    for name, ftype in header_fields:
        val = int(row[name])

        print(name + ":" + str(val))
        header_values[name] = val
        packet += pack_value(ftype, val)

    data_count = header_values["data_count"]
    # データ部
    for i in range(data_count):
        for name, ftype in data_fields:
            key = f"{name}_{i}" if f"{name}_{i}" in row else name
            val = int(row.get(key, 0))
            print(str(i) + ":" + name + ":" + str(val))
            packet += pack_value(ftype, val)
    return packet


# --------------------------
# main
# --------------------------
def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--ip", required=True)
    parser.add_argument("--port", type=int, required=True)
    parser.add_argument("--format", required=True)

    parser.add_argument("--mode", choices=["sample", "csv"], required=True)
    parser.add_argument("--value_csv")

    parser.add_argument("--interval", type=float, default=1.0)

    args = parser.parse_args()

    header_fields, data_fields = load_format_yaml(args.format)

    # UDPソケット
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    count = 0

    try:
        sock.connect((args.ip, args.port))
        while True:
            if args.mode == "csv":
                rows = load_value_csv(args.value_csv)
            else:
                rows = [create_sample_row(header_fields, data_fields, count)]

            for row in rows:
                packet = build_packet(header_fields, data_fields, row)
                sock.sendall(struct.pack("!I", len(packet)))
                sock.sendall(packet)
                count += 1
                print(f"sent: {count}", end="\r")
                time.sleep(args.interval)

            if args.mode == "sample":
                continue
            break

    except KeyboardInterrupt:
        print("\nInterrupted by user")

    except ConnectionRefusedError:
        print(f'Connection Refused: {args.ip}, {args.port}')

    except:
        print(f'Connection Error: {args.ip}, {args.port}')

    finally:
        if sock:
            sock.close()
        print(f"\nFinished. total packets: {count}")


if __name__ == "__main__":
    main()