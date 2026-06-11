#!/usr/bin/env python3

import argparse
import csv
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

    max_count = y["message"]["body"].get(
        "max_count",
        1
    )

    return header_fields, data_fields, max_count


def unpack_value(packet, offset, field_type):

    fmt, size = TYPE_MAP[field_type]

    if offset + size > len(packet):
        raise ValueError(
            f"packet too short "
            f"(offset={offset}, need={size}, len={len(packet)})"
        )

    value = struct.unpack_from(
        fmt,
        packet,
        offset
    )[0]

    return value, size


def decode_packet(packet, header_fields, data_fields):

    result = {}
    offset = 0

    #
    # Header
    #
    for name, field_type in header_fields:

        value, size = unpack_value(
            packet,
            offset,
            field_type
        )

        result[name] = value
        offset += size

    data_count = result["data_count"]

    #
    # Body
    #
    for i in range(data_count):

        for name, field_type in data_fields:

            value, size = unpack_value(
                packet,
                offset,
                field_type
            )

            result[f"{name}_{i}"] = value
            offset += size

    return result


def build_csv_header(
    header_fields,
    data_fields,
    max_count
):

    header = []

    for name, _ in header_fields:
        header.append(name)

    for i in range(max_count):

        for name, _ in data_fields:
            header.append(
                f"{name}_{i}"
            )

    return header


def print_decoded(decoded):

    print("========================================")

    for k, v in decoded.items():
        print(f"{k}: {v}")

    print("========================================")


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
        "--csv"
    )

    parser.add_argument(
        "--udp_port",
        type=int
    )

    args = parser.parse_args()

    header_fields, data_fields, max_count = \
        load_format_yaml(args.format)

    csv_writer = None
    csv_file = None

    if args.csv:

        csv_header = build_csv_header(
            header_fields,
            data_fields,
            max_count
        )

        csv_file = open(
            args.csv,
            "w",
            newline=""
        )

        csv_writer = csv.DictWriter(
            csv_file,
            fieldnames=csv_header
        )

        csv_writer.writeheader()

    packets = rdpcap(args.pcap)

    total = 0
    success = 0
    failed = 0

    for idx, pkt in enumerate(packets):

        if UDP not in pkt:
            continue

        udp = pkt[UDP]

        if args.udp_port:

            if udp.dport != args.udp_port and \
               udp.sport != args.udp_port:
                continue

        payload = bytes(udp.payload)

        total += 1

        try:

            decoded = decode_packet(
                payload,
                header_fields,
                data_fields
            )

            print_decoded(decoded)

            if csv_writer:
                csv_writer.writerow(decoded)

            success += 1

        except Exception as e:

            failed += 1

            print(
                f"[ERROR] packet #{idx} decode failed: {e}",
                file=sys.stderr
            )

    if csv_file:
        csv_file.close()

    print()
    print("========== Summary ==========")
    print(f"UDP Packets : {total}")
    print(f"Success     : {success}")
    print(f"Failed      : {failed}")


if __name__ == "__main__":
    main()