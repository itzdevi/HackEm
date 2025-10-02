@SCREEN
D=A
@POS
M=D

@24575
D=A
@END
M=D

(CLEAR)
    @POS
    A=M
    M=-1
    @POS
    MD=M+1
    @END
    D=M-D
    @CLEAR
    D;JGE

(LOOP)
    @LOOP
    0;JMP