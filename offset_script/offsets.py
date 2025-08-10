from dataclasses import dataclass


@dataclass
class Offset:

    def __init__(
        self, alias: str, name: str, comment: str = "", file: str = "client_dll.hpp"
    ):
        *namespaces, var_name = name.split("::")

        self.alias = alias
        self.file = file

        self.name = name
        self.var_name = var_name
        self.namespaces = namespaces

        self.comment = comment.strip().removeprefix("//").strip()


OFFSETS: list[Offset] = [
    Offset("localPlayerController", "dwLocalPlayerController", "", "offsets.hpp"),
    Offset("localPlayerPawn", "dwLocalPlayerPawn", "", "offsets.hpp"),
    Offset("entityList", "dwEntityList", "", "offsets.hpp"),
    Offset("ViewAngles", "dwViewAngles", "", "offsets.hpp"),
    Offset("ViewMatrix", "dwViewMatrix", "", "offsets.hpp"),
    Offset("GameRules", "dwGameRules", "C_CSGameRules", "offsets.hpp"),
    Offset("globalVars", "dwGlobalVars", "Global Vars *", "offsets.hpp"),
    Offset(
        "plantedC4", "dwPlantedC4", "C_PlantedC4 * (Need to deref twice)", "offsets.hpp"
    ),
    Offset("attack", "attack", "", "buttons.hpp"),
    Offset("jump", "jump", "", "buttons.hpp"),
    Offset("gameSceneNode", "m_pGameSceneNode", "(CSkeletonInstance*)"),
    Offset("maxHealth", "m_iMaxHealth"),
    Offset("health", "m_iHealth"),
    Offset("teamNum", "m_iTeamNum"),
    Offset("itemServices", "m_pItemServices"),
    Offset("cameraServices", "m_pCameraServices"),
    Offset("movementServices", "m_pMovementServices"),
    Offset("oldOrigin", "m_vOldOrigin"),
    Offset("controllerOfPawn", "m_hController"),
    Offset("flashDuration", "m_flFlashDuration"),
    Offset("IDEntIndex", "m_iIDEntIndex"),
    Offset("onGroundLastTick", "m_bOnGroundLastTick"),
    Offset("entitySpottedState", "C_CSPlayerPawn::m_entitySpottedState"),
    Offset("isScoped", "m_bIsScoped"),
    Offset("buttons", "m_nButtons"),
    Offset("crouchAmount", "m_flDuckAmount"),
    Offset("FOV", "m_iFOV"),
    Offset("hasDefuser", "m_bHasDefuser"),
    Offset("spotted", "m_bSpotted"),
    Offset("spottedByMask", "m_bSpottedByMask"),
    Offset("pawnOfController", "m_hPlayerPawn"),
    Offset("playerName", "m_iszPlayerName"),
    Offset("modelState", "m_modelState"),
    #
    # Bomb  offsets
    Offset("c4BombSite", "m_nBombSite"),
    Offset("c4ExplosionTime", "m_flC4Blow"),
    Offset("c4BeingDefused", "m_bBeingDefused"),
    Offset("c4Defused", "m_bBombDefused"),
    Offset("c4Defuser", "m_hBombDefuser"),
]
