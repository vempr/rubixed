cmake_minimum_required(VERSION 3.15)

project(rubixed C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---- Compiler portability flags ----
if(MSVC)
    # MSVC equivalent of C99 behavior
    add_compile_options(/std:c17)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-std=c99)
endif()

if(MSVC)
  add_compile_options(/wd4244 /wd4305)
endif()

# ---- FetchContent ----
include(FetchContent)

FetchContent_Declare(
  raylib
  URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# ---- Disable raylib extras safely ----
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_PROJECTS OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# ---- Executable ----
add_executable(rubixed
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

target_include_directories(rubixed PRIVATE
./ckociemba/include
)

target_link_libraries(rubixed PRIVATE raylib)

# ---- Platform libs ----
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