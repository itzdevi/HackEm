#include <Hack.h>
#include <iostream>
#include <bitset>

#define CLOCK_RATE 20000
#define FRAME_RATE 60
#define CHECK_BIT(var, pos) var & 1 << pos

unsigned int lastCommandIndex = 0;
unsigned int cycles = 0;

Hack::Hack() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);
    
    window = SDL_CreateWindow(
        "Hack",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        512, 256,
        SDL_WINDOW_SHOWN
    );
    surface = SDL_GetWindowSurface(window);
}

void Hack::ParseCommand(short command) {
    if (CHECK_BIT(command, 16)) {
        // C INSTRUCTION
        short a = (unsigned short)((unsigned short)command << 3) >> 15;
        short comp = (unsigned short)((unsigned short)command << 4) >> 10;
        short dest = (unsigned short)((unsigned short)command << 10) >> 13;
        short jump = (unsigned short)((unsigned short)command << 13) >> 13;

        short** regs = new short*[3] { 0 };
        switch (dest) {
            case 0b001:
                regs[0] = &(RAM[A]);
                break;
            case 0b010:
                regs[0] = &D;
                break;
            case 0b011:
                regs[0] = &(RAM[A]);
                regs[1] = &D;
                break;
            case 0b100:
                regs[0] = &A;
                break;
            case 0b101:
                regs[0] = &(RAM[A]);
                regs[1] = &A;
                break;
            case 0b110:
                regs[0] = &D;
                regs[1] = &A;
                break;
            case 0b111:
                regs[0] = &(RAM[A]);
                regs[1] = &D;
                regs[2] = &A;
                break;
        }

        short cycleValue = 0;
        switch (comp) {
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
            cycleValue = D+1;
            break;
        case 0b110111:
            // set register to A+1 || M+1
            cycleValue = a ? RAM[A]+1 : A+1;
            break;
        case 0b001110:
            // set register to D-1
            cycleValue = D-1;
            break;
        case 0b110010:
            // set register to A-1 || M-1
            cycleValue = a ? RAM[A]-1 : A-1;
            break;
        case 0b000010:
            // set register to D+A || D+M
            cycleValue = a ? D+RAM[A] : D+A;
            break;
        case 0b010011:
            // set register to D-A || D-M
            cycleValue = a ? D-RAM[A] : D-A;
            break;
        case 0b000111:
            // set register to A-D || M-D
            cycleValue = a ? RAM[A]+D : A+D;
            break;
        case 0b000000:
            // set register to A&D || A&M
            cycleValue = a ? RAM[A]&D : A&D;
            break;
        case 0b010101:
            // set register to A|D || A|M
            cycleValue = a ? RAM[A]|D : A|D;
            break;
        }

        for (int i = 0; i < 3; i++) {
            if (regs[i] == nullptr) break;
            *regs[i] = cycleValue;
        }

        switch (jump) {
            case 0b001:
                // JGT
                if (cycleValue > 0) PC = A;
                break;
            case 0b010:
                // JEQ
                if (cycleValue == 0) PC = A;
                break;
            case 0b011:
                // JGE
                if (cycleValue >= 0) PC = A;
                break;
            case 0b100:
                // JLT
                if (cycleValue < 0) PC = A;
                break;
            case 0b101:
                // JNE
                if (cycleValue != 0) PC = A;
                break;
            case 0b110:
                // JLE
                if (cycleValue <= 0) PC = A;
                break;
            case 0b111:
                // JMP
                PC = A;
                break;
        }

        delete[] regs;
    }
    else {
        // A INSTRUCTION
        A = command;
    }
}

void Hack::HandleScreen() {
    SDL_LockSurface(surface);
    uint8_t* pixels = (uint8_t*) surface->pixels;
    int x = 0;
    int y = 0;
    int pitch = surface->pitch;
    uint8_t bytesPerPixel = surface->format->BytesPerPixel;
    for (int i = 16384; i < 24576; i++) {
        short data = RAM[i];
        for (int j = 0; j < 16; j++) {
            if (x == 512) {
                x = 0;
                y++;
            }

            int bit = data >> (15 - j);
            pixels[y * pitch + x * bytesPerPixel + 0] = bit;
            pixels[y * pitch + x * bytesPerPixel + 1] = bit;
            pixels[y * pitch + x * bytesPerPixel + 2] = bit;

            x++;
        }
    }
    SDL_UnlockSurface(surface);
}

void Hack::HandleInput(SDL_Event e) {
    if (e.type != SDL_TEXTINPUT) {
        RAM[24576] = 0;
        return;
    }
    RAM[24576] = (short)e.text.text[0];
}

void Hack::AddCommand(short* command) {
    ROM[++lastCommandIndex] = command;
}

void Hack::Execute() {
    unsigned short ROMLength = sizeof(ROM) / sizeof(ROM[0]);
    while (PC < ROMLength) {
        if (ROM[PC]) {
                ParseCommand(*(ROM[PC]));
        }
        SDL_Event e;
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT)
            break;

        HandleInput(e);

        if (cycles >= CLOCK_RATE/FRAME_RATE) {
            // SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
            HandleScreen();
            cycles = 0;
        }
        SDL_UpdateWindowSurface(window);

        cycles++;
        PC++;
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}

short Hack::GetMemData(short address) {
    return RAM[address];
}
