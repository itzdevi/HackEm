# HackEk Overview
The HackEm project is based on the computer from the course nand2tetris by Noam Nisan and Shimon Schocken. It includes both an assembler and an emulator, and at a later point a compiler for either C or a custom designed language for the HACK architecture.

# HACK Computer Infrastracture
The HACK computer is a 16 bit computer with 256KB of instruction ROM, and 192KB of RAM.
The computer has 2 registers, A and D.
The A register is mainly used for addresses, but you can set it to any number, which makes it useful for quick math instructions.
The D register is a general purpose register, used for storing quick access data.

# HACK Assembly
In the hack assembly language there are 2 types of instructions.
## A Instruction
The A instruction is used for changing the A register.
Its syntax is as follows:
```
@<address>
```
The address can be any number, label, or variable, which will be explained at a later point.

## C Instruction
The C instruction is a more compilcated instruction used for logic.
Its syntax is as follows:
```
<destination>=<computation>;<jump>
```
The destination field can include any of the following combinations:  
`A`  
`D`  
`M`  
`AD`  
`AM`  
`MD`  
`AMD`  
