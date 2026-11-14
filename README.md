# Merkur CS2

[![C++20](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/Build-CMake-orange.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20x64-informational.svg)](https://microsoft.com)
[![DirectX](https://img.shields.io/badge/Rendering-DirectX%2011-success.svg)](https://docs.microsoft.com/en-us/windows/win32/direct3d11/atcanc-dx-graphics-direct3d-11)
[![Dear ImGui](https://img.shields.io/badge/UI-Dear%20ImGui-red.svg)](https://github.com/ocornut/imgui)

A high-performance, modular **Counter-Strike 2** external overlay and assistance tool written in modern C++20. Designed with a clean multi-threaded architecture, kernel-mode memory abstraction, flexible window management, and a Dear ImGui DirectX 11 interface.

---

## 📑 Table of Contents

- [Overview](#overview)
- [Architecture & Design](#architecture--design)
- [Key Features](#key-features)
  - [Visuals (ESP)](#visuals-esp)
  - [Aim Assistance](#aim-assistance)
  - [Game Utilities & Misc](#game-utilities--misc)
  - [Overlay & Windowing](#overlay--windowing)
  - [Config & Keybind System](#config--keybind-system)
- [Offset Management](#offset-management)
- [Project Structure](#project-structure)
- [Prerequisites & Building](#prerequisites--building)
  - [Requirements](#requirements)
  - [External Dependencies](#external-dependencies)
  - [Build Steps](#build-steps)
- [Default Keybindings](#default-keybindings)
- [Disclaimer](#disclaimer)

---

## 🔎 Overview

**Merkur CS2** is an external tool that reads game state and renders overlays without injecting DLLs into the game process:

- **External Memory Access:** Interacts with CS2 via an IOCTL kernel-mode driver (`ioctldriver`), bypassing standard user-mode handle detection (`OpenProcess`).
- **Hardware-Accelerated Overlay:** Powered by DirectX 11 and Dear ImGui (with FreeType font rendering) for maximum frame rates and low latency.
- **Dynamic Window Hijacking / UIAccess:** Bypasses fullscreen exclusive/borderless window capture using a tiered fallback strategy (UIAccess band $\to$ NVIDIA GeForce Overlay hook $\to$ Topmost layered window).
- **Automated Offset Pipeline:** Python tooling parses and generates updated offsets directly from public CS2 schema dumpers.

---

## 🏛️ Architecture & Design

```
                     ┌───────────────────────────────┐
                     │          Counter-Strike 2     │
                     │          (cs2.exe / 64-bit)   │
                     └───────────────▲───────────────┘
                                     │
                 Kernel Read/Write   │  (IOCTL Driver)
                                     ▼
                        ┌─────────────────────────┐
                        │     Kernel Driver       │
                        │    (via apd_api.lib)    │
                        └────────────▲────────────┘
                                     │
           ┌─────────────────────────┴─────────────────────────┐
           │                                                   │
┌──────────┴───────────────┐                       ┌───────────┴───────────────┐
│     Cheat Thread         │                       │     Overlay Thread        │
│ • Entity System Loop     │ ──[ Thread-Safe ]───▶ │ • DirectX 11 Render Loop  │
│ • Bone & Hitbox Tracking │      Render Data      │ • ImGui Menu Interface    │
│ • Aimbot / Triggerbot    │                       │ • ESP & Skeleton Draw     │
│ • Bunnyhop & Misc Logic  │                       │ • Bomb Timer HUD          │
└──────────────────────────┘                       └───────────────────────────┘
```

1. **Cheat Worker Thread:** Runs a decoupled loop that reads memory through the driver, parses the entity list, updates bone matrices, calculates screen coordinates, and processes active cheat features (aimbot, triggerbot, bunnyhop).
2. **Overlay Worker Thread:** Drives DirectX 11 rendering and ImGui menus. It consumes thread-safe snapshot data produced by the cheat thread.
3. **Low-Level Input Thread:** Listens for hotkeys independently of the game message loop to ensure responsive toggling.

---

## ✨ Key Features

### 👁️ Visuals (ESP)
- **3D Skeleton Bones:** Full-body skeletal tracking (Head, Neck, Spine, Arms, Wrists, Hands, Hips, Legs, Feet) scaled dynamically by distance.
- **2D Bounding Boxes:** Dynamic bounding boxes with optional animated pulsating glow effect.
- **Player Information:** Configurable player names, health bars, and weapon/item labels.
- **Team & Enemy Filters:** Separate visibility toggles and customizable RGBA color palettes for visible and occluded enemies/teammates.

### 🎯 Aim Assistance
- **Aimbot:** Smooth aim assistance with configurable field of view (FOV), speed factors, and hitbox targeting (Head, Neck, Chest, etc.).
- **FOV Circle Indicator:** Configurable visual indicator for the active aim assistance radius.
- **Visibility Checks:** Filters targets based on whether they are occluded or spotted.
- **TriggerBot:** Automated shot execution upon crosshair intersection with customizable pre-shot delay.
- **RCS (Recoil Control System):** Compensates for weapon recoil angles dynamically.
- **RageBot:** High-speed target acquisition mode.

### 🛠️ Game Utilities & Misc
- **C4 Bomb HUD:** Floating HUD showing planted status, bomb site (A / B), exact seconds until detonation, defusal progress, and defuser identifier.
- **BunnyHop:** Automated jump execution based on ground-state flags.
- **AntiFlash:** Suppresses flashbang blindness duration in real-time.
- **In-Game Radar:** Sets spotted flags so all enemy players appear on the radar.
- **FOV Changer:** Adjusts camera field-of-view beyond default limits.

### 🪟 Overlay & Windowing
The overlay utilizes a 3-tier fallback strategy implemented in `window_handle.hpp`:
1. **UIAccess:** Uses `PrepareForUIAccess` and undocumented `CreateWindowInBand` (Band 2) to display above topmost and elevated game windows.
2. **NVIDIA GeForce Overlay Hook:** Hijacks `CEF-OSC-WIDGET` when GeForce Experience overlay is present.
3. **Transparent Topmost Window:** Standard borderless topmost layered window fallback.

### ⚙️ Config & Keybind System
- **JSON Profile Management:** Profiles are saved to and loaded from `%localappdata%/Merkur/CS2/`.
- **Interactive Hotkey Editor:** Real-time keybinding reconfiguration directly inside the ImGui menu.
- **Persistent Preferences:** Remembers window positions, UI scaling, and FPS limiter settings across sessions.

---

## 🔄 Offset Management

Valve updates Counter-Strike 2 frequently. The project includes an automated Python script to fetch the latest offsets and schema values directly from public dumpers (`a2x/cs2-dumper`):

```bash
# Run the offset fetcher
python offset_script/fetchOffsets.py
```

This updates `includes/offsets.hpp` with up-to-date offsets for:
- Local player pawn & controller
- Entity list & view matrix
- Game rules & planted C4
- Bone & model matrices
- Button input states

---

## 📁 Project Structure

```
├── CheatBase/                     # Reusable framework (overlay, memory, input, config)
│   ├── lib/                       # Third-party libraries (ImGui, FreeType, nlohmann/json)
│   ├── src/                       # Memory driver connector, DirectX 11 overlay, keybinds
│   └── CMakeLists.txt             # Static library build definition
├── includes/                      # Project headers
│   ├── cheats/                    # Cheat data structures and interfaces
│   ├── offsets.hpp                # Game offsets (auto-generated)
│   ├── config.hpp                 # Serialization and settings definitions
│   └── entity_system.hpp          # Entity list parsing logic
├── offset_script/                 # Offset updater
│   ├── fetchOffsets.py            # Main offset fetcher script
│   ├── offsets.py                 # Offset definitions & targets
│   └── parse_file.py              # C++ header parser for schema dump output
├── src/                           # Implementation files
│   ├── cheats/                    # Cheat implementations (ESP, AimBot, BunnyHop, etc.)
│   ├── Overlay/                   # ImGui menu, HUDs, and drawing routines
│   ├── config.cpp                 # Configuration management
│   ├── entity_system.cpp          # Entity enumeration logic
│   └── render_data.cpp            # Snapshot synchronization
├── .vscode/                       # VS Code tasks & launch configurations
├── CMakeLists.txt                 # Main CMake project configuration
└── main.cpp                       # Application entrypoint & thread coordinator
```

---

## 🔨 Prerequisites & Building

### Requirements
- **Operating System:** Windows 10 or Windows 11 (64-bit)
- **Compiler:** Microsoft Visual C++ (MSVC) v143 or newer (Visual Studio 2022 recommended)
- **CMake:** Version 3.16 or newer
- **Windows SDK:** 10.0.26100.0 or compatible
- **Python:** 3.10+ (for offset script, requires `requests`)

### External Dependencies
This project links against **`apd_api.lib`** from the sibling project [`ioctldriver`](https://github.com/0x581/ioctldriver).
Ensure `ioctldriver` is cloned alongside this repository:

```
repos/
├── ioctldriver/       # Kernel driver & apd_api
│   ├── apd_api/
│   └── build/api/
└── cs2/               # This repository
```

### Build Steps

#### Option A: Command Line (CMake)

```bash
# Clone the repository
git clone https://github.com/0x581/cs-cheat.git cs2
cd cs2

# Generate build configuration
cmake -B build -G "Visual Studio 17 2022" -A x64

# Compile Release configuration
cmake --build build --config Release
```

The compiled binary will be located at `build/output/Release/cs-cheat.exe`.

#### Option B: Visual Studio Code
1. Open the repository folder in VS Code.
2. Install the **C/C++** and **CMake Tools** extensions.
3. Run the predefined task (`Ctrl+Shift+B` or Run Task $\to$ `cmake-build-release`).

---

## ⌨️ Default Keybindings

| Function | Default Key | Description |
|---|---|---|
| **Toggle Menu** | `Page Up` (`VK_PRIOR`) | Opens / closes the ImGui configuration menu |
| **Quick Exit** | `Page Down` (`VK_NEXT`) | Immediately shuts down overlay and threads |
| **AimBot** | `Right Mouse Button` | Engages aimbot when held |
| **TriggerBot** | `Right Mouse Button` | Engages triggerbot when held |
| **BunnyHop** | `Space` | Activates automated jump timing |

*All keybindings can be rebound at any time in the in-game menu under the **Keybinds** tab.*

---

## ⚠️ Disclaimer

> [!WARNING]
> This software is created solely for **educational, reverse-engineering, and research purposes**. Using automated tools or modifying game memory on official servers protected by Valve Anti-Cheat (VAC) or third-party anti-cheat solutions is a violation of the Steam Subscriber Agreement and will result in permanent account bans. The authors do not endorse or encourage cheating in multiplayer games.
