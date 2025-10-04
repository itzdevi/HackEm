@65
D=A
@R0
M=D

@CURSOR_X
M=0

@PROGRAM
D=A
@R15
M=D

@DRAW_CHARACTER
0;JMP

(PROGRAM)


@END
(END)
    0;JMP


(MUL_16)
    // RESET PARAMETERS
    @15
    D=A
    @MUL_16_I
    M=D
    @R10
    D=M
    @MUL_16_ORIGINAL
    M=D

    (LOOP_MUL_16)
        // ADD
        @MUL_16_ORIGINAL
        D=M
        @R10
        M=D+M

        // REMOVE 1 FROM I
        @MUL_16_I
        MD=M-1

        @LOOP_MUL_16
        D;JGT

    // RETURN
    @RETURN_CHARACTER
    0;JMP

(DRAW_CHARACTER)
    // CALCULATE CHARACTER POINTER
    @R0
    D=M
    @32
    D=D-A
    @R10
    M=D

    @MUL_16
    0;JMP
    (RETURN_CHARACTER)

    @R10
    D=M
    @129
    D=D+A
    @CHARACTER_PTR
    M=D

    @SCREEN
    D=A
    @SCREEN_OFFSET
    M=D

    @8
    D=A
    @DRAW_CHARACTER_I
    M=D
    (LOOP_DRAW_CHARACTER)
        @CHARACTER_PTR
        A=M
        D=M

        @SCREEN_OFFSET
        A=M
        M=D

        @2
        D=A
        @CHARACTER_PTR
        M=D+M

        @32
        D=A
        @SCREEN_OFFSET
        M=D+M

        // REMOVE 1 FROM I
        @DRAW_CHARACTER_I
        MD=M-1

        @LOOP_DRAW_CHARACTER
        D;JGT

    @R15
    A=M
    0;JMP