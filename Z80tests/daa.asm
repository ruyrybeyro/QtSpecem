; N C H   Loop to be made
;
; 0 0 0   00
; 0 0 1   10
; 0 1 0   01
; 0 1 1   11
; 1 0 0   02
; 1 0 1   12
; 1 1 0   11
; 1 1 1   13
;
; F       H x x N C

        ORG     50000

        LD      A,2             ; upper screen
        CALL    5633            ; open channel

        LD      HL,TABLE        ; point to flags table
        LD      B,8             ; 8 flags combination

LOOP1:
        PUSH    BC

        LD      B,255           ; Loop 256 A possible values

        LD      D,0             ; Start with "A" == 0
        ; DE will be loaded into AF

LOOP2:  LD      E,(HL)          ; LOAD "F" from table

        CALL    DAATEST

        INC     D
        DJNZ    LOOP2

        LD      E,(HL)          ; we need this for FF
        CALL    DAATEST

        POP     BC

        INC     HL              ; next value in flag table
        DJNZ    LOOP1
        RET

; Flag table
TABLE:  DEFB    00, $10, $01, $11, $02, $12, $11, $13

; =============================
; DAA wrapper
; =============================
DAATEST: 
        PUSH    HL
        PUSH    BC
        PUSH    DE

	; AF = DE
        PUSH    DE
        POP     AF

        CALL    PRINT

        ; reload DE
        POP     DE
        PUSH    DE

        ; AF = DE
        PUSH    DE
        POP     AF

        DAA                     ; instruction to test

        CALL    PRINT

        LD      A,13            ; NEW LINE
        RST     $10

        POP     DE
        POP     BC
        POP     HL
        RET

; =============================
; print line FLAGS A FLAGS' A'
; =============================
PRINT:  PUSH    AF

        PUSH    AF
        POP     DE

        ; PRINT N FLAG
        LD      A,$4E           ; "N"
        RST     $10

        LD      A,$20           ; " "
        RST     $10

        LD      A,$30           ; "0"
        BIT     1,E
        JR      Z,ISZERO1
        INC     A		; "1"
ISZERO1: 
        RST     $10

        ; PRINT C FLAG

        LD      A,$20           ; " "
        RST     $10

        LD      A,$43           ; "C"
        RST     $10

        LD      A,$20           ; " "
        RST     $10

        LD      A,$30           ; "0"
        BIT     0,E
        JR      Z,ISZERO2
        INC     A               ; "1"
ISZERO2: 
        RST     $10

        LD      A,$20           ; " "
        RST     $10

        ; PRINT H FLAG
        LD      A,$48           ; "H"
        RST     $10

        LD      A,$20           ; " "
        RST     $10

        LD      A,$30           ; "0"
        BIT     4,E
        JR      Z,ISZERO3
        INC     A		; "1"
ISZERO3: 
        RST     $10

        LD      A,$20           ; " "
        RST     $10

        ; print "A" in hexa
	CALL    PRINTA

        LD      A,$20           ; " "
        RST     $10

        POP     AF
        RET

; =============================
; print "A" in hexa (stored in D)
; =============================
PRINTA:
        ; Highest 4 bits first
        LD      A,D
        AND     $F0
        RRA
        RRA
        RRA
        RRA

        CALL    PRINTHEXA

        ; lower 4 bits of A
        LD      A,D
        AND     $0F
        CALL    PRINTHEXA
        RET

; =============================
; print lower 4 bits of A in hexa
; =============================
PRINTHEXA:
        PUSH    DE

        LD      E,$30           ; "0"
        CP      10
        JR      C,ISDIGIT
        LD      E,$37           ; "A" - 10
ISDIGIT: 
        ADD    A,E
        RST     $10

        POP     DE
        RET

