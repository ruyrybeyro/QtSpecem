;	FILL ZX SPECTRUM SCREEN GOING THROUGH EACH VISUAL LINE
;
;	SCREEN ADDRESS  
;
;       H =  0  1  0 Y7 Y6 Y2 Y1 Y0
;	L = Y5 Y4 Y3 X4 X3 X2 X1 X0

; pasmo --tapbas screenfill1.asm screenfill1.tap


	ORG	50000

	LD	HL,$4000	; SCREEN START

LOOP:	; LD	A,255
	LD	(HL),A
	INC	HL

        CALL    W_DELAY

	LD	A,H
	CP	$58
	JR	NZ, LOOP

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

