#include "sharedVariables.hpp"
#include "consoleControl.cpp"

CheatController activeCheats{};

FOVData fovData{};
ESPData espData{};
BombData bombData{};

HANDLE consoleOutput{ getConsoleOutputHandle() };
