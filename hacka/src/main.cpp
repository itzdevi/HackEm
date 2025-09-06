#include "assembler.h"

bool hasEnding(std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length())
    {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else
    {
        return false;
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;
    std::string s(argv[1]);
    if (!hasEnding(s, ".asm"))
        return 1;
    std::string tag = s.substr(0, s.length() - 4);
    Assemble(Symbolize(Parse(s)), tag + ".hack");
}