#pragma once

#include <stdlib.h>
#include <SDL2/SDL.h>

class Hack {
private:
    short A = 0;
    short D = 0;
    unsigned short PC = 0;
    short* ROM[32768] = { 0 };
    short RAM[24577] = { 0 };
    SDL_Window* window;
    SDL_Surface* surface;
    void ParseCommand(short command);
    void HandleScreen();
    void HandleInput(SDL_Event e);
public:
    Hack();
    void AddCommand(short* command);
    void Execute();
    short GetMemData(short address);
};