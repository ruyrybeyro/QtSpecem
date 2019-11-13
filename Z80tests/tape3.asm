; EXAMPLE TAPE : PRINT HEADERS

	ORG	50000

        LD      A,2             ; upper screen
        CALL    5633            ; open channel

	CALL	LOAD_HEADER

	CALL	PRINT_HEADER

        ; need to return to lower screen
        LD      A,1             ; lower screen
        CALL    5633            ; open channel
	
        CALL    $15DE           ; WAIT-KEY1

	RET


LOAD_HEADER:
	SCF
	LD 	A,$00 ; HEADER BLOCK
	LD 	IX,Header
	LD 	DE,17 ; 17 bytes
	CALL 	$0556 ; call LD_BYTES
	RET

Header:
        DB      0 ;expected type
        DS      10 ; name
        DW      0 ; size = 0 (use tamanho do header em cassette)
        DW      0 ; start = 0 (usar começo do header em cassette)
        DW      0 ;

PRINT_HEADER:
        LD      IX,Header

        LD	A,(IX)
        ADD	A,'0'
	CALL	PR_CHR

        LD	B,10
NLOOP:	INC	IX
	LD	A,(IX)
	CALL	PR_CHR
	DJNZ	NLOOP

	RET

;	PRINT CHAR
PR_CHR:	PUSH	IX
	RST	$10
	POP	IX
	RET
	END	50000

