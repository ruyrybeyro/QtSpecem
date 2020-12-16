; http://www.breakintoprogram.co.uk/computers/zx-spectrum/keyboard

; pasmo --tapbas keyboard.asm keyboard.tap

					ORG	50000
					CALL	PRINT_KEYB
STALL:				        CALL	Read_Keyboard
					JR	STALL

        				; need to return to lower screen
        				LD      A,1             ; lower screen
        				CALL    5633            ; open channel

					RET


PRINT_KEYB:			        CALL	3503 		; CLS
					LD      A,2             ; upper screen
        				CALL    5633            ; open channel

					LD	HL,Keyboard_Map
PRLOOP:					LD	A,(HL)
					CP	0
					RET     Z
					INC	HL
					LD	B,5
PRLOOP2:				LD	A,(HL)
					RST	$10
					INC	HL
					DJNZ	PRLOOP2
					LD	A,13
					RST	$10
					JR	PRLOOP

					; "stolen" and adapted keyboard scanning rows routine
					; from http://www.breakintoprogram.co.uk/computers/zx-spectrum/keyboard
Read_Keyboard:				LD HL,Keyboard_Map		; Point HL at the keyboard list
					LD IX,$5800			; Beginning of attribute area
					LD D,8					; This is the number of ports (rows) to check
					LD C,&FE				; C is always FEh for reading keyboard ports
Read_Keyboard_0:			LD B,(HL)				; Get the keyboard port address from table
					INC HL					; Increment to list of keys
					IN A,(C)				; Read the row of keys in
					AND &1F					; We are only interested in the first five bits
					LD E,5					; This is the number of keys in the row
Read_Keyboard_1:			SRL A					; Shift A right; bit 0 sets carry bit
					PUSH	AF
					LD	A,$34
					JR C,no_key	; If the bit is 0, we've found our key
					LD	A,$14
no_key:					LD	(IX),A
					POP	AF
					INC HL					; Go to next table address
	                                INC IX
					DEC E					; Decrement key loop counter
					JR NZ,Read_Keyboard_1 	; Loop around until this row finished
					PUSH	BC
					LD	BC,27		; next row of attributes
					ADD 	IX,BC
					POP	BC
					DEC D					; Decrement row loop counter
					JR NZ,Read_Keyboard_0	; Loop around until we are done
					AND A					; Clear A (no key found)
					RET

Keyboard_Map:				DB &FE,"#","Z","X","C","V"
					DB &FD,"A","S","D","F","G"
					DB &FB,"Q","W","E","R","T"
					DB &F7,"1","2","3","4","5"
					DB &EF,"0","9","8","7","6"
					DB &DF,"P","O","I","U","Y"
					DB &BF,"#","L","K","J","H"
					DB &7F," ","#","M","N","B"
					DB 0

					END 50000

