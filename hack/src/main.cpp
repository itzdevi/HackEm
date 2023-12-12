#include <iostream>
#include <fstream>
#include <string>
#include <vector>

extern "C" {
    #include "hack.h"
    Hack* HackInit();
    void HackFree(Hack* instance);
    void HackAddCommand(Hack* instance, short* command);
    void HackExecute(Hack* instance);
    void* HackGetMemData(Hack* instance, unsigned short address);
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    if (!hasEnding(argv[1], ".hack")) return 1;
    std::ifstream f(argv[1]);
    std::vector<std::string> content;
    std::string line;
    while (std::getline(f, line)) {
        content.push_back(line);
    }
    Hack* hack = HackInit();
    for (std::string line : content) {
        HackAddCommand(hack, new short(std::stoi(line, 0, 2)));
    }
    HackExecute(hack);

    HackFree(hack);

    return 0;
}