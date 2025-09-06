#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "emulator.h"

#include "util/file.h"

int main(int argc, char** argv) {
    if (argc < 2) return 1;

    std::vector<short> content = read_bin(argv[1]);
    // Hack* hack = HackInit();
    // for (size_t i = 0; i < content.size(); i++) {
    //     HackAddCommand(hack, &(content[i]));
    // }
    // HackExecute(hack);
    
    // HackFree(hack);

    return 0;
}