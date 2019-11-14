; EXAMPLE SAVE AND LOAD ; BLOCKS WITH HEADERS

	ORG	50000

        LD      A,2             ; upper screen
        CALL    5633            ; open channel

	CALL	FILL_SCREEN

	CALL	SAVE_SCREEN

        CALL    3503            ; CLS

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
	LD 	HL,16384
	LD  	IX,HEADER
	CALL 	$0970 ; SA-CONTRL
	RET

HEADER: DEFB 	$03
        DEFM 	"ECRA      " ; 10 BYTES
        DEFW 	6912	     ; LEN
        DEFW 	16384        ; BEGIN
        DEFW 	0

LOAD_SCREEN:
	LD 	A,(T_ADDR)
	LD 	(T_ADDR2),A
	LD 	IX,ExpectedHeader
	LD 	HL,0
	LD 	A,1 ; LOAD nesta rotina de ROM é 1
	LD 	(T_ADDR),a
	CALL 	SA_ALL ;command multiplexor
	LD 	A,(T_ADDR2)
	LD 	(T_ADDR),A
	RET

ExpectedHeader:
	DB 	3 ;expected type (Bytes)
        DEFM    "ECRA      " ; 10 BYTES
	;DB	0ffh ;name = ""
	;DS 	9
	DW 	0 ; size = 0 (use tamanho do header em cassette)
	DW 	0 ; start = 0 (usar começo do header em cassette)
	DW 	0 ;
	DS 	17 ; espaço em RAM para carregar header da cassete
T_ADDR2:
	DB 0

T_ADDR equ 5c74h
SA_ALL equ 075Ah
