#include <unordered_map>
#include <fstream>
#include <iostream>
#include <bitset>
#include "Assembler.h"

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

    // first run
    std::vector<std::string> tagged;
    int ln = 0;
    for (std::string line : source) {
        if (!(line[0] == '(' && line[line.length()-1] == ')')) {
            tagged.push_back(line);
            ln++;
            continue;
        }

        std::string tag = line.substr(1, line.length()-2);
        if (SYMBOLS.find(tag) != SYMBOLS.end()) continue;
        SYMBOLS[tag] = ln;
        continue;

    }

    // second run
    int lastVarPos = 16;
    std::vector<std::string> parsed;
    for (std::string line : tagged) {
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
            if (comp == "1")
                res += "0111111";
            if (comp == "-1")
                res += "0111010";
            if (comp == "D")
                res += "0001100";
            if (comp == "A")
                res += "0110000";
            if (comp == "M")
                res += "1110000";
            if (comp == "!D")
                res += "0001101";
            if (comp == "!A")
                res += "0110001";
            if (comp == "!M")
                res += "1110001";
            if (comp == "-D")
                res += "0001111";
            if (comp == "-A")
                res += "0110011";
            if (comp == "-M")
                res += "1110011";
            if (comp == "D+1")
                res += "0011111";
            if (comp == "A+1")
                res += "0110111";
            if (comp == "M+1")
                res += "1110111";
            if (comp == "D-1")
                res += "0001110";
            if (comp == "A-1")
                res += "0110010";
            if (comp == "M-1")
                res += "1110010";
            if (comp == "D+A")
                res += "0000010";
            if (comp == "D+M")
                res += "1000010";
            if (comp == "D-A")
                res += "0010011";
            if (comp == "D-M")
                res += "1010011";
            if (comp == "A-D")
                res += "0000111";
            if (comp == "M-D")
                res += "1000111";
            if (comp == "D&A")
                res += "0000000";
            if (comp == "D&M")
                res += "1000000";
            if (comp == "D|A")
                res += "0010101";
            if (comp == "D|M")
                res += "1010101";
            
            if (dest == "")
                res += "000";
            if (dest == "M")
                res += "001";
            if (dest == "D")
                res += "010";
            if (dest == "MD")
                res += "011";
            if (dest == "A")
                res += "100";
            if (dest == "AM")
                res += "101";
            if (dest == "AD")
                res += "110";
            if (dest == "AMD")
                res += "111";

            if (jump == "")
                res += "000";
            if (jump == "JGT")
                res += "001";
            if (jump == "JEQ")
                res += "010";
            if (jump == "JGE")
                res += "011";
            if (jump == "JLT")
                res += "100";
            if (jump == "JNE")
                res += "101";
            if (jump == "JLE")
                res += "110";
            if (jump == "JMP")
                res += "111";

            bin.push_back(res);
        }
    }

    std::string s(output);
    std::ofstream f(s);
    for (std::string line : bin) {
        f << line << "\n";
    }
    f.close();
}
