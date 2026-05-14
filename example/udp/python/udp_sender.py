import argparse
import csv
import random
import socket
import struct
import time

from scapy.all import IP, UDP
from scapy.utils import PcapWriter


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
# フォーマットCSV読み込み
# --------------------------
def load_format_csv(path):
    with open(path) as f:
        reader = list(csv.reader(f))

    header = reader[:7]
    data = reader[7:]

    header_fields = [(r[0], r[1]) for r in header]
    data_fields = [(r[0], r[1]) for r in data]

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
# パケット生成
# --------------------------
def build_packet(header_fields, data_fields, mode, row=None):
    packet = b''
    header_values = {}

    # ヘッダ
    for name, ftype in header_fields:
        for name, ftype in header_fields:
            if mode == "csv":
                val = int(row[name])
            elif name == "data_count":
                val = random.randint(1, 5)
            elif mode == "ones":
                val = 1
            else:
                val = generate_value(ftype)

        header_values[name] = val
        packet += pack_value(ftype, val)

    data_count = header_values["data_count"]
    # データ部
    for i in range(data_count):
        for name, ftype in data_fields:
            if mode == "csv":
                key = f"{name}_{i}" if f"{name}_{i}" in row else name
                val = int(row.get(key, 0))
            elif mode == "ones":
                val = 1
            else:
                val = generate_value(ftype)
            print(str(i) + ":" + name + ":" + str(val))
            packet += pack_value(ftype, val)
    return packet


# --------------------------
# 出力処理
# --------------------------
def process_packet(packet, args, sock=None, pcap_writer=None):
    # UDP送信
    if args.output in ["udp", "both"]:
        sock.sendto(packet, (args.ip, args.port))

    # PCAP書き込み（逐次）
    if args.output in ["pcap", "both"]:
        pkt = IP(dst=args.ip) / UDP(dport=args.port) / packet
        pcap_writer.write(pkt)


# --------------------------
# main
# --------------------------
def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--ip", required=True)
    parser.add_argument("--port", type=int, required=True)
    parser.add_argument("--format", required=True)

    parser.add_argument("--mode", choices=["random", "csv", "ones"], required=True)
    parser.add_argument("--value_csv")

    parser.add_argument("--interval", type=float, default=1.0)

    parser.add_argument("--output", choices=["udp", "pcap", "both"], required=True)
    parser.add_argument("--pcap", help="pcap output file")

    parser.add_argument("--count", type=int, default=0,
                        help="number of packets (0=infinite)")

    args = parser.parse_args()

    header_fields, data_fields = load_format_csv(args.format)

    # UDPソケット
    sock = None
    if args.output in ["udp", "both"]:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # PCAP
    pcap_writer = None
    if args.output in ["pcap", "both"]:
        if not args.pcap:
            raise ValueError("--pcap is required for pcap output")
        pcap_writer = PcapWriter(args.pcap, append=False, sync=True)

    count = 0

    try:
        if args.mode == "csv":
            rows = load_value_csv(args.value_csv)

            for row in rows:
                packet = build_packet(header_fields, data_fields, "csv", row)
                process_packet(packet, args, sock, pcap_writer)

                count += 1
                print(f"sent: {count}", end="\r")

                if args.count > 0 and count >= args.count:
                    break

        else:
            while True:
                packet = build_packet(header_fields, data_fields, args.mode)
                process_packet(packet, args, sock, pcap_writer)

                count += 1
                print(f"sent: {count}", end="\r")

                if args.count > 0 and count >= args.count:
                    break

                time.sleep(args.interval)

    except KeyboardInterrupt:
        print("\nInterrupted by user")

    finally:
        if sock:
            sock.close()
        if pcap_writer:
            pcap_writer.close()
        print(f"\nFinished. total packets: {count}")


if __name__ == "__main__":
    main()