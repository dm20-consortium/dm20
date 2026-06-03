#!/usr/bin/env python3

import argparse
import socket
import struct
import yaml

TYPE_MAP = {
    "uint8": "!B",
    "int8": "!b",
    "uint16": "!H",
    "int16": "!h",
    "uint32": "!I",
    "int32": "!i",
    "uint64": "!Q",
    "int64": "!q",
}


def pack_value(ftype, value):
    return struct.pack(TYPE_MAP[ftype], int(value))


def load_yaml(path):
    with open(path, "r") as f:
        return yaml.safe_load(f)


def encode_header(packet, header_def, header_val):

    for name, ftype in header_def.items():

        if name not in header_val:
            raise ValueError(f"missing header field: {name}")

        packet += pack_value(
            ftype,
            header_val[name]
        )

    return packet


def encode_detectable_information(packet, info_list):

    packet += pack_value(
        "uint8",
        len(info_list)
    )

    for info in info_list:

        packet += pack_value(
            "uint8",
            info["detectable_classes"]
        )

        xy_size = len(
            info["detectable_domain_x_list"]
        )

        if xy_size != len(
            info["detectable_domain_y_list"]
        ):
            raise ValueError(
                "x_list and y_list size mismatch"
            )

        packet += pack_value(
            "uint8",
            xy_size
        )

        for v in info["detectable_domain_x_list"]:
            packet += pack_value("int32", v)

        for v in info["detectable_domain_y_list"]:
            packet += pack_value("int32", v)

        packet += pack_value(
            "uint8",
            info["confidence"]
        )

        packet += pack_value(
            "uint16",
            info["minimal_detectable_size"]
        )

    return packet


def encode_body_item(packet, item_def, item_val):

    #
    # fields
    #
    for name, field_type in item_def["fields"].items():

        if name == "detectable_information_size":
            continue

        packet += pack_value(
            field_type,
            item_val[name]
        )

    #
    # detectable_information
    #
    packet = encode_detectable_information(
        packet,
        item_val["detectable_information"]
    )

    #
    # fields_after_detectable_information
    #
    for name, field_type in item_def[
        "fields_after_detectable_information"
    ].items():

        packet += pack_value(
            field_type,
            item_val[name]
        )

    return packet


def build_packet(format_yaml, values_yaml):

    packet = b''

    msg_def = format_yaml["message"]

    packet = encode_header(
        packet,
        msg_def["header"]["fields"],
        values_yaml["header"]
    )

    item_def = msg_def["body"]["item"]

    for body_item in values_yaml["body"]:

        packet = encode_body_item(
            packet,
            item_def,
            body_item
        )

    return packet


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
        required=True
    )

    parser.add_argument(
        "--values",
        required=True
    )

    args = parser.parse_args()

    format_yaml = load_yaml(args.format)

    values_yaml = load_yaml(args.values)

    packet = build_packet(
        format_yaml,
        values_yaml
    )

    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_DGRAM
    )

    try:

        sock.sendto(
            packet,
            (args.ip, args.port)
        )

        print(
            f"sent {len(packet)} bytes"
        )

    finally:

        sock.close()


if __name__ == "__main__":
    main()