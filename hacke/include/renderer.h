#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define VIDEO_BUFFER_SIZE 8192

class Renderer
{
private:
    GLFWwindow *window;
    unsigned int vao;
    unsigned int program;
    unsigned int ssbo;
    short *mappedVideoMemory;

public:
    Renderer();
    ~Renderer();
    void Poll();
    void Render();
    bool ShouldClose();
    void SetFramebuffer(short *buffer);
    short* GetVideoMemoryPointer();
    GLFWwindow* GetWindow() { return window; }
};