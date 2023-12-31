#include <unordered_map>
#include <fstream>
#include <iostream>
#include <bitset>
#include <string>
#include "assembler.h"

std::vector<std::string> Parse(std::string filename) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        exit(1);
    }

    std::string content;
    std::string line;
    while (std::getline(f, line)) {
        content += line + '\n';
    }

    f.close();

    std::vector<std::string> tokens;
    std::string token;
    bool isComment = false;
    for (int i = 0; i < content.length(); i++) {
        if (content[i] == ' ' || content[i] == '\t') {
            if (token != "")
                tokens.push_back(token);
            token = "";
            continue;
        }
        if (content.length()-1 >= i)
            if (content[i] == '/' && content[i+1] == '/') {
                if (token != "")
                    tokens.push_back(token);
                token = "";
                isComment = true;
                continue;
            }
        if (content[i] == '\n') {
            if (token != "")
                tokens.push_back(token);
            token = "";
            isComment = false;
            continue;
        }

        if (isComment) continue;
        token += content[i];
    }

    return tokens;
}

std::vector<std::string> Symbolize(std::vector<std::string> source) {
    std::unordered_map<std::string, unsigned short> SYMBOLS;
    {
        SYMBOLS["R0"] = 0;
        SYMBOLS["R1"] = 1;
        SYMBOLS["R2"] = 2;
        SYMBOLS["R3"] = 3;
        SYMBOLS["R4"] = 4;
        SYMBOLS["R5"] = 5;
        SYMBOLS["R6"] = 6;
        SYMBOLS["R7"] = 7;
        SYMBOLS["R8"] = 8;
        SYMBOLS["R9"] = 9;
        SYMBOLS["R10"] = 10;
        SYMBOLS["R11"] = 11;
        SYMBOLS["R12"] = 12;
        SYMBOLS["R13"] = 13;
        SYMBOLS["R14"] = 14;
        SYMBOLS["R15"] = 15;
        SYMBOLS["SCREEN"] = 16384;
        SYMBOLS["KBD"] = 24576;
    }

    // first pass
    std::vector<std::string> labeled;
    int ln = 0;
    for (std::string line : source) {
        if (line[0] != '(') {
            if (line[line.length()-1] == '(' || line[line.length()-1] == ')') {
                std::cerr << "Error at line " << ln + 1 << ":\n";
                std::cerr << "Expected EOL, found '" << line[line.length()-1] << "'\n";
                exit(1);
            }

            labeled.push_back(line);
            ln++;
        }
        else {
            if (line[line.length()-1] != ')') {
                std::cerr << "Error at line " << ln + 1 << ":\n";
                std::cerr << "Expected ')', found EOL\n";
                exit(1);
            }
            std::string label = line.substr(1, line.length()-2);
            if (SYMBOLS.find(label) != SYMBOLS.end()) continue;
            SYMBOLS[label] = ln;
        }
    }

    // second pass
    int lastVarPos = 16;
    std::vector<std::string> parsed;
    for (std::string line : labeled) {
        if (line[0] != '@') {
            parsed.push_back(line);
            continue;
        }

        std::string tag = line.substr(1, line.length() - 1);
        if (SYMBOLS.find(tag) == SYMBOLS.end()) {
            SYMBOLS[tag] = lastVarPos++;
            parsed.push_back("@" + std::to_string(SYMBOLS[tag]));
        }
        else {
            parsed.push_back("@" + std::to_string(SYMBOLS[tag]));
        }
    }

    return parsed;
}

