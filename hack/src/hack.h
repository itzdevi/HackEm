#pragma once

#include <GLFW/glfw3.h>

typedef struct Hack {
    short A;
    short D;
    unsigned short PC;
    short** ROM;
    short* RAM;
    GLFWwindow* window;
} Hack;

Hack* HackInit();
void HackFree(Hack* instance);
void HackAddCommand(Hack* instance, short* command);
void HackExecute(Hack* instance);
void* HackGetMemData(Hack* instance, unsigned short address);