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
file(GLOB_RECURSE CHEAT_SOURCES CONFIGURE_DEPENDS "src/cheats/*.cpp")
file(GLOB_RECURSE MEMORY_SOURCES CONFIGURE_DEPENDS "src/memory/*.cpp")
file(GLOB_RECURSE OVERLAY_SOURCES CONFIGURE_DEPENDS "overlay/*.cpp")
file(GLOB_RECURSE INCLUDE_SOURCES CONFIGURE_DEPENDS "includes/src/*.cpp")
file(GLOB_RECURSE IMGUI_SOURCES CONFIGURE_DEPENDS "lib/imgui/*.cpp")


set(PROJECT_SOURCES
    src/main.cpp
    ${CHEAT_SOURCES}
    ${MEMORY_SOURCES}
    ${OVERLAY_SOURCES}
    ${INCLUDE_SOURCES}
    ${IMGUI_SOURCES}
    lib/uiaccess/uiaccess.cpp
    lib/timer/timer.cpp
)

file (GLOB_RECURSE CHEAT_HEADERS CONFIGURE_DEPENDS "src/cheats/*.h")
file (GLOB_RECURSE MEMORY_HEADERS CONFIGURE_DEPENDS "src/memory/*.h")
file (GLOB_RECURSE INCLUDE_HEADERS CONFIGURE_DEPENDS "includes/*.h")
file (GLOB_RECURSE OVERLAY_HEADERS CONFIGURE_DEPENDS "overlay/*.h")

# Define HEADER files (ONLY C++ headers that are directly used or are top-level includes)
set(PROJECT_HEADERS
    ${CHEAT_HEADERS}
    ${MEMORY_HEADERS}
    ${INCLUDE_HEADERS}
    ${OVERLAY_HEADERS}
    lib/json.h # This is fine if it's a primary header
    lib/imgui.h
    lib/ft2build.h
    lib/uiaccess/uiaccess.h
    lib/timer/timer.h
)

add_executable(CS_Cheat ${PROJECT_SOURCES})
target_sources(CS_Cheat PRIVATE ${PROJECT_HEADERS})


# Add ALL include directories (both project-specific and library-specific)
# Paths within cs-cheat are now relative to cs-cheat/ (CMAKE_CURRENT_SOURCE_DIR)
target_include_directories(CS_Cheat PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../ioctldriver/apd_api
)

# Add library directories for apd_api.lib
target_link_directories(CS_Cheat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../ioctldriver/build/api
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)


target_link_libraries(CS_Cheat PUBLIC
    apd_api.lib
    freetype.lib
    bz2.lib
    zlib.lib
    libpng16.lib
    brotlidec.lib
    brotlicommon.lib
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
    SET_TARGET_PROPERTIES(CS_Cheat PROPERTIES LINK_FLAGS "/MANIFEST:EMBED /MANIFESTINPUT:\"${CMAKE_CURRENT_SOURCE_DIR}/lib/dpiaware.manifest\" /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
endif()


# Set the target type (explicitly for GUI in Release)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET CS_Cheat PROPERTY WIN32_EXECUTABLE TRUE)
endif()

# Set the output name for the executable
set_target_properties(CS_Cheat PROPERTIES OUTPUT_NAME "cs-cheat")
