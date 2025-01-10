namespace offsets {
    constexpr auto localPlayerPawn = 0x1869D88;
    constexpr auto onGroundLastTick = 0x2298;           // Relative to localPlayerPawn, bool
    constexpr auto movementServices = 0x11E8;           // Should be relative to localPlayerPawn, but idk
    constexpr auto buttons = 0x48;                      // Button State struct
    constexpr auto jump = 0x1862AB0;                    // Namespace in client.dll
}
