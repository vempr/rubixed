cmake_minimum_required(VERSION 3.15)
project(rubixed C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---- FetchContent Setup ----
include(FetchContent)
FetchContent_Declare(
  raylib
  URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# ---- IMPORTANT: Disable Raylib extras ----
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_GAMES OFF CACHE BOOL "" FORCE)

# FIX: Force CMake to bypass the Raylib < 3.5 minimum requirement error
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

FetchContent_MakeAvailable(raylib)

# ---- Executable ----
add_executable(rubixed)

# FIX: Explicitly prepend ${CMAKE_CURRENT_SOURCE_DIR}/ to stop CMake 
# from grabbing duplicate 'main.c' files from Raylib's Android project folders.
target_sources(rubixed PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cube/cube.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cube/draw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cube/anim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/scramble/scramble.c
    ${CMAKE_CURRENT_SOURCE_DIR}/scramble/scramble_engine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/theme.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/app_modes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/app_ui.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/ui.c
    ${CMAKE_CURRENT_SOURCE_DIR}/camera/camera.c
    ${CMAKE_CURRENT_SOURCE_DIR}/timer/timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/storage/log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/utils/utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/utils/table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/utils/stats.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/utils/projection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ckociemba/coordcube.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ckociemba/cubiecube.c
    ${CMAKE_CURRENT_SOURCE_DIR}/facecube.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ckociemba/search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ckociemba/prunetable_helpers.c
)

# ---- Include dirs ----
target_include_directories(rubixed PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ckociemba/include
)

# ---- Link Raylib ----
target_link_libraries(rubixed PRIVATE raylib)

# ---- Platform libraries ----
if(WIN32)
    target_link_libraries(rubixed PRIVATE winmm gdi32 opengl32)
elseif(APPLE)
    target_link_libraries(rubixed PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(rubixed PRIVATE m pthread dl rt X11)
endif()