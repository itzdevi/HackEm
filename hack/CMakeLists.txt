cmake_minimum_required(VERSION 3.0.0)
project(hack LANGUAGES C CXX) 

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include_directories(
    src
    include
)
add_subdirectory(
    GLFW
)

file(GLOB SRC
    src/*.cpp
    src/*.c
    src/*.h
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} glfw)