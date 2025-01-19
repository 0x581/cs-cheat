#include "sharedVariables.hpp"
#include "consoleControl.cpp"

CheatController activeCheats{};

FOVController fovController{};

ESPController espController{};

HANDLE consoleOutput{ getConsoleOutputHandle() };
