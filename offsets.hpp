#pragma once

namespace offsets {
    constexpr auto localPlayerPawn = 0x1869D88;           // Relative to client.dll
    constexpr auto onGroundLastTick = 0x2298;             // Member of localPlayerPawn
    constexpr auto movementServices = 0x11E8;             // Should be relative to localPlayerPawn, but idk
    constexpr auto buttons = 0x48;                        // Button State struct
    constexpr auto jump = 0x1862AB0;                      // Relative to client.dll
    constexpr auto entityList = 0x1A157C8;                // Relative to client.dll
    constexpr auto teamNum = 0x3E3;                       // Member of C_BaseEntity
    constexpr auto spotted = 0x8;                         // Member of EntitySpottedState_t
    constexpr auto flashDuration = 0x140C;                // Member of localPlayerPawn
    constexpr auto playerPawn = 0x80C;
    constexpr auto playerName = 0x660;
    constexpr auto entitySpottedState = 0x23D0;           // Member of C_CSPlayerPawn, was 0x23D0
    constexpr auto health = 0x344;                        // Member of C_BaseEntity
}
