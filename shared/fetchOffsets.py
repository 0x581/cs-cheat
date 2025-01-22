"""
This file gets the offsets from any of the following sites:
https://cloud.0x581.zip/s/fZna7TEp5SbsBc4/download?path=%2F&files=
https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/
"""

from dataclasses import dataclass

import requests

OFFSETS_URL = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/"

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
        "Member of localPlayerPawn, bool",
    ),
    Offset(
        "movementServices",
        "client_dll.hpp",
        "m_pMovementServices",
        "Member of C_BasePlayerPawn, CPlayer_MovementServices *",
    ),
    Offset(
        "buttons",
        "client_dll.hpp",
        "m_nButtons",
        "Member of CPlayer_MovementServices, CInButtonState",
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
        "Member of C_BaseEntity, uint8",
    ),
    Offset(
        "spotted",
        "client_dll.hpp",
        "m_bSpotted",
        "Member of EntitySpottedState_t, bool",
    ),
    Offset(
        "flashDuration",
        "client_dll.hpp",
        "m_flFlashDuration",
        "Member of localPlayerPawn, float",
    ),
    Offset(
        "pawnOfController",
        "client_dll.hpp",
        "m_hPlayerPawn",
        "Member of CCSPlayerController, Handle",
    ),
    Offset(
        "controllerOfPawn",
        "client_dll.hpp",
        "m_hController",
        "Member of C_BasePlayerPawn, Handle",
    ),
    Offset(
        "playerName",
        "client_dll.hpp",
        "m_iszPlayerName",
        "Member of CBasePlayerController, char[128]",
    ),
    Offset(
        "entitySpottedState",
        "client_dll.hpp",
        "C_CSPlayerPawn::m_entitySpottedState",
        "Member of C_CSPlayerPawn",
    ),
    Offset(
        "health",
        "client_dll.hpp",
        "m_iHealth",
        "Member of C_BaseEntity, int32",
    ),
    Offset(
        "attack",
        "buttons.hpp",
        "attack",
    ),
    Offset(
        "IDEntIndex",
        "client_dll.hpp",
        "m_iIDEntIndex",
    ),
    Offset(
        "CameraServices",
        "client_dll.hpp",
        "m_pCameraServices",
        "Member of C_BasePlayerPawn, CPlayer_CameraServices *",
    ),
    Offset(
        "FOV",
        "client_dll.hpp",
        "m_iFOV",
        "Member of CCSPlayerBase_CameraServices, uint32",
    ),
    Offset(
        "IsScoped",
        "client_dll.hpp",
        "m_bIsScoped",
        "Member of C_BasePlayerPawn, bool",
    ),
    Offset(
        "ViewAngles",
        "offsets.hpp",
        "dwViewAngles",
    ),
    Offset(
        "ViewMatrix",
        "offsets.hpp",
        "dwViewMatrix",
    ),
    Offset(
        "OldOrigin",
        "client_dll.hpp",
        "m_vOldOrigin",
        "Member of C_BasePlayerPawn, Vector",
    ),
    Offset(
        "GameRules",
        "offsets.hpp",
        "dwGameRules",
        "C_CSGameRules",
    ),
    Offset(
        "BombPlanted",
        "client_dll.hpp",
        "C_CSGameRules::m_bBombPlanted",
        "Member of C_CSGameRules, bool",
    ),
    Offset(
        "plantedC4",
        "offsets.hpp",
        "dwPlantedC4",
        "C_PlantedC4",
    ),
    Offset(
        "BombTicking",
        "client_dll.hpp",
        "m_bBombTicking",
        "Member of C_PlantedC4, bool",
    ),
    Offset(
        "m_bC4Activated",
        "client_dll.hpp",
        "m_bC4Activated",
        "Member of C_PlantedC4, bool",
    ),
    Offset(
        "BombSite",
        "client_dll.hpp",
        "m_nBombSite",
        "Member of C_PlantedC4, int32",
    ),
    Offset(
        "crouchAmount",
        "client_dll.hpp",
        "m_flDuckAmount",
        "Member of CPlayer_MovementServices, float",
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
    file_string = get_github_file(offset.file)

    namespaces = offset.name.split("::")[:-1]

    search_string = offset.name.split("::")[-1]
    search_string = f"{search_string} = "

    split_file = file_string.split(search_string)

    if len(split_file) == 1:
        print(f"Failed to find {offset.name} in {offset.file}.")
        return ""

    if len(split_file) == 2:
        hex_offset = split_file[1].split(";")[0]

        try:
            int(hex_offset, 16)
        except ValueError:
            print(f"Failed to parse {hex_offset} as a hex number for {offset.name}.")
            return ""

        return hex_offset

    matches = list(range(len(split_file) - 1))
    split_str_copy = split_file.copy()

    while len(namespaces) > 0:
        namespace = f"namespace {namespaces.pop()} {{"

        for i in matches.copy():
            split = split_str_copy[i].split(namespace)

            if len(split) == 1:
                matches.remove(i)
                continue

            if "{" in split[-1]:
                matches.remove(i)
                continue

            split_str_copy[i] = split[0]

    if len(matches) == 0:
        print(f"Failed to find {offset.name} in {offset.file}.")
        return ""

    if len(matches) > 1:
        print(f"Found multiple matches for {offset.name} in {offset.file}.")
        return ""

    match_data = split_file[matches[0] + 1]

    hex_offset = match_data.split(";")[0]

    try:
        int(hex_offset, 16)
    except ValueError:
        print(f"Failed to parse {hex_offset} as a hex number.")
        return ""

    return hex_offset


def get_all_offsets() -> str:
    """This function gets the offsets from the github repo."""
    offsets = "#pragma once\n\nnamespace offsets\n{\n"

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
    with open("shared/offsets.hpp", "w") as file:
        file.write(get_all_offsets())
