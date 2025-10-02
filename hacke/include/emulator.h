#pragma once

#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "renderer.h"

#define ROM_SIZE 32768
#define RAM_SIZE 24577
#define SCREEN_OFFSET 16384

class Emulator
{
private:
    short A = 0;
    short D = 0;
    unsigned short PC = 0;
    short ROM[ROM_SIZE] = {};
    short RAM[RAM_SIZE] = {};
    short *videoMemory;  // Points to GPU-mapped memory for screen
    Renderer renderer;

    void RunInstruction();

public:
    Emulator(std::vector<short> instructions);
    void Begin();
    short ReadMemory(unsigned short address);
    void WriteMemory(unsigned short address, short value);
};
