include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)

FetchContent_MakeAvailable(glfw)

add_executable(hacke src/main.cpp src/emulator.cpp src/renderer.cpp src/glad.c)
target_include_directories(hacke PRIVATE include)
target_link_libraries(hacke PRIVATE common)
target_link_libraries(hacke PRIVATE glfw)

set(CMAKE_CXX_STANDARD 23)