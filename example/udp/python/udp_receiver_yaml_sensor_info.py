#!/usr/bin/env python3

import argparse
import socket
import struct
import yaml

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


def unpack(ftype, packet, offset):

    fmt, size = TYPE_MAP[ftype]

    value = struct.unpack_from(
        fmt,
        packet,
        offset
    )[0]

    return value, offset + size


def load_yaml(path):
    with open(path) as f:
        return yaml.safe_load(f)


def decode_packet(packet, msg):

    result = {}

    offset = 0

    header = msg["header"]["fields"]

    for name, ftype in header.items():

        value, offset = unpack(
            ftype,
            packet,
            offset
        )

        result[name] = value

    body = {}

    item = msg["body"]["item"]

    for name, field in item["fields"].items():

        if name == "detectable_information_size":

            info_size, offset = unpack(
                "uint8",
                packet,
                offset
            )

            body[name] = info_size

            infos = []

            for _ in range(info_size):

                info = {}

                info[
                    "detectable_classes"
                ], offset = unpack(
                    "uint8",
                    packet,
                    offset
                )

                xy_size, offset = unpack(
                    "uint8",
                    packet,
                    offset
                )

                info[
                    "detectable_classes_xy_list_size"
                ] = xy_size

                xs = []

                for _ in range(xy_size):

                    v, offset = unpack(
                        "int32",
                        packet,
                        offset
                    )

                    xs.append(v)

                ys = []

                for _ in range(xy_size):

                    v, offset = unpack(
                        "int32",
                        packet,
                        offset
                    )

                    ys.append(v)

                info[
                    "detectable_domain_x_list"
                ] = xs

                info[
                    "detectable_domain_y_list"
                ] = ys

                info[
                    "confidence"
                ], offset = unpack(
                    "uint8",
                    packet,
                    offset
                )

                info[
                    "minimal_detectable_size"
                ], offset = unpack(
                    "uint16",
                    packet,
                    offset
                )

                infos.append(info)

            body[
                "detectable_information"
            ] = infos

            continue

        value, offset = unpack(
            field,
            packet,
            offset
        )

        body[name] = value

    for name, field in item[
        "fields_after_detectable_information"
    ].items():

        value, offset = unpack(
            field,
            packet,
            offset
        )

        body[name] = value

    result["body"] = body

    return result


def main():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--port",
        type=int,
        required=True
    )

    parser.add_argument(
        "--format",
        required=True
    )

    parser.add_argument(
        "--output_yaml",
        required=True
    )

    args = parser.parse_args()

    y = load_yaml(args.format)

    msg = y["message"]

    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_DGRAM
    )

    sock.bind(
        ("0.0.0.0", args.port)
    )

    sock.settimeout(1.0)

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
                msg
            )

            with open(
                args.output_yaml,
                "a"
            ) as f:

                yaml.safe_dump(
                    decoded,
                    f,
                    sort_keys=False
                )

                f.write("---\n")

            print(
                f"received from {addr}"
            )

    except KeyboardInterrupt:
        print("\nStopped")

    finally:
        sock.close()


if __name__ == "__main__":
    main()