; EXAMPLE SAVE AND LOAD ; BLOCKS WITHOUT HEADERS

	ORG	50000

        LD      A,2             ; upper screen
        CALL    5633            ; open channel

	CALL	FILL_SCREEN

	CALL	SAVE_SCREEN

	CALL	LOAD_SCREEN

        ; need to return to lower screen
        LD      A,1             ; lower screen
        CALL    5633            ; open channel
	
        CALL    $15DE           ; WAIT-KEY1

	RET

FILL_SCREEN:
	LD	HL,$4000	; SCREEN START
LOOP:	LD	(HL),A
	INC	HL
	LD	A,H
	CP	$58		; TEST FOR ATTR AREA
	JR	NZ,LOOP
	RET

SAVE_SCREEN:
	LD	A,$FF   ; DATA BLOCK
	LD  	IX,16384 ; grava a partir de 16384
	LD  	DE,6912  ; 6912 bytes
	CALL 	$04C2  ; call SA_BYTES
	RET

LOAD_SCREEN:
	LD 	A,$FF ; DATA BLOCK
	LD 	IX,1683
	LD 	DE,6912
	CALL 	$0556 ; call LD_BYTES
	RET

