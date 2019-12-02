	
	; BASIC ANTI-BREAK
	; BASED IN YS/WOS ROUTINE
	; RUI RIBEIRO/2019

	;ORG	$5CD0
	ORG	60000
START:
	; HL,(HANDLER)
	LD HL,HANDLER-START
	ADD HL,BC   ; HL=HANDLER

        PUSH HL
        POP  IX	    ; LD IX,HANDLER

	; self modifying code
	; update handled address loaded
        ; in HL in HANDLER: line
	; LD (HANDLER+1),HANDLER
	LD (IX+1),L ; HANDLER+1
	LD (IX+2),H ; HANDLER+2

	; Update ERR_SP pointer
	PUSH HL			; preserve handler address
	; GET ADDRESS pointed by ERR_SP
	; HL,(ERR_SP)
  	LD L,(IY+3) ; $5C3D ERR_SP
  	LD H,(IY+4) ; $5C3E ERR_SP
	PUSH	HL
	POP	IX	; LD IX,(ERR_SP)
	POP	HL
	; LD (IX),HANDLER
	LD 	(IX+0),L ; 
	LD	(IX+1),H ; 
	RET
HANDLER:
	LD HL,HANDLER		; value will be modified
	LD A,(IY+0)             ;ERR NR Sys Var 1 less than error code
	CP $14			;L BREAK into program
	JR Z,label1		; =
	CP $0C			;  D BREAK - CONT repeats
				; usual value pushed in (ERR_SP) 
	JP NZ,$1303             ;!= GO  MAIN_4
label1:  
	PUSH HL			; PUSH HANDLER
	LD (IY+0),$FF           ;  $5C3A ERR-NR - 
				; 0-1 clear error 
	RES 5, (IY+1)           ;  $5C3B 
                                ; FLAGS  - 5 Set when a new key has been pressed
	                        ; unset it

	JP $1B7D ; STMT_R_1 - Continue here as the BREAK key was not pressed.

