set(CMAKE_CXX_STANDARD 23)

add_executable(hacka src/main.cpp src/assembler.cpp)
target_include_directories(hacka PRIVATE include)