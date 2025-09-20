@SCREEN
D=A
@POSITION
M=D

@24576
D=A
@END
M=D

(CLEAR)
    @POSITION
    A=M
    M=-1
    @POSITION
    MD=M+1
    @END
    D=M-D
    @CLEAR
    D;JGE


(END)
    @END
    0;JMP