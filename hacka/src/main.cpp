#include <print>

#include "cxxopts.hpp"
#include "assembler.h"

int main(int argc, char **argv)
{
    cxxopts::Options options("hacka");
    options.add_options()("i,input", "Input file", cxxopts::value<std::string>())("o,output", "Output file", cxxopts::value<std::string>());
    auto result = options.parse(argc, argv);
    if (!result.count("input"))
    {
        std::println("Error: input field is required");
        return EXIT_FAILURE;
    }
    if (!result.count("output"))
    {
        std::println("Error: output field is required");
        return EXIT_FAILURE;
    }
    std::string inPath = result["input"].as<std::string>();
    std::string outPath = result["output"].as<std::string>();
    Assemble(Symbolize(Parse(inPath)), outPath);
}