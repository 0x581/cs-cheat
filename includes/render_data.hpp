#pragma once

#include <chrono>

#include "cs_defines.hpp"
#include "boneData.hpp"

struct RenderData
{
    bool isValid{false};
    std::chrono::high_resolution_clock::time_point lastCaptureTime{};

    struct Esp
    {
        struct EspData
        {
            bool isValid{false};
            bool isVisible{false};
            bool isFriendly{false};
            int maxHealth{0};
            int health{0};
            float distance{0.f};
            char playerName[PLAYER_NAME_LENGTH]{0};
            Vector2 min{0, 0};
            Vector2 max{0, 0};
            BoneData bones{};
        };

        EspData data[64]{};
    } esp;

    struct Bomb
    {
        bool planted{false};
        float timeRemaining{0.f};
        bool localPlayerCanDefuse{false};
        bool beingDefused{false};
        bool defused{false};
        CEntityIndex defuser{0};
        uint32_t site{0};
    } bomb;
};

extern RenderData renderData;
