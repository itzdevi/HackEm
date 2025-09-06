#include <print>

#include "cxxopts.hpp"
#include "emulator.h"
#include "util/file.h"

int main(int argc, char **argv)
{
    cxxopts::Options options("hacka");
    options.add_options()("i,input", "Input file", cxxopts::value<std::string>());
    auto result = options.parse(argc, argv);
    if (!result.count("input"))
    {
        std::println("Error: input field is required");
        return EXIT_FAILURE;
    }

    std::string inPath = result["input"].as<std::string>();
    std::vector instructions = read_bin(inPath);
    Emulator em(instructions);
    em.Begin();

    std::println("{}", em.ReadMemory(16));

    return EXIT_SUCCESS;
}