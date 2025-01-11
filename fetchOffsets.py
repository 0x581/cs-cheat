"""
This file gets the offsets from https://cloud.0x581.zip/s/fZna7TEp5SbsBc4/download?path=%2F&files=
"""

from dataclasses import dataclass

import requests

OFFSETS_URL = "https://cloud.0x581.zip/s/fZna7TEp5SbsBc4/download?path=%2F&files="

FETCHED_FILES: dict[str, str] = {}

@dataclass(init=True, frozen=True)
class Offset:
    alias: str
    file: str
    name: str
    comment: str = ""


OFFSETS: list[Offset] = [
    Offset(
        "localPlayerPawn",
        "offsets.hpp",
        "dwLocalPlayerPawn",
        "Relative to client.dll",
    ),
    Offset(
        "onGroundLastTick",
        "client_dll.hpp",
        "m_bOnGroundLastTick",
        "Member of localPlayerPawn",
    ),
    Offset(
        "movementServices",
        "client_dll.hpp",
        "m_pMovementServices",
        "Should be relative to localPlayerPawn, but idk",
    ),
    Offset(
        "buttons",
        "client_dll.hpp",
        "m_nButtons",
        "Button State struct",
    ),
    Offset(
        "jump",
        "buttons.hpp",
        "jump",
        "Relative to client.dll",
    ),
    Offset(
        "entityList",
        "offsets.hpp",
        "dwEntityList",
        "Relative to client.dll",
    ),
    Offset(
        "teamNum",
        "client_dll.hpp",
        "m_iTeamNum",
        "Member of C_BaseEntity",
    ),
    Offset(
        "spotted",
        "client_dll.hpp",
        "m_bSpotted",
        "Member of EntitySpottedState_t",
    ),
    Offset(
        "flashDuration",
        "client_dll.hpp",
        "m_flFlashDuration",
        "Member of localPlayerPawn",
    ),
    Offset(
        "playerPawn",
        "client_dll.hpp",
        "m_hPlayerPawn",
    ),
    Offset(
        "playerName",
        "client_dll.hpp",
        "m_iszPlayerName",
    ),
    Offset(
        "entitySpottedState",
        "client_dll.hpp",
        "m_entitySpottedState",
        "Member of C_CSPlayerPawn, was 0x23D0",
    ),
    Offset(
        "health",
        "client_dll.hpp",
        "m_iHealth",
        "Member of C_BaseEntity",
    ),

    Offset(
        "attack",
        "offsets.hpp",
        "attack",

    ),
    Offset(
        "IDEntIndex",
        "client_dll.hpp",
        "m_iIDEntIndex",
        
    ),
]


def get_github_file(file: str) -> str:
    """This function gets the file from the github repo."""
    if file not in FETCHED_FILES:
        response = requests.get(OFFSETS_URL + file)

        if response.status_code != 200:
            print(f"Failed to get {file} from github.")
            
            FETCHED_FILES[file] = ""

        FETCHED_FILES[file] = response.text

    return FETCHED_FILES[file]


def get_offset(offset: Offset) -> str:
    """This function gets the offset from the github repo."""
    offsets = get_github_file(offset.file).splitlines()

    offsets = [line for line in offsets if f"{offset.name} = " in line]

    if len(offsets) == 0:
        print(f"Failed to find {offset.name} in {offset.file}.")
        return ""

    if len(offsets) > 1:
        print(f"Found multiple options for {offset.name} in {offset.file}:")
        for line in offsets:
            print(f" -> {line}")
        return ""

    hex_offset = offsets[0].split(" = ")[1].split(";")[0]

    try:
        int(hex_offset, 16)
    except ValueError:
        print(f"Failed to parse {hex_offset} as a hex number.")
        return ""

    return hex_offset


def get_all_offsets() -> str:
    """This function gets the offsets from the github repo."""
    offsets = "#pragma once\n\nnamespace offsets {\n"

    longest = max(len(offset.alias) for offset in OFFSETS) + 40

    for offset in OFFSETS:
        offset_str = f"    constexpr auto {offset.alias} = {get_offset(offset)};"

        if offset.comment != "":
            offset_str = offset_str.ljust(longest)
            offset_str += f"// {offset.comment}"

        offset_str += "\n"

        offsets += offset_str

    offsets += "}\n"

    return offsets


if __name__ == "__main__":
    with open("offsets.hpp", "w") as file:
        file.write(get_all_offsets())
