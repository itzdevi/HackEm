#include "renderer.h"

#include <stdlib.h>
#include <print>

#include "glad/glad.h"

const char *vertexShaderSrc = R"glsl(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;
out vec2 texCoord;
void main() {
    texCoord = aTex;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)glsl";

const char *fragmentShaderSrc = R"glsl(
#version 330 core

#define VIDEO_MEMORY_SIZE 8192
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 256

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D screenTex;


layout(std430, binding = 1) buffer Memory {
    uint memory[];
};

void main() {
    static const uint8_t bitToByteTable[2] = {0, 255};
    size_t charIndex = 0;

    // Flip vertically
    float c = 1 - texture(screenTex, vec2(texCoord.x, 1.0 - texCoord.y)).r;
    FragColor = vec4(c, c, c, 1.0);
}
)glsl";

const float quadVertices[] = {
    // positions   // tex coords
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 1.0f};

const unsigned int quadIndices[] = {0, 1, 2, 2, 3, 0};

Renderer::Renderer()
{
    if (!glfwInit())
    {
        std::println("Could not initialize glfw");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(512, 256, "Hack", nullptr, nullptr);
    if (!window)
    {
        std::println("Could not initialize window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, VIDEO_BUFFER_SIZE * sizeof(unsigned short), videoMemory, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    unsigned int vbo,
        ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 256, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Renderer::~Renderer()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::Poll()
{
    glfwPollEvents();
}

void Renderer::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(program, "screenTex"), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}

bool Renderer::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void Renderer::SetFramebuffer(short *buffer)
{
    uint8_t framebuffer[512 * 256];
    MapVideoMemory(buffer, VIDEO_BUFFER_SIZE, framebuffer);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 256, GL_RED, GL_UNSIGNED_BYTE, framebuffer);
}

void Renderer::MapVideoMemory(const short *memory, size_t memorySize, uint8_t *framebuffer)
{
    // Pre-computed lookup table for byte values (0 or 255)
    static const uint8_t bitToByteTable[2] = {0, 255};

    size_t charIndex = 0;

    for (size_t i = 0; i < memorySize; ++i)
    {
        short value = memory[i];

        for (int bit = 0; bit < 16; ++bit)
        {
            framebuffer[charIndex++] = bitToByteTable[(value >> bit) & 1];
        }
    }
}