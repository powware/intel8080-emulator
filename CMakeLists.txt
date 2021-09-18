cmake_minimum_required(VERSION 3.18)

set(CXX_STANDARD 20)

project(intel8080-emulator CXX)

add_executable(intel8080-emulator ${PROJECT_SOURCE_DIR}/src/main.cpp
                                  ${PROJECT_SOURCE_DIR}/src/rom.cpp
)

target_compile_options(intel8080-emulator PRIVATE -Wall
                                                  -Wextra
                                                  -pedantic
                                                  -Wpedantic
                                                  -Werror
                                                  -Wnon-virtual-dtor
                                                  -Wshadow
                                                  -Wold-style-cast
                                                  -Wcast-align
                                                  -Wunused
                                                  -Woverloaded-virtual
                                                  -Wformat=2
                                                  -Wdouble-promotion
                                                  -Wuseless-cast
                                                  -Wnull-dereference
                                                  -Wlogical-op
                                                  -Wduplicated-branches
                                                  -Wduplicated-cond 
                                                  -Wmisleading-indentation
                                                  -Wconversion
                                                  -Wsign-conversion
                                                  -fno-exceptions
                                                  -fno-unwind-tables
                                                  -fno-asynchronous-unwind-tables
)

target_include_directories(intel8080-emulator PRIVATE ${PROJECT_SOURCE_DIR}/src)