# rubixed

Rubixed is a cross-platform (Windows, Linux, MacOS) Rubik's Cube visualizer, solver and timer written in C99. Utilizes the Raylib library for smooth 3D rendering and an implementation of the Kociemba Two-Phase Algorithm.

Demo: https://youtube.com/shorts/k43liRs40W4

## Features & tools

- Real time 3D visualizer with smooth rotational animations and customizable camera positions
- Four different modes (free, instant, physical, virtual) for all types of cubers
- Two-phase optimal solver capable of generating efficient solutions in milliseconds
- Integrated speedcubing metrics (session log, statistical analysis, calculations); WCA compliant (+2, DNF, AO5, AO12)
- Locally persistant solve data, making Rubixed suitable for tracking cubing improvement
- Simultaneous 2D projection of the cube's state for complete orientation
- Cross-platform compatibility via CMake build infra

## Build & Compiling

CMake orchestrates Rubixed's build chain and it's core dependencies. You do not need to install raylib manually on your system - ensure you have a standard C compiler and CMake installed.

```
git clone --recursive https://github.com/vempr/rubixed.git
cd rubixed

cmake -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release
```

## Libraries

- raylib: https://github.com/raysan5/raylib
- muodov/kociemba: https://github.com/muodov/kociemba/