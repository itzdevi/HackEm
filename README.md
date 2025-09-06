# Hack Emulator

A simple Hack CPU and assembler emulator.

## Overview

This project emulates the Hack CPU from _nand2tetris_ by **Shimon Schocken** and **Noam Nisan**, supporting Hack assembly programs and C-instructions.

## Getting Started

1. Build the project (see [Compilation Guide](docs/COMPILATION.md)) / install precompiled binaries
2. Assemble a program
    ```bash
    hacka -i examples/mul.asm -o mul.bin
    ```
3. Run the emulator with compiled binary
    ```bash
    hacke -i mul.bin
    ```
