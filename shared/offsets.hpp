#pragma once

namespace offsets
{
    constexpr auto localPlayerPawn = 0x186BDF8;           // Relative to client.dll
    constexpr auto onGroundLastTick = 0x2298;             // Member of localPlayerPawn, bool
    constexpr auto movementServices = 0x11E8;             // Member of C_BasePlayerPawn, CPlayer_MovementServices *
    constexpr auto buttons = 0x48;                        // Member of CPlayer_MovementServices, CInButtonState
    constexpr auto jump = 0x1864B30;                      // Relative to client.dll
    constexpr auto entityList = 0x1A176C8;                // Relative to client.dll
    constexpr auto teamNum = 0x3E3;                       // Member of C_BaseEntity, uint8
    constexpr auto spotted = 0x8;                         // Member of EntitySpottedState_t, bool
    constexpr auto flashDuration = 0x140C;                // Member of localPlayerPawn, float
    constexpr auto pawnOfController = 0x80C;              // Member of CCSPlayerController, Handle
    constexpr auto controllerOfPawn = 0x133C;             // Member of C_BasePlayerPawn, Handle
    constexpr auto playerName = 0x660;                    // Member of CBasePlayerController, char[128]
    constexpr auto entitySpottedState = 0x23D0;                // Member of C_CSPlayerPawn, was 0x23D0
    constexpr auto health = 0x344;                        // Member of C_BaseEntity, int32
    constexpr auto attack = 0x1864620;
    constexpr auto IDEntIndex = 0x1458;
    constexpr auto CameraServices = 0x11E0;
    constexpr auto FOV = 0x210;
    constexpr auto IsScoped = 0x23E8;
    constexpr auto ViewAngles = 0x1A8C830;
    constexpr auto ViewMatrix = 0x1A82740;
    constexpr auto OldOrigin = 0x1324;
    constexpr auto GameRules = 0x1A7E048;
    constexpr auto BombPlanted = 0x9A5; // bool
}
