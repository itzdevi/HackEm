#include "renderer.h"

#include <stdlib.h>
#include <print>
#include <string>
#include <cstdint>
#include <cstring>

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
#version 430 core

#define VIDEO_BUFFER_SIZE 8192
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 256

out vec4 FragColor;
in vec2 texCoord;

layout(std430, binding = 0) buffer VideoMemory {
    uint videoData[];
};

void main() {
    // Flip vertically for proper orientation
    vec2 flippedCoord = vec2(texCoord.x, 1.0 - texCoord.y);
    
    // Convert texture coordinates to pixel coordinates
    ivec2 pixelCoord = ivec2(flippedCoord * vec2(SCREEN_WIDTH-1, SCREEN_HEIGHT-1));
    
    // Clamp pixel coordinates to valid range
    pixelCoord = clamp(pixelCoord, ivec2(0), ivec2(SCREEN_WIDTH-1, SCREEN_HEIGHT-1));
    
    // Calculate which 16-bit word and which bit within that word
    int pixelIndex = pixelCoord.y * SCREEN_WIDTH + pixelCoord.x;
    int wordIndex = pixelIndex / 32;
    int bitIndex = pixelIndex % 32;
    
    // Bounds check - show red for out of bounds to debug
    if (wordIndex >= VIDEO_BUFFER_SIZE) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Red for debugging
        return;
    }
    
    // Extract the bit from the video memory
    uint word = videoData[wordIndex];
    uint bit = (word >> bitIndex) & 1u;
    
    // Convert bit to color (0 = black, 1 = white)
    float color = float(bit);
    FragColor = vec4(color, color, color, 1.0);
}
)glsl";

const float quadVertices[] = {
    // positions   // tex coords
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 1.0f};

const unsigned int quadIndices[] = {0, 1, 2, 2, 3, 0};

// Helper function to check shader compilation errors
bool checkShaderCompilation(unsigned int shader, const std::string& shaderType) {
    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::println("ERROR: {} shader compilation failed:\n{}", shaderType.c_str(), infoLog);
        return false;
    }
    return true;
}

// Helper function to check program linking errors
bool checkProgramLinking(unsigned int program) {
    int success;
    char infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::println("ERROR: Shader program linking failed:\n{}", infoLog);
        return false;
    }
    return true;
}

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
    
    // Enable VSync for smooth 60Hz rendering
    glfwSwapInterval(1);

    // Create persistent mapped SSBO for zero-copy GPU access to video memory
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, VIDEO_BUFFER_SIZE * sizeof(unsigned short), nullptr, 
                   GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    
    // Map the buffer persistently - GPU can read directly from this memory
    mappedVideoMemory = (short*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 
                                                 VIDEO_BUFFER_SIZE * sizeof(unsigned short),
                                                 GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    
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
    if (!checkShaderCompilation(vertexShader, "Vertex")) {
        std::println("Failed to compile vertex shader");
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);
    if (!checkShaderCompilation(fragmentShader, "Fragment")) {
        std::println("Failed to compile fragment shader");
        glDeleteShader(vertexShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    if (!checkProgramLinking(program)) {
        std::println("Failed to link shader program");
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Renderer::~Renderer()
{
    // Unmap the persistent buffer
    if (mappedVideoMemory) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    
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
    
    // The SSBO is already bound to binding point 0, no need to bind texture
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}

bool Renderer::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void Renderer::SetFramebuffer(short *buffer)
{
    // Copy data to persistent mapped buffer - GPU reads directly from this memory!
    // This is much faster than glBufferSubData because it's just a memcpy
    memcpy(mappedVideoMemory, buffer, VIDEO_BUFFER_SIZE * sizeof(short));
    // No OpenGL calls needed - the GPU buffer is persistently mapped and coherent
}

short* Renderer::GetVideoMemoryPointer()
{
    // Return pointer to GPU-mapped memory for direct writes
    // The emulator can write directly to this memory without any OpenGL calls!
    return mappedVideoMemory;
}
