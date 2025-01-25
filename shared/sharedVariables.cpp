#include "sharedVariables.hpp"
#include "consoleControl.cpp"

ActiveCheats activeCheats{};

FOVData fovData{};
ESPData espData{};
BombData bombData{};

HANDLE consoleOutput{ getConsoleOutputHandle() };
