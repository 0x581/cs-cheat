#pragma once

namespace offsets
{
    constexpr auto localPlayerPawn = 0x1869D88;           // Relative to client.dll
    constexpr auto onGroundLastTick = 0x2298;             // Member of localPlayerPawn, bool
    constexpr auto movementServices = 0x11E8;             // Member of C_BasePlayerPawn, CPlayer_MovementServices *
    constexpr auto buttons = 0x48;                        // Member of CPlayer_MovementServices, CInButtonState
    constexpr auto jump = 0x1862AB0;                      // Relative to client.dll
    constexpr auto entityList = 0x1A157C8;                // Relative to client.dll
    constexpr auto teamNum = 0x3E3;                       // Member of C_BaseEntity, uint8
    constexpr auto spotted = 0x8;                         // Member of EntitySpottedState_t, bool
    constexpr auto flashDuration = 0x140C;                // Member of localPlayerPawn, float
    constexpr auto pawnOfController = 0x80C;              // Member of CCSPlayerController, Handle
    constexpr auto controllerOfPawn = 0x133C;             // Member of C_BasePlayerPawn, Handle
    constexpr auto playerName = 0x660;                    // Member of CBasePlayerController, char[128]
    constexpr auto entitySpottedState = 0x23D0;           // Member of C_CSPlayerPawn, was 0x23D0
    constexpr auto health = 0x344;                        // Member of C_BaseEntity, int32
    constexpr auto attack = 0x18625A0;
    constexpr auto IDEntIndex = 0x1458;
    constexpr auto CameraServices = 0x10E0;
    constexpr auto FOV = 0x210;
    constexpr auto IsScoped = 0x13A8;
}
