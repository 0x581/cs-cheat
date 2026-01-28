"""
This file gets the offsets from any of the following sites:
https://cloud.0x581.zip/s/fZna7TEp5SbsBc4/download?path=%2F&files=
https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/
"""

import os
import sys
import requests

from offsets import OFFSETS

from parse_file import parse, NamespaceNode

OFFSETS_URL = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/"
# OFFSETS_URL = "https://cloud.0x581.zip/s/dwiLHbz7SHcwKZ3/download?path=%2F&files="

FETCHED_FILES: dict[str, NamespaceNode] = {}


def get_github_file(file: str) -> NamespaceNode:
    """This function gets the file from the github repo."""
    if file not in FETCHED_FILES:
        response = requests.get(OFFSETS_URL + file)

        if response.status_code != 200:
            print(f"Failed to get {file} from github.")
            sys.exit(-1)

        FETCHED_FILES[file] = parse(response.text)

    return FETCHED_FILES[file]


def get_all_offsets() -> str:
    """This function gets the offsets from the github repo."""
    offsets = "#pragma once\n\nnamespace offsets\n{\n"

    longest = max(len(offset.alias) for offset in OFFSETS) + 35

    for offset in OFFSETS:
        ns = get_github_file(offset.file)

        offset_node = ns.find_offset(offset)

        if offset_node is None:
            print(f"Failed to find offset {offset.name} in {offset.file}.")
            continue

        string = f"    constexpr auto {offset.alias} = {offset_node.value};"
        string = string.ljust(longest)

        string += f" // Member of {offset_node.parent.name}"

        if offset_node.comment:
            string += f", {offset_node.comment}"

        if offset.comment:
            string += f", {offset.comment}"

        string += "\n"

        offsets += string

    offsets += "}\n"

    return offsets


if __name__ == "__main__":
    output_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), os.pardir, "includes", "offsets.hpp"
    )
    with open(output_path, "w") as file:
        file.write(get_all_offsets())
