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

MAX_SIZE	EQU	BEGIN-$4000-5

		ORG 	$FF6A
START:	
		DI			; only needed once
		LD	SP,0		; stack to end of RAM

BEGIN:		LD	HL,$4000	; beggining of RAM/screen

L_NEXTBLK:		
		PUSH    HL	
		INC	HL		; size of block LSB
		INC	HL              ; size of block MSB
		XOR	A		; header block
		LD      (HL),A		; flag
		INC	HL              
		LD	DE,MAX_SIZE 	; max number of bytes
		PUSH	HL
		POP	IX
		SCF
		INC	D
		EX	AF,AF'
		DEC	D

		LD	A,$0F		; border white
		OUT	($FE),A		

		CALL	$0562    	; load block
		JR	Z,END_BLOCK	; load w/ sucess

		LD	A,$7F
		IN 	A,($FE)
		RR	A
		JR	NC,END_LOAD	; if SPACE 

		LD	(IX-01),L	; save flag block identifier byte in RAM


		CALL	$05B7		; load entry after flag check.

		    ; subtract from max number of bytes
                    ; to get bytes loaded
END_BLOCK:	LD	HL,MAX_SIZE
		OR	A		; carry =0
		SBC	HL,DE		; HL=bytes loaded

		PUSH	HL
		POP	DE		; DE=bytes loaded(+checksum?)
		DEC	DE		; DE=bytes loaded
		POP	HL		; POP IX before loading, beggining of block
		    
		
		LD	(HL),E          ; (HL) = size (word) at beggining of block
		INC	HL
		LD	(HL),D
		PUSH	IX
		POP	HL
		DEC	HL		; loading leaves IX at IX+1
		JR	L_NEXTBLK	; next block

END_LOAD:	POP	HL		; POP IX before loading

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

BEGIN_SBLOCK:	LD	HL,$4000
NEXT_SBLOCK:	CALL	DELAY	
		LD	E,(HL)	
		INC	HL
		LD	D,(HL)
		LD	A,E
		OR	D
		JR	Z,EXIT_SAVE	; size = 0, jump to FFD2
		LD	A,(IX+02)
		INC	HL
		INC	HL

		PUSH	HL
		POP	IX

		CALL	$04C6 		; CALL	SA_BYTES

		PUSH	IX
		POP	HL
		DEC	HL
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
STACK		DEFS	(8)	

		END	START
