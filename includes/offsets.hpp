#pragma once

namespace offsets
{
    constexpr auto localPlayerController = 0x22ECA28;         // Member of client_dll
    constexpr auto localPlayerPawn = 0x2062850;               // Member of client_dll
    constexpr auto entityList = 0x24A7B28;                    // Member of client_dll
    constexpr auto viewAngles = 0x2312BD8;                    // Member of client_dll
    constexpr auto viewMatrix = 0x2308860;                    // Member of client_dll
    constexpr auto gameRules = 0x23035F8;                     // Member of client_dll, C_CSGameRules
    constexpr auto globalVars = 0x20572A8;                    // Member of client_dll, Global Vars *
    constexpr auto plantedC4 = 0x230FCF0;                     // Member of client_dll, C_PlantedC4 **
    constexpr auto attack = 0x205B5E0;                        // Member of buttons
    constexpr auto jump = 0x205BAF0;                          // Member of buttons
    constexpr auto entityIdentity = 0x10;                     // Member of CEntityInstance, CEntityIdentity*
    constexpr auto gameSceneNode = 0x338;                     // Member of C_BaseEntity, CGameSceneNode*, (CSkeletonInstance*)
    constexpr auto maxHealth = 0x350;                         // Member of C_BaseEntity, int32
    constexpr auto health = 0x354;                            // Member of C_BaseEntity, int32
    constexpr auto teamNum = 0x3F3;                           // Member of C_BaseEntity, uint8
    constexpr auto itemServices = 0x13E0;                     // Member of C_BasePlayerPawn, CPlayer_ItemServices*
    constexpr auto cameraServices = 0x1410;                   // Member of C_BasePlayerPawn, CPlayer_CameraServices*
    constexpr auto movementServices = 0x1418;                 // Member of C_BasePlayerPawn, CPlayer_MovementServices*
    constexpr auto oldOrigin = 0x1588;                        // Member of C_BasePlayerPawn, Vector
    constexpr auto controllerOfPawn = 0x15A0;                 // Member of C_BasePlayerPawn, CHandle<CBasePlayerController>
    constexpr auto flashDuration = 0x15F8;                    // Member of C_CSPlayerPawnBase, float32
    constexpr auto idEntIndex = 0x3EAC;                       // Member of C_CSPlayerPawn, CEntityIndex
    constexpr auto onGroundLastTick = 0x23E4;                 // Member of C_CSPlayerPawn, bool
    constexpr auto entitySpottedState = 0x26E0;               // Member of C_CSPlayerPawn, EntitySpottedState_t
    constexpr auto isScoped = 0x26F8;                         // Member of C_CSPlayerPawn, bool
    constexpr auto buttons = 0x50;                            // Member of CPlayer_MovementServices, CInButtonState
    constexpr auto crouchAmount = 0x284;                      // Member of CCSPlayer_MovementServices, float32
    constexpr auto fov = 0x290;                               // Member of CCSPlayerBase_CameraServices, uint32
    constexpr auto hasDefuser = 0x48;                         // Member of CCSPlayer_ItemServices, bool
    constexpr auto spotted = 0x8;                             // Member of EntitySpottedState_t, bool
    constexpr auto spottedByMask = 0xC;                       // Member of EntitySpottedState_t, uint32[2]
    constexpr auto pawnOfController = 0x90C;                  // Member of CCSPlayerController, CHandle<C_CSPlayerPawn>
    constexpr auto playerName = 0x6F8;                        // Member of CBasePlayerController, char[128]
    constexpr auto modelState = 0x160;                        // Member of CSkeletonInstance, CModelState
    constexpr auto c4BombSite = 0x1174;                       // Member of C_PlantedC4, int32
    constexpr auto c4ExplosionTime = 0x11A0;                  // Member of C_PlantedC4, GameTime_t
    constexpr auto c4BeingDefused = 0x11AC;                   // Member of C_PlantedC4, bool
    constexpr auto c4Defused = 0x11C4;                        // Member of C_PlantedC4, bool
    constexpr auto c4Defuser = 0x11C8;                        // Member of C_PlantedC4, CHandle<C_CSPlayerPawn>
    constexpr auto nameStringableIndex = 0x14;                // Member of CEntityIdentity, int32
    constexpr auto entityIdentityName = 0x18;                 // Member of CEntityIdentity, CUtlSymbolLarge
    constexpr auto entityIdentityDesignerName = 0x20;         // Member of CEntityIdentity, CUtlSymbolLarge
    constexpr auto entityIdentityFlags = 0x30;                // Member of CEntityIdentity, uint32
}
