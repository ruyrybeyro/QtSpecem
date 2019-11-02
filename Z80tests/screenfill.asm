;	FILL ZX SPECTRUM SCREEN 
;	GOING THROUGH EACH VISUAL LINE
;	SLOWER AND PRINTING HL IN HEXADECIMAL
;
;	SCREEN ADDRESS  
;
;       H =  0  1  0 Y7 Y6 Y2 Y1 Y0
;
;	Y7-Y6 - section
;	Y2-Y0 - lines 8x8
;
;	L = Y5 Y4 Y3 X4 X3 X2 X1 X0
;
;	Y5-Y3 - line pixels
;	X4-X0 - X

	ORG	50000

        LD      A,2             ; upper screen
        CALL    5633            ; open channel

        LD	A,(23695)	; ATTR T
	XOR	A
	OR	$32		; Yellow PAPER ; Red INK
	LD	(23695),A

	LD	HL,$4000	; SCREEN START
LOOP:
	PUSH	HL

	LD	B,32		; 32 COLUMNS
X4_X0:	
	CALL	PRINT_STAT	; PRINT HL/COORDS

	LD	(HL),A
	INC	L

        CALL    W_DELAY

	DJNZ	X4_X0

	POP	HL

	CALL	P_DOWN

	LD	A,H
	CP	$58		; TEST FOR ATTR AREA
	JR	NZ,LOOP

        ; need to return to lower screen
        LD      A,1             ; lower screen
        CALL    5633            ; open channel
	
        CALL    $15DE           ; WAIT-KEY1

	RET

; Move HL one pixel line down
P_DOWN: INC H           ; Go down onto the next pixel line
	LD 	A,H          ; Check if we have gone onto next character boundary
	AND 	7
	RET 	NZ          ; No, so skip the next bit
	LD 	A,L          ; Go onto the next character line
	ADD 	A,32
	LD 	L,A
	RET 	C           ; Check if we have gone onto next third of screen
	LD 	A,H          ; Yes, so go onto next third
	SUB 	8
	LD 	H,A
        RET


W_DELAY:
        PUSH    BC

	LD	B,50

WAIT2:	PUSH	BC
	LD	B,255
WAIT:	DJNZ	WAIT

	POP	BC
	DJNZ	WAIT2

        POP     BC
	RET

; PRINT HL/COORDS
PRINT_STAT:

	PUSH	HL
	PUSH	AF
	PUSH	BC

	; PRINT HL (hexa)
	LD	B,20	 ; COL
	LD	C,1	 ; LINE
	CALL	PRINT_AT

	LD	IX,TXTADDR
	CALL	PRINT_STRING

	LD	A,H
	CALL	PRINT_A
	LD	A,L
	CALL	PRINT_A

       ; PRINT HL (decimal)
        LD      B,20     ; COL
        LD      C,2      ; LINE
        CALL    PRINT_AT

        LD      IX,TXTADDR2
        CALL    PRINT_STRING

        CALL    PRINTDEC


	; PRINT SECTION
        LD      B,20     ; COL
        LD      C,3      ; LINE
        CALL    PRINT_AT

	LD      IX,TXTSEC
        CALL    PRINT_STRING

	LD	A,H
	AND	$18
	RRA
	RRA
	RRA
	PUSH	AF
	CALL    PRINTHEXA
	POP	AF

	; BUILDING Y
	RLA     ; ACCOUNT FOR LINE
	RLA
	RLA
	LD	D,A	; SAVE ON D

        ; PRINT LINE 
        LD      B,20     ; COL
        LD      C,4      ; LINE
        CALL    PRINT_AT

        LD      IX,TXTLINE
        CALL    PRINT_STRING

	LD	A,L
	AND	$E0
	RRA
	RRA
	RRA
	RRA
	RRA
	PUSH	AF
	CALL	PRINTHEXA
	POP	AF

	; STILL BUILDING Y
	ADD	A,D     ; SEC + LINE
	RLA             ; ACCOUNTING FOR
	RLA             ; LINECHR
	RLA
	LD	D,A	
	

       ; PRINT LINE CHR
        LD      B,20     ; COL
        LD      C,5      ; LINE
        CALL    PRINT_AT

        LD      IX,TXTCHR
        CALL    PRINT_STRING

        LD      A,H
        AND     $07
	PUSH	AF
        CALL    PRINTHEXA
	POP	AF

	; BUILDING Y
	ADD	A,D	; + LINECHR
	LD	D,A

        ; PRINT COL
        LD      B,20     ; COL
        LD      C,6      ; LINE
        CALL    PRINT_AT

        LD      IX,TXTCOL
        CALL    PRINT_STRING

	PUSH	HL
        LD      A,L
        AND     $1F
	LD	L,A
	LD	H,0
        CALL    PRINTDEC
	POP	HL

	; PRINT X
        LD      B,20     ; COL
        LD      C,7      ; LINE
        CALL    PRINT_AT

        LD      IX,TXT_X
        CALL    PRINT_STRING

        PUSH    HL
        LD      A,L
        AND     $1F	; COL
        RLA
        RLA
        RLA		; * 8
        LD      L,A
        LD      H,0
        CALL    PRINTDEC
        POP     HL

        ; PRINT Y
        LD      B,20     ; COL
        LD      C,8      ; LINE
        CALL    PRINT_AT

        LD      IX,TXT_Y
        CALL    PRINT_STRING

	LD	H,0
	LD	L,D
	CALL	PRINTDEC

	POP	BC
	POP	AF
	POP	HL
	RET

; PRINT AT LINE, COL
; B = COL
; C = LINE
;
PRINT_AT:
	LD      A,$16    ; PRINT AT

        PUSH    HL
        CALL    $201E    ; CALL PR-AT-TAB
        POP     HL
	RET

; PRINT A NULL TERMINATED STRING
; IX = STRING
;
PRINT_STRING:
	LD	A,(IX)
	CP	0
	RET	Z
	RST	$10
	INC	IX
	JR	PRINT_STRING

; =============================
; print "A" in hexa
; =============================
PRINT_A:
        PUSH	AF
        ; Highest 4 bits first
        AND     $F0
        RRA
        RRA
        RRA
        RRA

        CALL    PRINTHEXA

	POP	AF
        ; lower 4 bits of A
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
; PRINT 16 BIT SIGNED NUMBER
; HL = NUMBER
;
PRINTDEC:
	PUSH	DE
	PUSH	HL
	LD	E,$20	; SPACES

	LD	BC,$D8F0 ; -10000
	CALL	$192A ; routine OUT-SP-NO outputs space or number

	JP	$1A30	; OUT-NUM-3

TXTADDR:
	DEFB	"ADDR $"
	DEFB	0
TXTADDR2:
        DEFB    "ADDR "
        DEFB    0
TXTSEC:
	DEFB	"SECTION  "
	DEFB	0
TXTCOL:
	DEFB	"COL  "
	DEFB	0
TXTLINE:
	DEFB	"LINE     "
	DEFB	0
TXTCHR:
	DEFB	"CHRLINE  "
	DEFB	0
TXT_X:
	DEFB	"X    "
	DEFB	0
TXT_Y:
	DEFB	"Y    "
	DEFB	0

	END 50000

