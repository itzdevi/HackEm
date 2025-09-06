#include "util/file.h"

#include <fstream>

std::vector<short> read_bin(std::string path)
{
    std::ifstream f(path, std::ios::binary);
    std::vector<short> content;
    short word;
    while (f.read(reinterpret_cast<char *>(&word), sizeof(word)))
    {
        content.push_back(word);
    }
    return content;
}