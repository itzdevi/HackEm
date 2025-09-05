#pragma once

typedef struct Hack {
    short A;
    short D;
    unsigned short PC;
    short** ROM;
    short* RAM;
} Hack;

Hack* HackInit();
void HackFree(Hack* instance);
void HackAddCommand(Hack* instance, short* command);
void HackExecute(Hack* instance);
short HackGetMemData(Hack* instance, unsigned short address);