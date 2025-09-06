#pragma once

#include <vector>

#define ROM_SIZE 32768
#define RAM_SIZE 24577

class Emulator
{
private:
    short A = 0;
    short D = 0;
    unsigned short PC = 0;
    short ROM[ROM_SIZE] = {};
    short RAM[RAM_SIZE] = {};

    void RunInstruction();

public:
    Emulator(std::vector<short> instructions);
    void Begin();
    short ReadMemory(unsigned short address);
};

typedef struct Hack
{
    short A;
    short D;
    unsigned short PC;
    short **ROM;
    short *RAM;
} Hack;

Hack *HackInit();
void HackFree(Hack *instance);
void HackAddCommand(Hack *instance, short *command);
void HackExecute(Hack *instance);
short HackGetMemData(Hack *instance, unsigned short address);