cmake_minimum_required(VERSION 3.18)

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)

set(CMAKE_CXX_STANDARD 20)

project(intel8080-emulator CXX)

add_executable(intel8080-emulator ${PROJECT_SOURCE_DIR}/src/main.cpp
                                  ${PROJECT_SOURCE_DIR}/src/memory.cpp
)

if(CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    target_compile_DEFINITIONS(intel8080-emulator PRIVATE DEBUG)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
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
                                                      -Wstrict-aliasing=2
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(intel8080-emulator PRIVATE /permissive-
                                                      /W4
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
        #target_compile_options(intel8080-emulator PRIVATE /fsanitize=address)
    endif()
else()
    message(WARNING "No warnings specified for the chosen compiler.")
endif()

target_include_directories(intel8080-emulator PRIVATE ${PROJECT_SOURCE_DIR}/src)

######################
# SFML CONFIGURATION #
######################

set(SFML_STATIC_LIBRARIES true)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

target_link_libraries(intel8080-emulator PRIVATE sfml-graphics)