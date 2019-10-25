;	FILL ZX SPECTRUM SCREEN GOING THROUGH EACH VISUAL LINE
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

	LD	B,255
WAIT:	DJNZ	WAIT
	LD	B,255
WAIT2:	DJNZ	WAIT2

        POP     BC
	RET

	END 50000

