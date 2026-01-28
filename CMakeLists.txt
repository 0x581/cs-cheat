cmake_minimum_required(VERSION 3.16)

# 1. Define the new configuration type BEFORE project()
# Add "DebugNoInputHook" to the list of supported configurations
set(CMAKE_CONFIGURATION_TYPES "Release;Debug;DebugNoInputHook" CACHE STRING "Supported build configurations" FORCE)

project(CS_Cheat_Project CXX)

# *** BUILD-TIME COMPILER CHECK ***
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(FATAL_ERROR "This project requires the MSVC compiler. Detected compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()
# ********************************

set(IS_DEBUG_LIKE_CONFIG "$<OR:$<CONFIG:Debug>,$<CONFIG:DebugNoInputHook>>")

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)


# Define common build directories (relative to the build directory created by CMake Tools)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output")

# Define the SOURCE files (those that compile to .obj)
file(GLOB_RECURSE CHEAT_SOURCES CONFIGURE_DEPENDS "cheats/*.cpp")
file(GLOB_RECURSE OVERLAY_SOURCES CONFIGURE_DEPENDS "overlay/*.cpp")
file(GLOB_RECURSE INCLUDE_SOURCES CONFIGURE_DEPENDS "includes/*.cpp")


set(PROJECT_SOURCES
    main.cpp
    ${CHEAT_SOURCES}
    ${OVERLAY_SOURCES}
    ${INCLUDE_SOURCES}
)

file (GLOB_RECURSE CHEAT_HEADERS CONFIGURE_DEPENDS "cheats/*.hpp")
file (GLOB_RECURSE INCLUDE_HEADERS CONFIGURE_DEPENDS "includes/*.hpp")
file (GLOB_RECURSE OVERLAY_HEADERS CONFIGURE_DEPENDS "overlay/*.hpp")

# Define HEADER files (ONLY C++ headers that are directly used or are top-level includes)
set(PROJECT_HEADERS
    ${CHEAT_HEADERS}
    ${INCLUDE_HEADERS}
    ${OVERLAY_HEADERS}
)

add_executable(CS_Cheat ${PROJECT_SOURCES})
target_sources(CS_Cheat PRIVATE ${PROJECT_HEADERS})

add_subdirectory(CheatBase)
target_link_libraries(CS_Cheat PRIVATE CheatBase)


# Add ALL include directories (both project-specific and library-specific)
# Paths within cs-cheat are now relative to cs-cheat/ (CMAKE_CURRENT_SOURCE_DIR)
target_include_directories(CS_Cheat PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

# Compiler definitions
target_compile_definitions(CS_Cheat PRIVATE
    $<${IS_DEBUG_LIKE_CONFIG}:DEBUG>
    # Disable input hook for this config
    $<$<CONFIG:DebugNoInputHook>:DISABLE_INPUT_HOOK>
)

# Set compiler options
target_compile_options(CS_Cheat PRIVATE
    /permissive-         # MSVC: Enforce strict C++ compliance
    /W3                 # MSVC: Warning level 3

    # Explicitly set debug compiler options for both Debug and DebugNoInputHook
    $<${IS_DEBUG_LIKE_CONFIG}:/Zi>   # Generate Program Database debug information
    $<${IS_DEBUG_LIKE_CONFIG}:/Od>   # Disable optimization
    $<${IS_DEBUG_LIKE_CONFIG}:/RTC1> # Enable runtime checks
    $<${IS_DEBUG_LIKE_CONFIG}:/MDd>  # Link to debug DLL runtime (Multithreaded Debug DLL)
    # $<${IS_DEBUG_LIKE_CONFIG}:/JMC>  # Enable Just My Code debugging
    # $<${IS_DEBUG_LIKE_CONFIG}:/GF->  # Disable Eliminate Duplicate Strings
    # $<${IS_DEBUG_LIKE_CONFIG}:/Gy->  # Disable Enable Function-Level Linking
    # $<${IS_DEBUG_LIKE_CONFIG}:/Gm->  # Disable Minimal Rebuild
)

# Linker flags for the executable
target_link_options(CS_Cheat PRIVATE
    $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
    # Apply /NODEFAULTLIB:LIBCMT for Release builds (if needed)
    $<$<CONFIG:Release>:/NODEFAULTLIB:LIBCMT>

    $<${IS_DEBUG_LIKE_CONFIG}:/SUBSYSTEM:CONSOLE>

    # Explicitly set debug linker options for both Debug and DebugNoInputHook
    $<${IS_DEBUG_LIKE_CONFIG}:/DEBUG>      # Generate debug info
    $<${IS_DEBUG_LIKE_CONFIG}:/INCREMENTAL> # Enable incremental linking
    $<${IS_DEBUG_LIKE_CONFIG}:/OPT:NOREF>
    # $<${IS_DEBUG_LIKE_CONFIG}:/PDB:"$<TARGET_FILE_DIR:CS_Cheat>/$<TARGET_FILE_NAME:CS_Cheat>.pdb"> # Specify PDB path

)


# Set the target type (explicitly for GUI in Release)
# WIN32_EXECUTABLE is true for Windows GUI applications. Console for Debug/DebugNoInputHook.
set_property(TARGET CS_Cheat PROPERTY WIN32_EXECUTABLE "$<CONFIG:Release>")


# Set Whole Program Optimization (LTCG) for Release builds
set_property(TARGET CS_Cheat PROPERTY CMAKE_INTERPROCEDURAL_OPTIMIZATION "$<$<CONFIG:Release>:TRUE>")



# Set the output name for the executable
set_target_properties(CS_Cheat PROPERTIES OUTPUT_NAME "cs-cheat")
