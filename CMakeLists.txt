# C:/Users/581/source/repos/0x581/cs-cheat/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
project(CS_Cheat_Project CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define common build directories (relative to the build directory created by CMake Tools)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}")

# Define the SOURCE files (those that compile to .obj)
set(PROJECT_SOURCES
    src/main.cpp
    overlay/src/overlay.cpp
    lib/timer/timer.cpp
    src/cheats/src/antiFlash.cpp
    src/cheats/src/bunnyHop.cpp
    src/cheats/src/bombTimer.cpp
    src/cheats/src/fovChanger.cpp
    src/cheats/src/radarHack.cpp
    src/cheats/src/triggerBot.cpp
    src/cheats/src/esp.cpp
    includes/src/strings.cpp
    includes/src/console_control.cpp
    includes/src/shared_variables.cpp
    src/cheats/src/aimBot.cpp
    includes/src/config.cpp
    src/memory/memory.cpp
    lib/uiaccess/uiaccess.cpp
    overlay/src/drawing.cpp
    src/cheats/src/rageBot.cpp
    src/cheats/cheats.cpp
    src/cheats/cheat_data.cpp
    src/cheats/check_cheat_should_run.cpp
    includes/src/entity_system.cpp
    overlay/src/window_handle.cpp
    overlay/src/drawing/menu.cpp
    overlay/src/drawing/bomb.cpp
    src/memory/map_driver.cpp
    lib/imgui/imgui_impl_dx11.cpp
    lib/imgui/imgui_impl_win32.cpp
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/misc/cpp/imgui_stdlib.cpp
    lib/imgui/misc/fonts/binary_to_compressed_c.cpp
    overlay/src/drawing/cheat_ui.cpp
    overlay/src/init_imgui.cpp
    includes/src/keyboard.cpp
    includes/src/hotkeys.cpp
)

# Define HEADER files (ONLY C++ headers that are directly used or are top-level includes)
set(PROJECT_HEADERS
    includes/entity_system.h
    includes/console_control.h
    includes/cs_defines.h
    includes/hotkeys.h
    includes/keyboard.h
    lib/imgui/imgui_impl_dx11.h
    lib/imgui/imgui_impl_win32.h
    lib/imgui/imconfig.h
    lib/imgui/imgui.h
    lib/imgui/imgui_internal.h
    lib/imgui/imstb_rectpack.h
    lib/imgui/imstb_textedit.h
    lib/imgui/misc/cpp/imgui_stdlib.h
    lib/json.h # This is fine if it's a primary header
    lib/json/include/nlohmann/json.hpp # This is the primary single header for nlohmann json
    lib/json/include/nlohmann/json_fwd.hpp # Forward declarations header
    lib/json/single_include/nlohmann/json.hpp # If you use the single include version
    lib/json/single_include/nlohmann/json_fwd.hpp # If you use the single include version fwd
    lib/uiaccess/uiaccess.h
    overlay/window_handle.h
    src/cheats/cheats.h
    includes/config.h
    includes/delays.h
    includes/files.h
    includes/modules.h
    includes/offsets.h
    includes/boneData.h
    includes/colors.h
    overlay/overlay.h
    lib/imgui.h
    lib/timer/timer.h
    includes/vector_math.h
    src/cheats/cheat_data.h
    src/cheats/check_cheat_should_run.h
    src/cheats/src/cheat_entries.h
    src/memory/map_driver.h
    src/memory/memory.h
    includes/shared_variables.h
    includes/strings.h
    # IMPORTANT: Removed all 'lib/json/include/nlohmann/detail/' headers from here.
    # These are internal headers of the single-header JSON library and should not be
    # listed individually as sources for the target. They are pulled in by json.hpp itself.
    # Also ensure no non-C++ files like .natvis, .py, .txt, .ttf are in this list.
)

add_executable(CS_Cheat ${PROJECT_SOURCES})
target_sources(CS_Cheat PRIVATE ${PROJECT_HEADERS})

# Add ALL include directories (both project-specific and library-specific)
# Paths within cs-cheat are now relative to cs-cheat/ (CMAKE_CURRENT_SOURCE_DIR)
target_include_directories(CS_Cheat PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/json/include # Still important for #include <nlohmann/json.hpp>
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/json/single_include # Also important if you use this
    # Path for ioctldriver: From cs-cheat (CMAKE_CURRENT_SOURCE_DIR), go up one (to 0x581/), then down to ioctldriver/apd_api/
    ${CMAKE_CURRENT_SOURCE_DIR}/../ioctldriver/apd_api
)

# Add library directories for apd_api.lib
target_link_directories(CS_Cheat PRIVATE
    # Path for ioctldriver: From cs-cheat, go up one (to 0x581/), then down to ioctldriver/build/api/
    ${CMAKE_CURRENT_SOURCE_DIR}/../ioctldriver/build/api
)

target_link_libraries(CS_Cheat PUBLIC
    apd_api.lib
    winmm.lib
    d3d11.lib
    dxgi.lib
    user32.lib
    gdi32.lib
    shell32.lib
)

# Compiler definitions
target_compile_definitions(CS_Cheat PRIVATE
    UNICODE  # Use Unicode!!!
    _UNICODE  # Use Unicode!!!
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
endif()

# Set compiler options
target_compile_options(CS_Cheat PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

# Linker flags for the executable
if (WIN32)
    target_link_options(CS_Cheat PRIVATE
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
    )
endif()

# UAC Execution Level (Windows-specific)
if (WIN32)
    target_link_options(CS_Cheat PRIVATE "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
endif()

# Set the target type (explicitly for GUI in Release)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET CS_Cheat PROPERTY WIN32_EXECUTABLE TRUE)
endif()

# Set the output name for the executable
set_target_properties(CS_Cheat PROPERTIES OUTPUT_NAME "cs-cheat")