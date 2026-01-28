# CheatBase Library CMakeLists.txt
# This library encapsulates the cheating functionality and can be easily integrated into a parent project

# Define the SOURCE files (those that compile to .obj)
file(GLOB_RECURSE CHEAT_BASE_LIB_IMGUI_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/*.cpp")
file(GLOB_RECURSE CHEAT_BASE_LIB_UIACCESS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lib/uiaccess/*.cpp")
file(GLOB_RECURSE CHEAT_BASE_LIB_TIMER_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lib/timer/*.cpp")
file(GLOB_RECURSE CHEAT_BASE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Collect all headers
file(GLOB_RECURSE CHEAT_BASE_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

set(CHEAT_BASE_SOURCES
    ${CHEAT_BASE_LIB_IMGUI_SOURCES}
    ${CHEAT_BASE_LIB_UIACCESS_SOURCES}
    ${CHEAT_BASE_LIB_TIMER_SOURCES}
    ${CHEAT_BASE_SOURCES}
)

# Create the CheatBase library
add_library(CheatBase STATIC 
    ${CHEAT_BASE_SOURCES}
    ${CHEAT_BASE_HEADERS}
)

# Public include directories that consumers of this library need
target_include_directories(CheatBase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
)

# Private include directories (only used internally)
target_include_directories(CheatBase PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay
)

target_include_directories(CheatBase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ioctldriver/apd_api
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)


# Add library directories for apd_api.lib
target_link_directories(CS_Cheat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ioctldriver/build/api
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/lib files"
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

# Set output properties
set_target_properties(CheatBase PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
)


# Compiler definitions
target_compile_definitions(CheatBase PUBLIC
    UNICODE  # Use Unicode!!!
    _UNICODE  # Use Unicode!!!
)


# Manifest embedding for Windows (admin privileges and DPI awareness)
if(WIN32)
    set_target_properties(CheatBase PROPERTIES
        LINK_FLAGS "/MANIFEST:EMBED /MANIFESTINPUT:\"${CMAKE_CURRENT_SOURCE_DIR}/lib/dpiaware.manifest\" /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
    )
endif()
