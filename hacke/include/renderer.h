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
    unsigned int tex;
    unsigned int ssbo;
    unsigned short videoMemory[VIDEO_BUFFER_SIZE] = {};

    void MapVideoMemory(const short *memory, size_t memorySize, uint8_t *framebuffer);

public:
    Renderer();
    ~Renderer();
    void Poll();
    void Render();
    bool ShouldClose();
    void SetFramebuffer(short *buffer);
};