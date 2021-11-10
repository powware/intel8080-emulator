cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Release")
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)

set(CMAKE_CXX_STANDARD 20)

project(intel8080-emulator CXX)

add_executable(space_invaders WIN32 ${PROJECT_SOURCE_DIR}/src/space_invaders.cpp
                                    ${PROJECT_SOURCE_DIR}/src/intel8080/cpu.cpp
                                    ${PROJECT_SOURCE_DIR}/src/intel8080/register.cpp
                                    ${PROJECT_SOURCE_DIR}/src/intel8080/memory.cpp
                                    ${PROJECT_SOURCE_DIR}/src/intel8080/rom.cpp
                                    ${PROJECT_SOURCE_DIR}/src/intel8080/ram.cpp
                                    ${PROJECT_SOURCE_DIR}/src/intel8080/vram.cpp
                                    ${PROJECT_SOURCE_DIR}/src/intel8080/logger.cpp
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(space_invaders PRIVATE -Wall
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
                                                  -Wstrict-aliasing=2
    )

    if(CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
        target_compile_options(space_invaders PRIVATE -O0
                                                      -ggdb
        )
    else()
        target_compile_options(space_invaders PRIVATE -O2)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(space_invaders PRIVATE /W4
                                                  /WX
                                                  /permissive
                                                  /w14640
                                                  /w14242
                                                  /w14254
                                                  /w14263
                                                  /w14265
                                                  /w14287
                                                  /we4289
                                                  /w14296
                                                  /w14311
                                                  /w14545
                                                  /w14546
                                                  /w14547
                                                  /w14549
                                                  /w14555
                                                  /w14619
                                                  /w14640
                                                  /w14826
                                                  /w14905
                                                  /w14906
                                                  /w14928
    )

    if(CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
        target_compile_options(space_invaders PRIVATE /Od)
    else()
        target_compile_options(space_invaders PRIVATE /O2)
    endif()
else()
    message(WARNING "No warnings specified for the chosen compiler.")
endif()

if(CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    target_compile_definitions(space_invaders PRIVATE DEBUG)
endif()

target_include_directories(space_invaders PRIVATE ${PROJECT_SOURCE_DIR}/src/intel8080)

######################
# SFML CONFIGURATION #
######################

set(SFML_STATIC_LIBRARIES true)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

target_link_libraries(space_invaders PRIVATE sfml-graphics)