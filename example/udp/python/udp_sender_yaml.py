#!/usr/bin/env python3

import argparse
import socket
import struct
import yaml


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


# --------------------------------
# YAMLフォーマット読込
# --------------------------------
def load_format_yaml(path):
    with open(path, "r") as f:
        return yaml.safe_load(f)


# --------------------------------
# YAMLデータ読込
# --------------------------------
def load_value_yaml(path):
    with open(path, "r") as f:
        return yaml.safe_load(f)


# --------------------------------
# pack
# --------------------------------
def pack_scalar(field_type, value):
    return struct.pack(TYPE_MAP[field_type], int(value))


# --------------------------------
# item生成
# --------------------------------
def encode_item(item_def, item_data):

    packet = b""

    fields = item_def["fields"]

    for name, spec in fields.items():

        #
        # scalar
        #
        if isinstance(spec, str):

            value = item_data[name]

            packet += pack_scalar(
                spec,
                value
            )

        #
        # array
        #
        elif isinstance(spec, dict):

            elem_type = spec["type"].replace("[]", "")

            values = item_data[name]

            for v in values:

                packet += pack_scalar(
                    elem_type,
                    v
                )

    return packet


# --------------------------------
# packet生成
# --------------------------------
def build_packet(fmt_yaml, value_yaml):

    packet = b""

    header_def = fmt_yaml["message"]["header"]["fields"]

    header_data = value_yaml["header"]

    #
    # header
    #
    for name, field_type in header_def.items():

        packet += pack_scalar(
            field_type,
            header_data[name]
        )

    #
    # body
    #
    item_def = fmt_yaml["message"]["body"]["item"]

    body_data = value_yaml["body"]

    data_count = header_data["data_count"]

    if len(body_data) != data_count:
        raise ValueError(
            f"data_count={data_count}, body size={len(body_data)}"
        )

    for item in body_data:

        packet += encode_item(
            item_def,
            item
        )

    return packet


# --------------------------------
# main
# --------------------------------
def main():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--ip",
        required=True
    )

    parser.add_argument(
        "--port",
        type=int,
        required=True
    )

    parser.add_argument(
        "--format",
        required=True,
        help="format yaml"
    )

    parser.add_argument(
        "--value",
        required=True,
        help="value yaml"
    )

    args = parser.parse_args()

    fmt_yaml = load_format_yaml(
        args.format
    )

    value_yaml = load_value_yaml(
        args.value
    )

    packet = build_packet(
        fmt_yaml,
        value_yaml
    )

    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_DGRAM
    )

    sock.sendto(
        packet,
        (args.ip, args.port)
    )

    print(
        f"sent {len(packet)} bytes "
        f"to {args.ip}:{args.port}"
    )

    sock.close()


if __name__ == "__main__":
    main()