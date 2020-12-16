;	FILL ZX SPECTRUM SCREEN GOING THROUGH EACH VISUAL LINE
;
;	SCREEN ADDRESS  
;
;       H =  0  1  0 Y7 Y6 Y2 Y1 Y0
;	L = Y5 Y4 Y3 X4 X3 X2 X1 X0

; pasmo --tapbas screenfill2.asm screenfill2.asm

	ORG	50000

	LD	HL,$4000	; SCREEN START

	LD	B,3		; 3 SECTIONS
Y7_Y6:	PUSH	BC

        LD	B,8		; NEXT LINE - 8 TEXT LINES
Y5_Y3:  PUSH	BC	

	LD	B,8		; CHAR SECTION - 8 VERTICAL PIXELS

Y2_Y0:  PUSH	BC
	LD	B,31		; 32 COLUMNS

	LD	A,255
X4_X0:	
	LD	(HL),A
	INC	L

        CALL    W_DELAY

	DJNZ	X4_X0

	LD      (HL),A		; FILL THE 32nd COLUMN

	; back to column 0
	LD	A,L
	AND	$E0
	LD	L,A

	INC	H		; NEXT CHAR "LINE"
        CALL    W_DELAY


	POP	BC
	DJNZ	Y2_Y0

	DEC	H		; ONE TOO MUCH

	; Zero Y2-Y0
	LD	A,H
	AND	$58
	LD	H,A

	; Add 1 to Y5-Y3
	LD	A,L
        ADD	A,$20
	LD	L,A
	
	POP	BC
	DJNZ	Y5_Y3

	; Add one to section (Y7-Y6)
        LD	A,H
	ADD	A,$8
	LD	H,A

	POP	BC
	DJNZ	Y7_Y6

        ; need to return to lower screen
        LD      A,1             ; lower screen
        CALL    5633            ; open channel

        CALL    $15DE           ; WAIT-KEY1

	RET

W_DELAY:
        PUSH    BC

	LD	B,255
WAIT:	DJNZ	WAIT
	LD	B,255
WAIT2:	DJNZ	WAIT2

        POP     BC
	RET

	END 50000

