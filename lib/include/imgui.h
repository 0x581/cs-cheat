#pragma once

#define M_PI 3.1415926535

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3dcompiler")

#include <windows.h>
#include <tchar.h>
#include <memory>
#include <functional>
#include <array>
#include <dwmapi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <conio.h>
#include <iostream>
#include <string>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_internal.h" // gradient

// Define JSON serialization for ImGui types
#include "json.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec2, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec4, x, y, z, w)
