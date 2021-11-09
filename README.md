## Intel8080 Emulator

An emulator for the Intel8080 CPU written in modern C++.

### Prerequisites
- [C++20 compliant compiler](https://en.cppreference.com/w/cpp/compiler_support) (mainly for std::filesystem)
- [CMake 3.16](https://cmake.org/)
- [SFML 2.5](https://www.sfml-dev.org/tutorials/2.5/#getting-started) (static libraries)


### Building from source
- `git clone git@github.com:powware/intel8080-emulator.git`
- `mkdir build`
- `cd build`
- `cmake ..`
- `cmake --build . --parallel --config Release`

## Usage
The application needs the roms folder within it's current working directory.

### Controls
- Enter: insert coin
- (NumPad)1: select player 1
- Left: move left
- Right: move right
- Space: fire projectile

