#include "emulator.h"

#include <print>

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
        std::println("Buffer overflow: ROM size too big");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < instructions.size(); i++)
    {
        ROM[i] = instructions[i];
    }
}

void Emulator::Begin()
{
    while (PC < ROM_SIZE)
    {
        RunInstruction();
        PC++;
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
        short *regs[3] = {nullptr, nullptr, nullptr};
        if (dest & 0b100)
            regs[0] = &A;
        if (dest & 0b010)
            regs[1] = &D;
        if (dest & 0b001)
            regs[2] = &RAM[A];

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
            cycleValue = a ? RAM[A] : A;
            break;
        case 0b001101:
            // set register to !D
            cycleValue = ~D;
            break;
        case 0b110001:
            // set register to !A || !M
            cycleValue = a ? ~RAM[A] : ~A;
            break;
        case 0b001111:
            // set register to -D
            cycleValue = -D;
            break;
        case 0b110011:
            // set register to -A || -M
            cycleValue = a ? -RAM[A] : -A;
            break;
        case 0b011111:
            // set register to D+1
            cycleValue = D + 1;
            break;
        case 0b110111:
            // set register to A+1 || M+1
            cycleValue = a ? RAM[A] + 1 : A + 1;
            break;
        case 0b001110:
            // set register to D-1
            cycleValue = D - 1;
            break;
        case 0b110010:
            // set register to A-1 || M-1
            cycleValue = a ? RAM[A] - 1 : A - 1;
            break;
        case 0b000010:
            // set register to D+A || D+M
            cycleValue = a ? D + RAM[A] : D + A;
            break;
        case 0b010011:
            // set register to D-A || D-M
            cycleValue = a ? D - RAM[A] : D - A;
            break;
        case 0b000111:
            // set register to A-D || M-D
            cycleValue = a ? RAM[A] - D : A + D;
            break;
        case 0b000000:
            // set register to A&D || A&M
            cycleValue = a ? RAM[A] & D : A & D;
            break;
        case 0b010101:
            // set register to A|D || A|M
            cycleValue = a ? RAM[A] | D : A | D;
            break;
        }

        for (int i = 0; i < 3; i++)
        {
            if (regs[i])
                *regs[i] = cycleValue;
        }

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
    return RAM[address];
}