
PLOT	EQU	$22DF
DRAW	EQU     $24BA

	ORG	50000
	; DRAW PIXEL RIGHTMOST DOWN CORNER
	LD	BC,0
	CALL	PLOT

	; HORIZONTAL LINE
	LD	BC,$00FF
        LD      DE,$0101 ; -->
	CALL	DRAW
	
	; VERTICAL LINE
	LD	B,175
	LD	C,0
	LD	DE,$0101 ; UP
	CALL	DRAW

	; HORIZONTAL LINE
	LD	BC,$00FF
	LD	DE,$01FF ; <----
	CALL	DRAW

	; VERTICAL LINE
        LD      B,175
        LD      C,0
        LD      DE,$FF01 ; DOWN
	CALL	DRAW

	; CIRCLE
	LD	B,128 ; X
	LD	C,90  ; Y
	LD	D,50  ; Z
	CALL	CIRCLE


RET_TO_BASIC:
	; USING FP CALCULATOR CORRUPTS HL' 
	; so we need to restore it 
	; to be able to return to BASIC
	EXX
        LD      HL,$2758
        EXX

	RET
; B = X
; C = Y
; D = RADIUS in pixels (Z)

CIRCLE:
	; PUSH  X in FP stack
	PUSH	DE
	LD	A,B
	PUSH	BC
	CALL    $2D28 ; STACK_A
	POP	BC
	; PUSH Y
	LD	A,C
	CALL    $2D28 ; STACK_A
	POP	DE
        ; PUSH Z
	LD	A,D
	CALL    $2D28 ; STACK_A

        JP      $232D ; CIRCLE - non oficial entry

	END	50000

