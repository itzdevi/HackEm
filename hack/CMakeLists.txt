cmake_minimum_required(VERSION 3.0.0)
project(hack)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED On)
set(CMAKE_C_EXTENSIONS Off)

set(CMAKE_C_FLAGS "-std=c11")

include_directories(src)
add_subdirectory(SDL)

file(GLOB SRC
    src/*.cpp
    src/*.h
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} SDL2::SDL2-static SDL2::SDL2main)