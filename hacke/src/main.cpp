#include <print>

#include "emulator.h"
#include "util/file.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::println("Missing ROM parameter.\nUsage: hacke <ROM>");
        return EXIT_FAILURE;
    }

    Emulator em(read_bin(argv[1]));
    em.Begin();

    std::println("{}", em.ReadMemory(16));

    return EXIT_SUCCESS;
}