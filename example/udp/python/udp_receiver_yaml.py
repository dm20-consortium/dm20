#!/usr/bin/env python3

import argparse
import socket
import struct
import yaml


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


# --------------------------------
# YAMLフォーマット読込
# --------------------------------
def load_format_yaml(path):
    with open(path, "r") as f:
        return yaml.safe_load(f)


# --------------------------------
# scalar decode
# --------------------------------
def unpack_scalar(packet, offset, field_type):

    fmt, size = TYPE_MAP[field_type]

    value = struct.unpack_from(
        fmt,
        packet,
        offset
    )[0]

    return value, offset + size


# --------------------------------
# item decode
# --------------------------------
def decode_item(packet, offset, item_def):

    result = {}

    fields = item_def["fields"]

    for name, spec in fields.items():

        #
        # scalar
        #
        if isinstance(spec, str):

            value, offset = unpack_scalar(
                packet,
                offset,
                spec
            )

            result[name] = value

        #
        # array
        #
        elif isinstance(spec, dict):

            elem_type = spec["type"].replace("[]", "")

            count_field = spec["count"]

            count = result[count_field]

            values = []

            for _ in range(count):

                value, offset = unpack_scalar(
                    packet,
                    offset,
                    elem_type
                )

                values.append(value)

            result[name] = values

    return result, offset


# --------------------------------
# packet decode
# --------------------------------
def decode_packet(packet, fmt_yaml):

    offset = 0

    decoded = {
        "header": {},
        "body": []
    }

    #
    # header
    #
    header_def = fmt_yaml["message"]["header"]["fields"]

    for name, field_type in header_def.items():

        value, offset = unpack_scalar(
            packet,
            offset,
            field_type
        )

        decoded["header"][name] = value

    data_count = decoded["header"]["data_count"]

    #
    # body
    #
    item_def = fmt_yaml["message"]["body"]["item"]

    for _ in range(data_count):

        item, offset = decode_item(
            packet,
            offset,
            item_def
        )

        decoded["body"].append(item)

    return decoded


# --------------------------------
# YAML保存
# --------------------------------
def append_yaml(path, data):

    try:

        with open(path, "r") as f:
            content = yaml.safe_load(f)

        if content is None:
            content = []

    except FileNotFoundError:
        content = []

    content.append(data)

    with open(path, "w") as f:
        yaml.dump(
            content,
            f,
            sort_keys=False
        )


# --------------------------------
# main
# --------------------------------
def main():

    parser = argparse.ArgumentParser()

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
        "--output_yaml",
        required=True
    )

    args = parser.parse_args()

    fmt_yaml = load_format_yaml(
        args.format
    )

    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_DGRAM
    )

    sock.bind(
        ("0.0.0.0", args.port)
    )

    sock.settimeout(1.0)

    print(
        f"Listening UDP port {args.port}"
    )

    count = 0

    try:

        while True:

            try:

                packet, addr = sock.recvfrom(
                    65535
                )

            except socket.timeout:
                continue

            decoded = decode_packet(
                packet,
                fmt_yaml
            )

            append_yaml(
                args.output_yaml,
                decoded
            )

            count += 1

            print(
                f"received {count} "
                f"from {addr} "
                f"data_count="
                f"{decoded['header']['data_count']}"
            )

    except KeyboardInterrupt:

        print("\nStopped by user")

    finally:

        sock.close()

        print(
            f"Finished. total packets={count}"
        )


if __name__ == "__main__":
    main()