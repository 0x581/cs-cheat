#include "sharedVariables.hpp"
#include "consoleControl.cpp"

FOVData fovData{};
ESPData espData{};
BombData bombData{};

HANDLE consoleOutput{ getConsoleOutputHandle() };
