#include <iostream>
#include <stdlib.h>
#include "emulator.h"

#define CLOCK_RATE 2000
#define FRAME_RATE 60
#define CHECK_BIT(var, pos) var & 1 << pos

unsigned int lastCommandIndex = 0;
unsigned int cycles = 0;

// void HandleInput(GLFWwindow* window, unsigned int key) {
//     RAM[24576] = (short)key;
// }

Hack *HackInit()
{
    Hack *hack = (Hack *)malloc(sizeof(Hack));
    hack->A = 0;
    hack->D = 0;
    hack->PC = 0;
    hack->ROM = (short **)calloc(32768, sizeof(short *));
    hack->RAM = (short *)calloc(24577, sizeof(short));

    return hack;
}

void HackFree(Hack *instance)
{
    free(instance->ROM);
    free(instance);
}

void ParseCommand(Hack *instance, short command)
{
    if (CHECK_BIT(command, 15))
    {
        // C INSTRUCTION
        short a = (unsigned short)((unsigned short)command << 3) >> 15;
        short comp = (unsigned short)((unsigned short)command << 4) >> 10;
        short dest = (unsigned short)((unsigned short)command << 10) >> 13;
        short jump = (unsigned short)((unsigned short)command << 13) >> 13;

        short **regs = (short **)calloc(3, sizeof(short *));
        switch (dest)
        {
        case 0b001:
            regs[0] = &(instance->RAM[instance->A]);
            break;
        case 0b010:
            regs[0] = &instance->D;
            break;
        case 0b011:
            regs[0] = &(instance->RAM[instance->A]);
            regs[1] = &instance->D;
            break;
        case 0b100:
            regs[0] = &instance->A;
            break;
        case 0b101:
            regs[0] = &(instance->RAM[instance->A]);
            regs[1] = &instance->A;
            break;
        case 0b110:
            regs[0] = &instance->D;
            regs[1] = &instance->A;
            break;
        case 0b111:
            regs[0] = &(instance->RAM[instance->A]);
            regs[1] = &instance->D;
            regs[2] = &instance->A;
            break;
        }

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
            cycleValue = instance->D;
            break;
        case 0b110000:
            // set register to A || M
            cycleValue = a ? instance->RAM[instance->A] : instance->A;
            break;
        case 0b001101:
            // set register to !D
            cycleValue = ~instance->D;
            break;
        case 0b110001:
            // set register to !A || !M
            cycleValue = a ? ~instance->RAM[instance->A] : ~instance->A;
            break;
        case 0b001111:
            // set register to -D
            cycleValue = -instance->D;
            break;
        case 0b110011:
            // set register to -A || -M
            cycleValue = a ? -instance->RAM[instance->A] : -instance->A;
            break;
        case 0b011111:
            // set register to D+1
            cycleValue = instance->D + 1;
            break;
        case 0b110111:
            // set register to A+1 || M+1
            cycleValue = a ? instance->RAM[instance->A] + 1 : instance->A + 1;
            break;
        case 0b001110:
            // set register to D-1
            cycleValue = instance->D - 1;
            break;
        case 0b110010:
            // set register to A-1 || M-1
            cycleValue = a ? instance->RAM[instance->A] - 1 : instance->A - 1;
            break;
        case 0b000010:
            // set register to D+A || D+M
            cycleValue = a ? instance->D + instance->RAM[instance->A] : instance->D + instance->A;
            break;
        case 0b010011:
            // set register to D-A || D-M
            cycleValue = a ? instance->D - instance->RAM[instance->A] : instance->D - instance->A;
            break;
        case 0b000111:
            // set register to A-D || M-D
            cycleValue = a ? instance->RAM[instance->A] + instance->D : instance->A + instance->D;
            break;
        case 0b000000:
            // set register to A&D || A&M
            cycleValue = a ? instance->RAM[instance->A] & instance->D : instance->A & instance->D;
            break;
        case 0b010101:
            // set register to A|D || A|M
            cycleValue = a ? instance->RAM[instance->A] | instance->D : instance->A | instance->D;
            break;
        }

        for (int i = 0; i < 3; i++)
        {
            if (regs[i] == NULL)
                break;
            *regs[i] = cycleValue;
        }

        switch (jump)
        {
        case 0b001:
            // JGT
            if (cycleValue > 0)
                instance->PC = instance->A;
            break;
        case 0b010:
            // JEQ
            if (cycleValue == 0)
                instance->PC = instance->A;
            break;
        case 0b011:
            // JGE
            if (cycleValue >= 0)
                instance->PC = instance->A;
            break;
        case 0b100:
            // JLT
            if (cycleValue < 0)
                instance->PC = instance->A;
            break;
        case 0b101:
            // JNE
            if (cycleValue != 0)
                instance->PC = instance->A;
            break;
        case 0b110:
            // JLE
            if (cycleValue <= 0)
                instance->PC = instance->A;
            break;
        case 0b111:
            // JMP
            instance->PC = instance->A;
            break;
        }

        free(regs);
    }
    else
    {
        // A INSTRUCTION
        instance->A = command;
    }
}

void HackAddCommand(Hack *instance, short *command)
{
    instance->ROM[lastCommandIndex++] = command;
}

void HackExecute(Hack *instance)
{
    const unsigned short ROMLength = 32768;
    while (instance->PC < ROMLength)
    {
        if (instance->ROM[instance->PC])
        {
            ParseCommand(instance, *(instance->ROM[instance->PC]));
        }

        // if (cycles >= CLOCK_RATE / FRAME_RATE)
        // {
        //     HandleScreen(instance);
        //     cycles = 0;
        // }

        cycles++;
        instance->PC++;
    }
}

short HackGetMemData(Hack *instance, unsigned short address)
{
    return instance->RAM[address];
}
