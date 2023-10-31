cmake_minimum_required(VERSION 3.0.0)
project(hackc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED On)
set(CMAKE_C_EXTENSIONS Off)

set(CMAKE_C_FLAGS "-std=c11")

include_directories(src)

file(GLOB SRC
    src/*.cpp
    src/*.h
)

add_executable(${PROJECT_NAME} ${SRC})