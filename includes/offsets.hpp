#pragma once

namespace offsets
{
    constexpr auto localPlayerController = 0x252B0C8;         // Member of client_dll
    constexpr auto localPlayerPawn = 0x2553D38;               // Member of client_dll
    constexpr auto entityList = 0x2709010;                    // Member of client_dll
    constexpr auto viewAngles = 0x256A4B8;                    // Member of client_dll
    constexpr auto viewMatrix = 0x2559640;                    // Member of client_dll
    constexpr auto gameRules = 0x25537E8;                     // Member of client_dll, C_CSGameRules
    constexpr auto globalVars = 0x2221AB8;                    // Member of client_dll, Global Vars *
    constexpr auto plantedC4 = 0x24B9B90;                     // Member of client_dll, C_PlantedC4 **
    constexpr auto attack = 0x2225900;                        // Member of buttons
    constexpr auto jump = 0x2225E10;                          // Member of buttons
    constexpr auto entityIdentity = 0x10;                     // Member of CEntityInstance, CEntityIdentity*
    constexpr auto gameSceneNode = 0x330;                     // Member of C_BaseEntity, CGameSceneNode*, (CSkeletonInstance*)
    constexpr auto maxHealth = 0x348;                         // Member of C_BaseEntity, int32
    constexpr auto health = 0x34C;                            // Member of C_BaseEntity, int32
    constexpr auto teamNum = 0x3E7;                           // Member of C_BaseEntity, uint8
    constexpr auto itemServices = 0x12F8;                     // Member of C_BasePlayerPawn, CPlayer_ItemServices*
    constexpr auto cameraServices = 0x1328;                   // Member of C_BasePlayerPawn, CPlayer_CameraServices*
    constexpr auto movementServices = 0x1330;                 // Member of C_BasePlayerPawn, CPlayer_MovementServices*
    constexpr auto oldOrigin = 0x14A4;                        // Member of C_BasePlayerPawn, Vector
    constexpr auto controllerOfPawn = 0x14BC;                 // Member of C_BasePlayerPawn, CHandle<CBasePlayerController>
    constexpr auto flashDuration = 0x1510;                    // Member of C_CSPlayerPawnBase, float32
    constexpr auto idEntIndex = 0x370C;                       // Member of C_CSPlayerPawn, CEntityIndex
    constexpr auto onGroundLastTick = 0x1DB8;                 // Member of C_CSPlayerPawn, bool
    constexpr auto entitySpottedState = 0x1EB8;               // Member of C_CSPlayerPawn, EntitySpottedState_t
    constexpr auto isScoped = 0x1ED0;                         // Member of C_CSPlayerPawn, bool
    constexpr auto buttons = 0x50;                            // Member of CPlayer_MovementServices, CInButtonState
    constexpr auto crouchAmount = 0x40C;                      // Member of CCSPlayer_MovementServices, float32
    constexpr auto fov = 0x298;                               // Member of CCSPlayerBase_CameraServices, uint32
    constexpr auto hasDefuser = 0x48;                         // Member of CCSPlayer_ItemServices, bool
    constexpr auto spotted = 0x8;                             // Member of EntitySpottedState_t, bool
    constexpr auto spottedByMask = 0xC;                       // Member of EntitySpottedState_t, uint32[2]
    constexpr auto pawnOfController = 0x92C;                  // Member of CCSPlayerController, CHandle<C_CSPlayerPawn>
    constexpr auto playerName = 0x6FC;                        // Member of CBasePlayerController, char[128]
    constexpr auto modelState = 0x140;                        // Member of CSkeletonInstance, CModelState
    constexpr auto c4BombSite = 0x128C;                       // Member of C_PlantedC4, int32
    constexpr auto c4ExplosionTime = 0x12B8;                  // Member of C_PlantedC4, GameTime_t
    constexpr auto c4BeingDefused = 0x12C4;                   // Member of C_PlantedC4, bool
    constexpr auto c4Defused = 0x12DC;                        // Member of C_PlantedC4, bool
    constexpr auto c4Defuser = 0x12E0;                        // Member of C_PlantedC4, CHandle<C_CSPlayerPawn>
    constexpr auto entityIdentityName = 0x18;                 // Member of CEntityIdentity, CUtlSymbolLarge
    constexpr auto entityIdentityDesignerName = 0x20;         // Member of CEntityIdentity, CUtlSymbolLarge
    constexpr auto entityIdentityFlags = 0x30;                // Member of CEntityIdentity, uint32
}
