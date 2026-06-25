cmake_minimum_required(VERSION 3.14)
project(rubixed C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 1. Automatically fetch and build Raylib cross-platform
include(FetchContent)
FetchContent_Declare(
  raylib
  URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# 2. Collect all your C source files
file(GLOB_RECURSE SOURCES
    "main.c"
    "cube/*.c"
    "scramble/*.c"
    "app/*.c"
    "ui/*.c"
    "camera/*.c"
    "timer/*.c"
    "solver/*.c"
    "storage/*.c"
    "ckociemba/*.c"
)

# 3. Define the executable target
add_executable(rubixed ${SOURCES})

# 4. Include paths
target_include_directories(rubixed PRIVATE ./ckociemba/include)

# 5. Link Raylib (and mandatory platform-specific system graphics libraries)
target_link_libraries(rubixed PRIVATE raylib)

if(WIN32)
    # Ensures a clean static Windows compilation
    target_link_libraries(rubixed PRIVATE winmm gdi32 opengl32 paths)
elseif(APPLE)
    target_link_libraries(rubixed PRIVATE "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(LINUX)
    target_link_libraries(rubixed PRIVATE m pthread dl rt X11)
endif()