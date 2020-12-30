; PIRATA 2-3 bug fixed, code optimized and smaller
; Rui Ribeiro - Dez/2020
;
; border cyan/red - introduce tape to copy
;                   SPACE to force end?
;
; border yellow   - introduce blank tape and press 1 (any key??)
;
; border green    - 1 another copy
;                   2 start over
;
; FORMAT of tape block in memory
;
;  BYTE 1 - LSB size of block
;  BYTE 2 - MSB size of block	
;  BYTE 3 - flag identifying block
;  -------- block 

; if BYTE1 and BYTE2 = 0, no more blocks

		ORG 	$FF67
	
		DI			; only needed once
		LD	SP,0		; stack to end of RAM

BEGIN:		LD	IX,$4000	; beggining of RAM/screen
L_NEXTBLK:		
		XOR	A		; header block
		LD      (IX+02),A
		PUSH	IX
		INC	IX		; size of block LSB
		INC	IX              ; size of block MSB
		INC	IX              ; flag
		LD	DE,BEGIN-$4000-5 ; max number of bytes
		SCF
		INC	D
		EX	AF,AF'
		DEC	D

		LD	A,$0F		; border white
		OUT	($FE),A		

		CALL	$0562    	; load block
		JR	Z,END_LOAD	; load w/ sucess

		LD	A,$7F
		IN 	A,($FE)
		RR	A
		JR	NC,SAVE_SECTION	; if SPACE 

		LD	(IX-01),L	; save flag block identifier byte in RAM


		CALL	$05B7		; load entry after flag check.

		    ; subtract from max number of bytes
                    ; to get bytes loaded
END_LOAD:	LD	HL,$BF54	
		OR	A		; carry =0
		SBC	HL,DE		; HL=bytes loaded

		PUSH	HL
		POP	DE		; DE=bytes loaded(+checksum?)
		DEC	DE		; DE=bytes loaded
		POP	HL		; POP IX before loading, beggining of block
		    
		
		LD	(HL),E          ; (HL) = size (word) at beggining of block
		INC	HL
		LD	(HL),D
		DEC	IX		; loading leaves IX at IX+1
		JR	L_NEXTBLK	; next block

SAVE_SECTION:	POP	HL		; POP IX before loading

		XOR	A
		LD	(HL),A		; (IX) = 0 word, no more blocks
		INC	HL
		LD	(HL),A		

		LD	A,06            ; border yellow
		OUT	($FE),A

		CALL	DELAY	

		    ; wait for any key
ANYKEY:		XOR	A
		IN 	A,($FE)
		RR	A
		JR 	C,ANYKEY		

BEGIN_SBLOCK:	LD	IX,$4000
NEXT_SBLOCK:	CALL	DELAY	
		LD	E,(IX+00)	
		LD	D,(IX+01)
		LD	A,E
		OR	D
		JR	Z,EXIT_SAVE	; size = 0, jump to FFD2
		LD	A,(IX+02)
		INC	IX
		INC	IX
		INC	IX

		CALL	$04C6 		; CALL	SA_BYTES

		DEC	IX
		JR	NEXT_SBLOCK	; save next block

EXIT_SAVE:	LD	A,04         	; border green
		OUT	($FE),A

		LD	BC,$F7FE
WAIT_1_2:	IN	A,(C)
		BIT	1,A	    	; key "2"
		JR	Z,BEGIN      	; begin from scratch
		BIT	0,A         	; key "1"
		JR	Z,BEGIN_SBLOCK	; save again another copy
		JR	WAIT_1_2	; wait cycle

		; aprox 0.9 seconds
DELAY:		LD	BC,$FFFF	    
DLOOP:		DEC	BC
		LD	A,B
		OR	C
		JR	NZ,DLOOP
		RET
STACK		DEFS	(6)	

		END	$FF67
