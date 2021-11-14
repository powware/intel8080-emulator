## Intel8080 Emulator

An emulator for the Intel8080 CPU written in modern C++. Currently only setup to run the arcade game Space Invaders.

### Prerequisites
- [C++20 compliant compiler](https://en.cppreference.com/w/cpp/compiler_support) (mainly for std::filesystem)
- [CMake 3.16](https://cmake.org/)
- [SFML 2.5](https://www.sfml-dev.org/tutorials/2.5/#getting-started) (static libraries)

### Building from source
- `git clone https://github.com/powware/intel8080-emulator.git`
- `mkdir build`
- `cd build`
- `cmake ..`
- `cmake --build . --parallel --config Release`

### Usage
For the application to load the roms you need to have the roms folder within it's working directory.

#### Controls
- Enter: insert coin
- (NumPad)1: select player 1
- Left: move left
- Right: move right
- Space: fire projectile

### Points of Improvement
- better separation of ports via an interface/class (similar to ram, rom, vram)
- make SFML optional when not compiling for space invaders using the method mentioned above
- use intel8080 tests to valdiate correctness
- debugability/logging
