set(CMAKE_CXX_STANDARD 23)

add_executable(hacke src/main.cpp src/emulator.cpp)
target_include_directories(hacke PRIVATE include)