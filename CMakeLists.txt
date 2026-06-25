cmake_minimum_required(VERSION 3.15)
project(rubixed C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Suppress the extracted timestamp policy warnings 
cmake_policy(SET CMP0135 NEW)

# 1. Fetch and compile Raylib 5.0
include(FetchContent)
FetchContent_Declare(
  raylib
  URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Crucial Fix: Exclude Raylib internal templates from your target directory scope
set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# 2. Scope only your project files to avoid picking up internal Raylib templates
add_executable(rubixed)

target_sources(rubixed PRIVATE
    main.c
    cube/cube.c
    cube/draw.c
    cube/anim.c
    scramble/scramble.c
    scramble/scramble_engine.c
    app/app.c
    app/theme.c
    app/app_modes.c
    app/app_ui.c
    ui/ui.c
    camera/camera.c
    timer/timer.c
    solver/solver.c
    storage/log.c
    app/utils/utils.c
    app/utils/table.c
    app/utils/stats.c
    app/utils/projection.c
    ckociemba/coordcube.c
    ckociemba/cubiecube.c
    ckociemba/facecube.c
    ckociemba/search.c
    ckociemba/prunetable_helpers.c
)

# 3. Include Header Directories
target_include_directories(rubixed PRIVATE ./ckociemba/include)

# 4. Link Dependencies
target_link_libraries(rubixed PRIVATE raylib)

if(WIN32)
    # Direct system linking for clean MSVC Compilation
    target_link_libraries(rubixed PRIVATE winmm gdi32 opengl32)
elseif(APPLE)
    target_link_libraries(rubixed PRIVATE "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(LINUX)
    target_link_libraries(rubixed PRIVATE m pthread dl rt X11)
endif()