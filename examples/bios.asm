// SETTING UP FONT POINTER TO R0
@128
D=A
@R0
M=D

// ROW 0
@960
D=A
@R0
A=M
M=D
@R0
M=M+1

// ROW 1
@1632
D=A
@R0
A=M
M=D
@R0
M=M+1

// ROW 2
@3120
D=A
@R0
A=M
M=D
@R0
M=M+1

// ROW 3
@3120
D=A
@R0
A=M
M=D
@R0
M=M+1

// ROW 4
@4080
D=A
@R0
A=M
M=D
@R0
M=M+1

// ROW 5
@3120
D=A
@R0
A=M
M=D
@R0
M=M+1

// ROW 6
@3120
D=A
@R0
A=M
M=D
@R0
M=M+1

// ROW 6
@3120
D=A
@R0
A=M
M=D
@R0
M=M+1

@CURSOR_X
M=0
@CURSOR_Y
M=0

@100
D=A
@COUNT
M=D
(WRITE_LOOP)
    @WRITE_RETURN
    D=A
    @R15
    M=D
    @DRAW_LETTER
    0;JMP
    (WRITE_RETURN)

    @CURSOR_X
    MD=M+1
    // WRAP TEXT
    @32
    D=A-D
    @NO_WRAP
    D;JGE

    @CURSOR_X
    M=0
    @320
    D=A
    @CURSOR_Y
    M=D+M

    (NO_WRAP)

    @COUNT
    MD=M-1
    @WRITE_LOOP
    D;JGT

    @END
    0;JMP


(DRAW_LETTER)
    @ROW
    M=0
    @SCREEN
    D=A
    @CURSOR_X
    D=D+M
    @CURSOR_Y
    D=D+M
    @SCREEN_OFFSET
    M=D

    (RENDER)
        // SELECT ROW
        @ROW
        D=M
        @128
        A=D+A
        D=M
        
        // DRAW ROW
        @SCREEN_OFFSET
        A=M
        M=D

        // UPDATE OFFSET
        @32
        D=A
        @SCREEN_OFFSET
        M=D+M

        // LOOP
        @ROW
        MD=M+1
        @8
        D=D-A
        @RENDER
        D;JLT

        @R15
        A=M
        0;JMP

@END
(END)
    0;JMP