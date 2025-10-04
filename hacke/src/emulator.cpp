#include "emulator.h"

#include <print>
#include <chrono>

#define CHECK_BIT(var, pos) var & 1 << pos

unsigned int lastCommandIndex = 0;
unsigned int cycles = 0;

// void HandleInput(GLFWwindow* window, unsigned int key) {
//     RAM[24576] = (short)key;
// }

Emulator::Emulator(std::vector<short> instructions)
{
    if (instructions.size() > ROM_SIZE)
    {
        std::println("Buffer overflow: ROM size is too big");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < instructions.size(); i++)
    {
        ROM[i] = instructions[i];
    }
    
    // Get pointer to GPU-mapped video memory for direct writes
    videoMemory = renderer.GetVideoMemoryPointer();
}

void Emulator::Begin()
{
    using namespace std::chrono;
    
    // 60 FPS = 16.67ms per frame
    const auto frameDuration = duration_cast<nanoseconds>(duration<double>(1.0 / 60.0));
    auto lastFrameTime = steady_clock::now();
    
    while (PC < ROM_SIZE && !renderer.ShouldClose())
    {
        renderer.Poll();

        // Run CPU instructions at full speed
        RunInstruction();
        PC++;

        // Only render at 60Hz
        auto currentTime = steady_clock::now();
        if (currentTime - lastFrameTime >= frameDuration)
        {
            // No SetFramebuffer call needed! Video memory writes go directly to GPU
            renderer.Render();
            lastFrameTime = currentTime;
        }
    }
}

void Emulator::RunInstruction()
{
    short command = ROM[PC];
    if (CHECK_BIT(command, 15))
    {
        // C INSTRUCTION
        unsigned short a = (command >> 12) & 1;
        unsigned short comp = (command >> 6) & 0b111111;
        unsigned short dest = (command >> 3) & 0b111;
        unsigned short jump = command & 0b111;

        short cycleValue = 0;
        switch (comp)
        {
        case 0b101010:
            // set register to 0
            cycleValue = 0;
            break;
        case 0b111111:
            // set register to 1
            cycleValue = 1;
            break;
        case 0b111010:
            // set register to -1
            cycleValue = -1;
            break;
        case 0b001100:
            // set register to D
            cycleValue = D;
            break;
        case 0b110000:
            // set register to A || M
            cycleValue = a ? ReadMemory(A) : A;
            break;
        case 0b001101:
            // set register to !D
            cycleValue = ~D;
            break;
        case 0b110001:
            // set register to !A || !M
            cycleValue = a ? ~ReadMemory(A) : ~A;
            break;
        case 0b001111:
            // set register to -D
            cycleValue = -D;
            break;
        case 0b110011:
            // set register to -A || -M
            cycleValue = a ? -ReadMemory(A) : -A;
            break;
        case 0b011111:
            // set register to D+1
            cycleValue = D + 1;
            break;
        case 0b110111:
            // set register to A+1 || M+1
            cycleValue = a ? ReadMemory(A) + 1 : A + 1;
            break;
        case 0b001110:
            // set register to D-1
            cycleValue = D - 1;
            break;
        case 0b110010:
            // set register to A-1 || M-1
            cycleValue = a ? ReadMemory(A) - 1 : A - 1;
            break;
        case 0b000010:
            // set register to D+A || D+M
            cycleValue = a ? D + ReadMemory(A) : D + A;
            break;
        case 0b010011:
            // set register to D-A || D-M
            cycleValue = a ? D - ReadMemory(A) : D - A;
            break;
        case 0b000111:
            // set register to A-D || M-D
            cycleValue = a ? ReadMemory(A) - D : A - D;
            break;
        case 0b000000:
            // set register to A&D || A&M
            cycleValue = a ? ReadMemory(A) & D : A & D;
            break;
        case 0b010101:
            // set register to A|D || A|M
            cycleValue = a ? ReadMemory(A) | D : A | D;
            break;
        }

        // Handle register assignments
        if (dest & 0b100)  // A register
            A = cycleValue;
        if (dest & 0b010)  // D register  
            D = cycleValue;
        if (dest & 0b001)  // Memory at address A
            WriteMemory(A, cycleValue);  // Use WriteMemory for proper video memory handling

        switch (jump)
        {
        case 0b001:
            // JGT
            if (cycleValue > 0)
                PC = A - 1;
            break;
        case 0b010:
            // JEQ
            if (cycleValue == 0)
                PC = A - 1;
            break;
        case 0b011:
            // JGE
            if (cycleValue >= 0)
                PC = A - 1;
            break;
        case 0b100:
            // JLT
            if (cycleValue < 0)
                PC = A - 1;
            break;
        case 0b101:
            // JNE
            if (cycleValue != 0)
                PC = A - 1;
            break;
        case 0b110:
            // JLE
            if (cycleValue <= 0)
                PC = A - 1;
            break;
        case 0b111:
            // JMP
            PC = A - 1;
            break;
        }
    }
    else
    {
        // A INSTRUCTION
        A = command;
    }
}

short Emulator::ReadMemory(unsigned short address)
{
    if (address >= SCREEN_OFFSET && address < SCREEN_OFFSET + VIDEO_BUFFER_SIZE)
    {
        // Read from GPU-mapped video memory
        return videoMemory[address - SCREEN_OFFSET];
    }
    return RAM[address];
}

void Emulator::WriteMemory(unsigned short address, short value)
{
    if (address >= SCREEN_OFFSET && address < SCREEN_OFFSET + VIDEO_BUFFER_SIZE)
    {
        // Write directly to GPU-mapped video memory - instant GPU update!
        videoMemory[address - SCREEN_OFFSET] = value;
    }
    else
    {
        // Write to regular RAM
        RAM[address] = value;
    }
}