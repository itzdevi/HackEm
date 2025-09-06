// reset memory
@5
D=A
@R0
M=D
@3
D=A
@R1
M=D

(MUL)
    @R0
    D=M
    @RES
    M=D+M
    @R1
    MD=M-1
    @MUL
    D;JGT