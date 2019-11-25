; EXAMPLE SAVE AND LOAD ; BLOCKS WITHOUT HEADERS

	ORG	50000

        LD      A,2             ; upper screen
        CALL    5633            ; open channel

	LD	IX,TAPE_TXT1
	CALL	PRINT_STRING

	CALL    WAIT_KEY

	CALL	FILL_SCREEN

	CALL	SAVE_SCREEN

	CALL    CLEAR_SCREEN

        LD      IX,TAPE_TXT2
        CALL    PRINT_STRING

        CALL    WAIT_KEY  

	CALL	LOAD_SCREEN

        LD      A,1             ; lower screen
        CALL    5633            ; open channel

        JP      WAIT_KEY   

;	RET

FILL_SCREEN:
	LD	HL,$4000	; SCREEN START
LOOP:	LD	(HL),A
	INC	HL
	LD	A,H
	CP	$58		; TEST FOR ATTR AREA
	JR	NZ,LOOP
	RET

CLEAR_SCREEN:
	LD      HL,$4000        ; SCREEN START
LOOP2:  XOR	A
	LD      (HL),A
        INC     HL
	LD	A,H
        CP      $58             ; TEST FOR ATTR AREA
        JR      NZ,LOOP2
        RET

SAVE_SCREEN:
	LD	A,$FF   ; DATA BLOCK
	LD  	IX,$4000 ; grava a partir de 16384
	LD  	DE,$1B00  ; 6912 bytes
	JP 	$04C2  ; call SA_BYTES

LOAD_SCREEN:
	LD 	A,$FF ; DATA BLOCK
	LD 	IX,$4000
	LD 	DE,$1B00
	SCF	      ; LOAD
	JP 	$0556 ; call LD_BYTES

; PRINT A NULL TERMINATED STRING
; IX = STRING
;
PRINT_STRING:
        LD      A,(IX)
        CP      0
        RET     Z
        RST     $10
        INC     IX
        JR      PRINT_STRING

WAIT_KEY:
  	ld hl,23560         ; LAST K system variable.
       	ld (hl),0           ; put null value there.
NOKEY:
   	ld a,(hl)           ; new value of LAST K.
       	cp 0                ; is it still zero?
       	jr z,NOKEY          ; yes, so no key pressed.
       	ret                 ; key was pressed.

TAPE_TXT1:
	DB	"Press any key when ready for saving"
	DB	0

TAPE_TXT2:
	DB	"Go backwards on tape"
	DB	$0D
	DB	"Press any key when ready"
	DB	0

	END 	50000

