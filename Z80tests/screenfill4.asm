;	FILL ZX SPECTRUM SCREEN GOING THROUGH EACH VISUAL LINE
;	SLOWER AND PRINTING HL IN HEXADECIMAL
;
;	SCREEN ADDRESS  
;
;       H =  0  1  0 Y7 Y6 Y2 Y1 Y0
;	L = Y5 Y4 Y3 X4 X3 X2 X1 X0

	ORG	50000

	LD	HL,$4000	; SCREEN START
LOOP:
	PUSH	HL

	LD	B,32		; 32 COLUMNS
X4_X0:	
	CALL	PRINT_HL

	LD	(HL),A
	INC	L

        CALL    W_DELAY

	DJNZ	X4_X0

	POP	HL

	CALL	P_DOWN

	LD	A,H
	CP	$58
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

PRINT_HL:
	; PRINT AT
	PUSH	HL
	PUSH	AF
	PUSH	BC

	LD	B,27	 ; COL
	LD	C,0	 ; LINE
	LD      A,$16    ; PRINT AT

	PUSH	HL
	CALL	$201E	 ; CALL PR-AT-TAB
	POP	HL

	; PRINT HL
	LD	A,H
	CALL	PRINT_A
	LD	A,L
	CALL	PRINT_A

        LD      A,13            ; NEW LINE
        RST     $10

	POP	BC
	POP	AF
	POP	HL
	RET

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

	END 50000

