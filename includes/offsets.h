#pragma once

namespace offsets
{
    constexpr auto localPlayerController = 0x1A46A00;        // Relative to client.dll
    constexpr auto localPlayerPawn = 0x184C0D0;              // Relative to client.dll
    constexpr auto onGroundLastTick = 0x2298;                // Member of localPlayerPawn, bool
    constexpr auto movementServices = 0x11E8;                // Member of C_BasePlayerPawn, CPlayer_MovementServices *
    constexpr auto buttons = 0x48;                           // Member of CPlayer_MovementServices, CInButtonState
    constexpr auto jump = 0x1844E00;                         // Relative to client.dll
    constexpr auto entityList = 0x19F7F20;                   // Relative to client.dll
    constexpr auto teamNum = 0x3E3;                          // Member of C_BaseEntity, uint8
    constexpr auto spotted = 0x8;                            // Member of EntitySpottedState_t, bool
    constexpr auto flashDuration = 0x140C;                   // Member of localPlayerPawn, float
    constexpr auto pawnOfController = 0x824;                 // Member of CCSPlayerController, Handle
    constexpr auto controllerOfPawn = 0x133C;                // Member of C_BasePlayerPawn, Handle
    constexpr auto playerName = 0x660;                       // Member of CBasePlayerController, char[128]
    constexpr auto entitySpottedState = 0x23D0;              // Member of C_CSPlayerPawn
    constexpr auto health = 0x344;                           // Member of C_BaseEntity, int32
    constexpr auto attack = 0x18448F0;
    constexpr auto IDEntIndex = 0x1458;
    constexpr auto CameraServices = 0x11E0;                  // Member of C_BasePlayerPawn, CPlayer_CameraServices *
    constexpr auto FOV = 0x210;                              // Member of CCSPlayerBase_CameraServices, uint32
    constexpr auto IsScoped = 0x23E8;                        // Member of C_BasePlayerPawn, bool
    constexpr auto ViewAngles = 0x1A6B2E0;
    constexpr auto ViewMatrix = 0x1A60EF0;
    constexpr auto OldOrigin = 0x1324;                       // Member of C_BasePlayerPawn, Vector
    constexpr auto GameRules = 0x1A5C7F8;                    // C_CSGameRules
    constexpr auto BombPlanted = 0x9A5;                      // Member of C_CSGameRules, bool
    constexpr auto plantedC4 = 0x1A670D8;                    // C_PlantedC4 * (Need to dereference twice)
    constexpr auto BombTicking = 0xF90;                      // Member of C_PlantedC4, bool
    constexpr auto m_bC4Activated = 0xFD8;                   // Member of C_PlantedC4, bool
    constexpr auto BombSite = 0xF94;                         // Member of C_PlantedC4, int32
    constexpr auto crouchAmount = 0x228;                     // Member of CPlayer_MovementServices, float
    constexpr auto globalVars = 0x1840148;                   // Pointer to the global vars struct
    constexpr auto c4ExplosionTime = 0xFC0;                  // Member of C_PlantedC4, float
    constexpr auto itemServices = 0x11B0;                    // Member of C_BasePlayerPawn, CPlayer_ItemServices *
    constexpr auto hasDefuser = 0x40;                        // Member of CCSPlayer_ItemServices, bool
    constexpr auto gameSceneNode = 0x328;                    // Member of C_BaseEntity, CSkeletonInstance*
    constexpr auto modelState = 0x170;                       // Member of CSkeletonInstance, CModelState
    constexpr auto SpottedByMask = 0xC;                       // Member of CSkeletonInstance, CModelState
}
