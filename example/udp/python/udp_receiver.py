import argparse
import csv
import socket
import struct
import yaml

# --------------------------
# 型定義
# --------------------------
TYPE_MAP = {
    "uint8":  ("!B", 1),
    "int8":   ("!b", 1),
    "uint16": ("!H", 2),
    "int16":  ("!h", 2),
    "uint32": ("!I", 4),
    "int32":  ("!i", 4),
    "uint64": ("!Q", 8),
    "int64":  ("!q", 8),
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

    max_count = y["message"]["body"].get(
        "max_count", 1
    )
    return header_fields, data_fields, max_count


# --------------------------
# デコード
# --------------------------
def decode_packet(packet, header_fields, data_fields):
    offset = 0
    result = {}

    # ---- header ----
    for name, ftype in header_fields:
        fmt, size = TYPE_MAP[ftype]
        value = struct.unpack_from(fmt, packet, offset)[0]
        print("name:" + name + " / value: " + str(value))
        result[name] = value
        offset += size

    data_count = result["data_count"]

    # ---- data ----
    for i in range(data_count):
        for name, ftype in data_fields:
            fmt, size = TYPE_MAP[ftype]
            value = struct.unpack_from(fmt, packet, offset)[0]
            result[f"{name}_{i}"] = value
            print(str(i) + ":name:" + name + " / value: " + str(value))
            offset += size

    return result


# --------------------------
# CSVヘッダ生成
# --------------------------
def build_csv_header(header_fields, data_fields, max_count):
    header = [name for name, _ in header_fields]

    for i in range(max_count):
        for name, _ in data_fields:
            header.append(f"{name}_{i}")

    return header


# --------------------------
# main
# --------------------------
def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--port", type=int, required=True)
    parser.add_argument("--format", required=True)
    parser.add_argument("--output_csv", required=True)

    parser.add_argument("--max_count", type=int, default=0,
                        help="max data_count for CSV columns")

    args = parser.parse_args()

    header_fields, data_fields, max_count = load_format_yaml(args.format)
    if args.max_count > 0:
        max_count = args.max_count
    
    # UDP受信
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", args.port))

    # CSV準備
    csv_header = build_csv_header(header_fields, data_fields, max_count)

    with open(args.output_csv, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=csv_header)
        writer.writeheader()

        print(f"Listening on UDP port {args.port}...")
        sock.settimeout(1.0)

        try:
            while True:
                try:
                    packet, addr = sock.recvfrom(65535)

                except socket.timeout:
                    continue

                decoded = decode_packet(packet, header_fields, data_fields)
                writer.writerow(decoded)
                print(f"received from {addr}, data_count={decoded['data_count']}")
                f.flush()

        except KeyboardInterrupt:
            print("\nStopped by user")

        finally:
            sock.close()


if __name__ == "__main__":
    main()