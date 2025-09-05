#pragma once

#include <vector>
#include <string>

std::vector<std::string> Parse(std::string filename);
std::vector<std::string> Symbolize(std::vector<std::string> source);
void Assemble(std::vector<std::string> symbolized, std::string output);