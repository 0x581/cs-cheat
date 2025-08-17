#pragma once

namespace offsets
{
    constexpr auto localPlayerController = 0x1E1E850; // Member of client_dll
    constexpr auto localPlayerPawn = 0x1BF14A0;       // Member of client_dll
    constexpr auto entityList = 0x1D15578;            // Member of client_dll
    constexpr auto viewAngles = 0x1E3D780;            // Member of client_dll
    constexpr auto viewMatrix = 0x1E328A0;            // Member of client_dll
    constexpr auto gameRules = 0x1E31AE0;             // Member of client_dll, C_CSGameRules
    constexpr auto globalVars = 0x1BE69B0;            // Member of client_dll, Global Vars *
    constexpr auto plantedC4 = 0x1E37E10;             // Member of client_dll, C_PlantedC4 **
    constexpr auto attack = 0x1BEAB30;                // Member of buttons
    constexpr auto jump = 0x1BEB040;                  // Member of buttons
    constexpr auto entityIdentity = 0x10;             // Member of CEntityInstance, CEntityIdentity*
    constexpr auto gameSceneNode = 0x330;             // Member of C_BaseEntity, CGameSceneNode*, (CSkeletonInstance*)
    constexpr auto maxHealth = 0x348;                 // Member of C_BaseEntity, int32
    constexpr auto health = 0x34C;                    // Member of C_BaseEntity, int32
    constexpr auto teamNum = 0x3EB;                   // Member of C_BaseEntity, uint8
    constexpr auto itemServices = 0x1408;             // Member of C_BasePlayerPawn, CPlayer_ItemServices*
    constexpr auto cameraServices = 0x1438;           // Member of C_BasePlayerPawn, CPlayer_CameraServices*
    constexpr auto movementServices = 0x1440;         // Member of C_BasePlayerPawn, CPlayer_MovementServices*
    constexpr auto oldOrigin = 0x15B0;                // Member of C_BasePlayerPawn, Vector
    constexpr auto controllerOfPawn = 0x15C8;         // Member of C_BasePlayerPawn, CHandle<CBasePlayerController>
    constexpr auto flashDuration = 0x1658;            // Member of C_CSPlayerPawnBase, float32
    constexpr auto idEntIndex = 0x16FC;               // Member of C_CSPlayerPawnBase, CEntityIndex
    constexpr auto onGroundLastTick = 0x2540;         // Member of C_CSPlayerPawn, bool
    constexpr auto entitySpottedState = 0x2838;       // Member of C_CSPlayerPawn, EntitySpottedState_t
    constexpr auto isScoped = 0x2850;                 // Member of C_CSPlayerPawn, bool
    constexpr auto buttons = 0x48;                    // Member of CPlayer_MovementServices, CInButtonState
    constexpr auto crouchAmount = 0x290;              // Member of CCSPlayer_MovementServices, float32
    constexpr auto fov = 0x288;                       // Member of CCSPlayerBase_CameraServices, uint32
    constexpr auto hasDefuser = 0x40;                 // Member of CCSPlayer_ItemServices, bool
    constexpr auto spotted = 0x8;                     // Member of EntitySpottedState_t, bool
    constexpr auto spottedByMask = 0xC;               // Member of EntitySpottedState_t, uint32[2]
    constexpr auto pawnOfController = 0x8FC;          // Member of CCSPlayerController, CHandle<C_CSPlayerPawn>
    constexpr auto playerName = 0x6E8;                // Member of CBasePlayerController, char[128]
    constexpr auto modelState = 0x190;                // Member of CSkeletonInstance, CModelState
    constexpr auto c4BombSite = 0x1174;               // Member of C_PlantedC4, int32
    constexpr auto c4ExplosionTime = 0x11A0;          // Member of C_PlantedC4, GameTime_t
    constexpr auto c4BeingDefused = 0x11AC;           // Member of C_PlantedC4, bool
    constexpr auto c4Defused = 0x11C4;                // Member of C_PlantedC4, bool
    constexpr auto c4Defuser = 0x11C8;                // Member of C_PlantedC4, CHandle<C_CSPlayerPawn>
    constexpr auto nameStringableIndex = 0x14;        // Member of CEntityIdentity, int32
    constexpr auto entityIdentityName = 0x18;         // Member of CEntityIdentity, CUtlSymbolLarge
    constexpr auto entityIdentityDesignerName = 0x20; // Member of CEntityIdentity, CUtlSymbolLarge
    constexpr auto entityIdentityFlags = 0x30;        // Member of CEntityIdentity, uint32
}