void Assemble(std::vector<std::string> symbolized, std::string output) {
    std::vector<std::string> bin;
    int ln = 0;
    for (std::string line : symbolized) {
        if (line[0] == '@') {
            // A INSTRUCTION
            unsigned short val = (unsigned short)std::stoi(line.substr(1, line.length() - 1));
            bin.push_back(std::bitset<16>(val).to_string());
        }
        else {
            // C INSTRUCTION
            std::string token;
            std::string dest;
            std::string comp;
            std::string jump;
            for (int i = 0; i <= line.length(); i++) {
                if (i == line.length()) {
                    if (comp == "") {
                        comp = token;
                        break;
                    }
                    else {
                        jump = token;
                        break;
                    }
                }
                if (line[i] == '=') {
                    dest = token;
                    token = "";
                    continue;
                }
                if (line[i] == ';') {
                    comp = token;
                    token = "";
                    continue;
                }
                token += line[i];
            }

            std::string res = "111";
            if (comp == "0")
                res += "0101010";
            else if (comp == "1")
                res += "0111111";
            else if (comp == "-1")
                res += "0111010";
            else if (comp == "D")
                res += "0001100";
            else if (comp == "A")
                res += "0110000";
            else if (comp == "M")
                res += "1110000";
            else if (comp == "!D")
                res += "0001101";
            else if (comp == "!A")
                res += "0110001";
            else if (comp == "!M")
                res += "1110001";
            else if (comp == "-D")
                res += "0001111";
            else if (comp == "-A")
                res += "0110011";
            else if (comp == "-M")
                res += "1110011";
            else if (comp == "D+1")
                res += "0011111";
            else if (comp == "A+1")
                res += "0110111";
            else if (comp == "M+1")
                res += "1110111";
            else if (comp == "D-1")
                res += "0001110";
            else if (comp == "A-1")
                res += "0110010";
            else if (comp == "M-1")
                res += "1110010";
            else if (comp == "D+A")
                res += "0000010";
            else if (comp == "D+M")
                res += "1000010";
            else if (comp == "D-A")
                res += "0010011";
            else if (comp == "D-M")
                res += "1010011";
            else if (comp == "A-D")
                res += "0000111";
            else if (comp == "M-D")
                res += "1000111";
            else if (comp == "D&A")
                res += "0000000";
            else if (comp == "D&M")
                res += "1000000";
            else if (comp == "D|A")
                res += "0010101";
            else if (comp == "D|M")
                res += "1010101";
            else {
                std::cerr << "Unrecognized symbol at line: " << ln + 1 << " \n";
                std::cerr << "Illegal value '" << comp << "', expected expression\n";
                exit(1);
            }

            if (dest == "")
                res += "000";
            else if (dest == "M")
                res += "001";
            else if (dest == "D")
                res += "010";
            else if (dest == "MD")
                res += "011";
            else if (dest == "A")
                res += "100";
            else if (dest == "AM")
                res += "101";
            else if (dest == "AD")
                res += "110";
            else if (dest == "AMD")
                res += "111";
            else {
                std::cerr << "Unrecognized symbol at line: " << ln + 1 << " \n";
                std::cerr << "Illegal value '" << dest << "', expected destination\n";
                exit(1);
            }

            if (jump == "")
                res += "000";
            else if (jump == "JGT")
                res += "001";
            else if (jump == "JEQ")
                res += "010";
            else if (jump == "JGE")
                res += "011";
            else if (jump == "JLT")
                res += "100";
            else if (jump == "JNE")
                res += "101";
            else if (jump == "JLE")
                res += "110";
            else if (jump == "JMP")
                res += "111";
            else {
                std::cerr << "Unrecognized symbol at line: " << ln + 1 << " \n";
                std::cerr << "Illegal value '" << jump << "', expected jump instruction\n";
                exit(1);
            }

            bin.push_back(res);
        }
        ln++;
    }

    unsigned short res[bin.size()] = { 0 };
    for (int i = 0; i < bin.size(); i++) {
        unsigned short x = std::stoi(bin[i], 0, 2);
        res[i] = x;
    }

    std::string s(output);
    std::fstream f;
    f.open(s, std::ios::out | std::ios::binary);
    f.write(reinterpret_cast<char*>(res), sizeof(res));
    f.close();
}
