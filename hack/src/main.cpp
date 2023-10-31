#include <iostream>
#include "Hack.h"
#include <fstream>
#include <string>
#include <vector>

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

int main(int argc, char** argv) {
#if defined(unix) || defined(__unix__) || defined(__unix)
#include <stdlib.h>
setenv("SDL_VIDEO_X11_VISUALID", "", 1);
#endif
    if (argc < 2) return 1;
    if (!hasEnding(argv[1], ".hack")) return 1;
    std::ifstream f(argv[1]);
    std::vector<std::string> content;
    std::string line;
    while (std::getline(f, line)) {
        content.push_back(line);
    }
    Hack hack;
    for (std::string line : content) {
        hack.AddCommand(new short(std::stoi(line, 0, 2)));
    }
    hack.Execute();

    return 0;
}