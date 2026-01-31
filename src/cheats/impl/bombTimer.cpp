#include "cheat_entries.hpp"

#include "render_data.hpp"

void runBombTimer(const CheatData &cheatData)
{
    const auto timeRemaining = cheatData.bombData.explosionTime - cheatData.globalVars.currentTime;

    const auto localPlayerDefuseTime = cheatData.localPlayer.hasDefuser ? 5.0 : 10.0;

    renderData.bomb = {
        .planted = cheatData.bombData.planted,
        .timeRemaining = timeRemaining,
        .localPlayerCanDefuse = timeRemaining > localPlayerDefuseTime,
        .beingDefused = cheatData.bombData.beingDefused,
        .defused = cheatData.bombData.defused,
        .defuser = cheatData.bombData.defuser,
        .site = cheatData.bombData.site};
}
