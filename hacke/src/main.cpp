#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "emulator.h"

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
    std::ifstream f(argv[1], std::ios::binary);
    std::vector<short> content;
    short word;
    while (f.read(reinterpret_cast<char*>(&word), sizeof(word))) {
        content.push_back(word);
    }
    Hack* hack = HackInit();
    for (size_t i = 0; i < content.size(); i++) {
        HackAddCommand(hack, &(content[i]));
    }
    HackExecute(hack);

    std::cout << HackGetMemData(hack, 16) << "\n";
    
    HackFree(hack);

    return 0;
}