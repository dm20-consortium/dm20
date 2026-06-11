#!/usr/bin/env python3

import argparse
import struct
import sys
import yaml

from scapy.utils import rdpcap
from scapy.layers.inet import UDP


TYPE_MAP = {
    "uint8": ("!B", 1),
    "int8": ("!b", 1),
    "uint16": ("!H", 2),
    "int16": ("!h", 2),
    "uint32": ("!I", 4),
    "int32": ("!i", 4),
    "uint64": ("!Q", 8),
    "int64": ("!q", 8),
}


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


def decode_payload(payload, header_fields, data_fields):

    offset = 0
    result = {}

    #
    # header
    #
    for name, ftype in header_fields:

        if ftype not in TYPE_MAP:
            raise ValueError(
                f"unsupported type '{ftype}'"
            )

        fmt, size = TYPE_MAP[ftype]

        if offset + size > len(payload):
            raise ValueError(
                f"header field '{name}' exceeds payload size"
            )

        value = struct.unpack_from(
            fmt,
            payload,
            offset
        )[0]

        result[name] = value
        offset += size

    if "data_count" not in result:
        raise ValueError(
            "data_count not found in header"
        )

    data_count = result["data_count"]

    result["objects"] = []

    #
    # body
    #
    for i in range(data_count):

        obj = {}

        for name, ftype in data_fields:

            if ftype not in TYPE_MAP:
                raise ValueError(
                    f"unsupported type '{ftype}'"
                )

            fmt, size = TYPE_MAP[ftype]

            if offset + size > len(payload):
                raise ValueError(
                    f"object[{i}] field '{name}' exceeds payload size"
                )

            value = struct.unpack_from(
                fmt,
                payload,
                offset
            )[0]

            obj[name] = value
            offset += size

        result["objects"].append(obj)

    #
    # 残りデータ確認
    #
    if offset != len(payload):
        raise ValueError(
            f"decoded={offset} bytes "
            f"but payload={len(payload)} bytes "
            f"(remaining={len(payload)-offset})"
        )

    return result


def print_result(index, decoded):

    print("=" * 80)
    print(f"PACKET {index}")

    for k, v in decoded.items():

        if k == "objects":
            continue

        print(f"{k}: {v}")

    for idx, obj in enumerate(decoded["objects"]):

        print(f"\nOBJECT[{idx}]")

        for k, v in obj.items():
            print(f"  {k}: {v}")


def main():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--pcap",
        required=True
    )

    parser.add_argument(
        "--format",
        required=True
    )

    parser.add_argument(
        "--udp-port",
        type=int,
        default=None
    )

    args = parser.parse_args()

    header_fields, data_fields = load_format_yaml(
        args.format
    )

    packets = rdpcap(args.pcap)

    success = 0
    failed = 0

    for index, pkt in enumerate(packets, start=1):

        try:

            if UDP not in pkt:
                continue

            udp = pkt[UDP]

            if (
                args.udp_port is not None
                and udp.dport != args.udp_port
                and udp.sport != args.udp_port
            ):
                continue

            payload = bytes(udp.payload)

            decoded = decode_payload(
                payload,
                header_fields,
                data_fields
            )

            print_result(index, decoded)

            success += 1

        except Exception as e:

            failed += 1

            print(
                f"[ERROR] packet={index}: {e}",
                file=sys.stderr
            )

    print(
        f"\nsummary: success={success}, failed={failed}",
        file=sys.stderr
    )


if __name__ == "__main__":
    main()