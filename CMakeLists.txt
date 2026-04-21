cmake_minimum_required(VERSION 3.26)
project(md2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mcpu=apple-m1 -DRELEASE")

set(CMAKE_BUILD_RPATH "/Library/Frameworks/")

find_package(SDL2_IMAGE REQUIRED)
find_package(SDL2 REQUIRED)
#include_directories(${SDL2_INCLUDE_DIRS})

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(md2
        Image.cpp
        Matrix.cpp
        Q2Model.cpp
        Pak.cpp
        main.cpp
        Pak.cpp
        Pak.h)

target_link_libraries(md2 ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} -lSDL2_image)
#target_link_libraries(md2 ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} SDL2_image::SDL2_image)
