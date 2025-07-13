#include "../shared_variables.h"
#include "../console_control.h"

bool isConnected{ false };

ESPData espData{};
BombData bombData{};

HANDLE consoleOutput{ getConsoleOutputHandle() };
