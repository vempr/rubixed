# rubixed

Rubixed is a cross-platform (Windows, Linux, MacOS) Rubik's Cube visualizer, solver and timer written in C99. Utilizes the Raylib library for smooth 3D rendering and an implementation of the Kociemba Two-Phase Algorithm.

Demo: https://youtube.com/shorts/k43liRs40W4

## Controls

### General

- <kbd>enter</kbd>: generate new scramble/start fresh session
- <kbd>backspace</kbd>: reset cube (remove scramble and animation state)
- <kbd>q</kbd>: mark current solve as DNF
- <kbd>space</kbd>: timer control (inspection, start, stop depending on mode)

### Mode switching

- <kbd>1</kbd>: freestyle mode (no timer, no saving)
- <kbd>2</kbd>: instant solve mode (generate scramble + <kbd>space</kbd> to solve)
- <kbd>3</kbd>: physical solve mode (like CSTimer but in 3D)
- <kbd>4</kbd>: virtual solve mode (like physical solve mode but with your keyboard)

### Cube moves (virtual, free modes)

- U (up face):
  - <kbd>F</kbd> -> U
  - <kbd>J</kbd> -> U'
- D (down face):
  - <kbd>S</kbd> -> D
  - <kbd>W</kbd> -> D'
- L (left face):
  - <kbd>E</kbd> -> L
  - <kbd>D</kbd> -> L'
- R (right face):
  - <kbd>I</kbd> -> R
  - <kbd>K</kbd> -> R'
- F (front face):
  - <kbd>G</kbd> -> F
  - <kbd>H</kbd> -> F'
- B (back face):
  - <kbd>R</kbd> -> B
  - <kbd>Y</kbd>/<kbd>Z</kbd> -> B' (physical key, not mapped)

### Whole cube rotation

Hold <kbd>shift</kbd>:

- <kbd>left shift</kbd> + <kbd>I</kbd> -> x
- <kbd>left shift</kbd> + <kbd>F</kbd> -> y
- <kbd>left shift</kbd> + <kbd>H</kbd> -> z

- <kbd>right shift</kbd> + <kbd>I</kbd> -> x'
- <kbd>right shift</kbd> + <kbd>F</kbd> -> y'
- <kbd>right shift</kbd> + <kbd>H</kbd> -> z'

(' means counterclockwise; CCW in cubing just means a turn in the direction opposite of the normed CW move)

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

## Contributions

Feel free to make pull requests and spice rubixed up a bit! I invested more time towards the functionality of this project, so it is definitely lacking somewhat in terms of aesthetics and customizability. If you are interested, please work on the `functional` branch and not the `main` branch. The `main` branch captured my first time building a C project perfectly and I just can't bring myself to wipe them..