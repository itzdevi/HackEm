add_executable(hacke src/main.cpp src/emulator.cpp)
target_include_directories(hacke PRIVATE include)
target_link_libraries(hacke PRIVATE common)

set(CMAKE_CXX_STANDARD 23)