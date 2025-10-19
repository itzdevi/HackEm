@129
D=A
@FONT_OFFSET
M=D

@4096
D=A
@TEXT_OFFSET
M=D

@2048
D=A
@TEXT_SIZE
M=D

@WORD_IDX
M=0
@CHAR_COUNT
M=0
@Y_POS
M=0

@TEXT_OFFSET
D=M
@PTR
M=D

@2048
D=A
@IDX
M=D


(WRITE)
    @SCREEN
    D=A
    @CLEAR_PTR
    M=D
    (CLEAR_LOOP)
        @CLEAR_PTR
        A=M
        M=0
        @CLEAR_PTR
        MD=M+1
        @24576
        D=A-D
        @CLEAR_LOOP
        D;JGE

    @KBD
    D=M
    @TEXT_OFFSET
    A=M
    M=D
    //M=D

    @SCREEN_OFFSET
    M=0
    @WORD_IDX
    M=0
    @CHAR_COUNT
    M=0
    @TEXT_IDX
    M=0
    @Y_POS
    M=0
    (LOOP_WRITE)
        // GET CURRENT CHARACTER
        @TEXT_IDX
        D=M
        @TEXT_OFFSET
        D=D+M

        // SET PARAMETERS
        A=D
        D=M
        @R0
        M=D
        @LOOP_WRITE_CONTINUE
        D=A
        @R15
        M=D

        // CALL DRAW
        @DRAW_CHARACTER
        0;JMP

        (LOOP_WRITE_CONTINUE)

        // INCREASE INDEX AND LOOP
        @TEXT_IDX
        MD=M+1
        @TEXT_SIZE
        D=D-M
        @LOOP_WRITE
        D;JLT

    @WRITE
    0;JMP
    

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
    @SKIP_DRAW_CHARACTER
    D;JEQ
    @32
    D=D-A
    @R10
    M=D

    @MUL_16
    0;JMP
    (RETURN_CHARACTER)

    @R10
    D=M
    @FONT_OFFSET
    D=D+M
    @CHAR_COUNT
    D=D-M
    @CHARACTER_PTR
    M=D

    @SCREEN
    D=A
    @WORD_IDX
    D=D+M
    @Y_POS
    D=D+M
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
        M=D|M

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

    (SKIP_DRAW_CHARACTER)

    @CHAR_COUNT
    MD=M+1
    @2
    D=A-D
    @SKIP_INC
    D;JGT
    @WORD_IDX
    M=M+1
    @CHAR_COUNT
    M=0
    (SKIP_INC)

    @WORD_IDX
    D=M
    @32
    D=D-A
    @NO_WRAP
    D;JLT
    @WORD_IDX
    M=0
    @256
    D=A
    @Y_POS
    M=D+M

    (NO_WRAP)

    @R15
    A=M
    0;JMP