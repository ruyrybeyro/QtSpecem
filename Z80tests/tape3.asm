; EXAMPLE SAVE AND LOAD ; BLOCKS WITHOUT HEADERS

	ORG	50000

        LD      A,2             ; upper screen
        CALL    5633            ; open channel

	CALL	LOAD_HEADER

        ; need to return to lower screen
        LD      A,1             ; lower screen
        CALL    5633            ; open channel
	
        CALL    $15DE           ; WAIT-KEY1

	RET


LOAD_HEADER:
	LD 	A,$00 ; HEADER BLOCK
	LD 	IX,1683
	LD 	DE,17
	CALL 	$0556 ; call LD_BYTES
	RET

