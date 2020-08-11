;
; kempston/cursor/fuller/sinclair/timex 2068 joystick detection and testing
; Rui Ribeiro/2020
;
; MIT License
;
; pasmo --tapbas joy.asm joy.tap
;

CL_ALL		EQU     $0DAF ; clearing the whole display area

; UDGs EQUs

; ZX Spectrum sysvar for defining UDGs
UDG		EQU	$5C7B

;
; UDG CHARs
;

UUP     	EQU     $90
ULEFT   	EQU     $91
UFIRE   	EQU     $92
URIGHT  	EQU     $93
UDOWN   	EQU     $94

; Print EQUs

; AT control code
AT      	EQU     $16

; line and column for the joystick status
LINE1   	EQU     10
COL     	EQU     13

; ATTR EQUs

ATTR_BIT_ON	EQU	$3A ; red   ink, white paper
ATTR_BIT_OFF	EQU	$38 ; black ink, white paper

; beginning of attr area
ATTR		EQU	$5800

; first actual memory ATTR location (UP)
FIRST_ATTR	EQU	ATTR+(LINE1*32)+COL

; memory offsets from first location
; so ATTR mapping tables can be 1 byte
; ought not to be 0, 0 is for ignoring a bit

A_UP     	EQU     2      ; ATTR+(LINE1*32)+COL+2              
A_LEFT   	EQU     2*32   ; ATTR+((LINE1+2)*32)+COL              
A_FIRE   	EQU     2*32+2 ; ATTR+((LINE1+2)*32)+COL+2
A_RIGHT  	EQU     2*32+4 ; ATTR+((LINE1+2)*32)+COL+4
A_DOWN   	EQU     4*32+2 ; ATTR+((LINE1+4)*32)+COL+2

; Begin of program/asm code

	ORG	32768

	; point UDGs to our table
	LD	HL,UDGs		
	LD 	(UDG),HL		

	; clear screen
	CALL	CL_ALL

	; print our screen
	LD	HL,MAIN_SCREEN
	CALL	PRINT

L_DETECT:
	; if space pressed leave
        CALL    T_SPACE
	RET	NZ

	; wait for an interrupt
	HALT               ; CALL    DELAY

	; detect joystick type
	CALL	DETECT_JOY

	; go to loop if not detected
	JR	C,L_DETECT

	; print joystck name
        CALL	JOY_NAME

	 
L_TEST:	
	; if CAPS+space (BREAK) pressed leaves to BASIC
	CALL	T_SPACE
	RET	NZ

	; wait for an interrupt
	HALT                ; CALL	DELAY

        ; show joytstick actions	
        CALL	TEST_JOY

	; loop into L_TEST
        ; end is via BREAK key test
	JR	L_TEST

	; Never reachs here
	;RET

;
; loads joystick port
; Entry:
;      HL=pointing to first element of detection array
; Exit:
;      BC=joytick port to be tested
;      HL=pointing to second element of detection array
;      C=1 end of detection array
;      if C=0, 
;		A with IN value
;		E with value to be tested
;

LOAD_JOY_PORT:
	; gets BC to be tested from table
	; BC==0 end of table
        LD      C,(HL)
        INC     HL
        LD      B,(HL)
        INC     HL

	; if BC=0 we have reached the end of the detection array
        LD      A,B
        OR      C
	JR	NZ,ALL_OK

	SCF	; returns C=1 (end,error, search exausted)
        RET     

ALL_OK:        
        IN      A,(C)	;	A=read joystick BC port
        LD      E,(HL)	;	E=expected left direction value from detection array
        INC     HL
        RET

;
; DETECT JOYSTICK
; Returns:
; C=1 error
; otherwise
; BC=PORT to read
; HL pointing to third element of detection array
;

DETECT_JOY:
	; exception for Kempston
        LD 	HL,KEMPSTON_P	; point to beginning of joystick detection array
        CALL	LOAD_JOY_PORT   ; get joytick port in BC and A with read value, 
				; E with value to test
        CP	E 		; exception, kempston must be detected as is
	JR	Z,DETECTED

	; less bytes duplicating code for 
        ; dealing with 2nd Kempston port exception
        LD      HL,KEMPSTON_P2  ; point to beginning of joystick detection array
        CALL    LOAD_JOY_PORT   ; get joytick port in BC and A with read value,
                                ; E with value to test
        CP      E               ; exception, kempston must be detected as is
        JR      Z,DETECTED

LOOP_JOY:
        LD	BC,4		; point to next joystick
	ADD	HL,BC
	CALL	LOAD_JOY_PORT	; get joytick port in BC and A with read value
				; E with value to test
	RET	C		; return if no more joyticks in table, C=1 error
	CPL			; the rest of joystick makes are active low
				; negate bits
	AND	E		
	JR	Z,LOOP_JOY	; if value not as expected, try next joystick in table
DETECTED:
        ;SCF                     ; C=1
	;CCF			; C=0, success
	XOR	A		; A not used
	RET

;
; PRINTS JOYSTICK NAME
; HL= pointer to detected joystick name
; Returns:
; HL pointing to fourth element of detection array
;

JOY_NAME:
	PUSH	BC
	LD	E,(HL)
	INC	HL
	LD	D,(HL)		; DE=string of joystick name
	INC	HL
        PUSH	HL

	; copy DE to HL
	;PUSH	DE
	;POP	HL
	EX	DE,HL

	CALL	PRINT

	; print 10 spaces
	LD	B,10
SPACES:	LD	A,' '
	RST	$10
	DJNZ	SPACES

        POP	HL
	POP	BC
	RET

;
; TEST JOYSTICK
; ENTRIES:
;        HL pointing to addr of 8 words mapping bits to ATTR
;        BC port of joystick
;

TEST_JOY:
	PUSH	HL
	PUSH	BC

; dealing with exceptions to the rules
; Cursor joytick reads two I/O ports


        ; D' = value to be ORed later on to joystick
        ; port reading
;	XOR	A
;	LD	(CURS_TMP),A
	EXX
	LD	D,0	
	EXX

	; test if dealing with cursor joystick	
	LD	DE,CURSORP

	; 16 bit CP HL,DE	
	OR	A
	SBC	HL,DE
	ADD	HL,DE
	JR	NZ,NO_CURSOR	; jump if HL not equal CURSORP

	; read from cursor Joystick, first port
	; $F7FE row 1-5, for 5 (left)
	IN	A,(C)
	CPL	
	AND	$10	; and with key 5
	; shift it to an unused bit on the other port
	; store it in D'
        SRA     A 
	SRA	A
	SRA	A       
	EXX
        ;LD      (CURS_TMP),A
	LD	D,A
	EXX

	; replace port for cursor joystick for rest of buttons
        ; keyboard row 6-0
	LD	BC,$EFFE

NO_CURSOR:	

	LD	E,(HL)
	INC	HL
	LD	D,(HL)
			; DE pointing to attr 8 word array now

	IN	A,(C)   ; Read joystick port

	LD	L,A     ; save it in L

	; print BC and A from IN A,(C)
	CALL	PRINT_JPORT_A

        ; Kempston joytick is not active low

	; We can get away comparing only the lower byte
	; since no other joytick uses those lower ports

	LD	A,$1F
	CP	C	
	JR	Z,IS_KEMPSTON
        LD      A,$37
        CP      C
        JR      Z,IS_KEMPSTON

	; other joysticks besides kempston are active low
	; invert bits
	LD	A,L
	CPL		; invert bits reading

	; still dealing with the Cursor exception
	; adding back the left key, another port
        ; on bit 1, unused bit of the port with the most keys
        ; if not cursor joystick, D' will be 0

	LD	L,A

	; load D' into A
	EXX
	;LD	A,(CURS_TMP)
	LD	A,D
	EXX

	OR	L	; OR joystick reading with A (formely D')
	LD	L,A	; store it

IS_KEMPSTON:

; now is time for rotating the 8 bits of joystick input

	LD	B,8

; we will need HL, use C for the joy port input

        LD      C,L
BITS:	
	; get screen attribute offset in A
	; from corresponding bit behaviour array
	; if 0, the offset will be calculated
	; but no action taken
	LD	A,(DE)
	INC	DE	; point to next bit

	; build attribute byte in HL
	LD	HL,FIRST_ATTR
	; HL=HL+A
	ADD	A,L
	LD	L,A
	JR	NC,NO_C
	INC	H

	; HL=attribute area address

NO_C:	SLA	C	; shift left, bit 7 into carry
	JR	NC,BIT_0

        CP	0	; if behaviour is 0, no action
	JR	Z,BIT_1

	; in direction or fire, change UDG atribute colour	
	LD	(HL),ATTR_BIT_ON  ; ink red
	JR	BIT_1

	; in inaction, possible restoration to former colour
BIT_0:	LD	(HL),ATTR_BIT_OFF ; ink black

BIT_1:	DJNZ	BITS	; B not 0, go to next bit

	POP	BC
	POP	HL
	RET

;	
; Print a string in HL
; terminated by $
;
; INPUT: HL=$ terminated string address
;

PRINT:	
	; get char
	LD	A,(HL)

	CP	'$'	; if $
	RET	Z	; leave

	RST	$10	; print it
	INC	HL	; go to next char in string
	JR	PRINT	; loop to PRINT

;
;DELAY:	
;	HALT
;	RET

;
; INPUT: none
; returns: Z=1 if CAPS+SPACE pressed
;

T_SPACE:
        PUSH	BC

	; first test for SPACE key
	LD	BC,$7FFE	; keyboard row SPACE-B
	IN	A,(C)		; read half key row port
	CPL			; active low, invert bits
	AND	1		; test for SPACE
	JR	Z,T_END		; jump if not space

        ; if SPACE key pressed
        ; now test for CAPS key
	LD	BC,$FEFE	; keyboard row CAPS-V
	IN	A,(C)		; read half key row port
	CPL                     ; active low, invert bits
	AND	1               ; test for CAPS

T_END:	POP	BC
	RET

;
; Print BC and A (Joystick port and reading)
;
; INPUT: BC = joystick port
;        A  = result from IN A,(C)
;
PRINT_JPORT_A:
	; save registers use by routine
        ; and by rst $10, that we need ahead
	EXX
	PUSH	DE	; D' used by RST $10
	EXX
;	PUSH	BC
	PUSH	HL
	PUSH	AF

	; position cursor
	LD      HL,JOY_PORT
        CALL    PRINT

	; print detected joystick port in BC
	CALL	PRINT_BC

	; print a space
	LD	A,' ' 
	RST	$10
	
	; restore copy of read A byte from joystick
	POP	AF
	; and print it
	CALL	PRINTA

	; restore registers
	POP	HL
;	POP	BC
	EXX	
	POP	DE
	EXX
        ; return
	RET

;
; Print BC
;
; INPUT: BC holding 16 bit value to be printed in hexa (0000-FFFF)
;
PRINT_BC:

	; print B
	LD	A,B
	CALL	PRINTA

        ; print C
	LD	A,C

	; intead of calling it and returning
	; just drop on it
	;CALL	PRINTA

; 
; print "A" in hexa 
; 
; INPUT: A holding 8 bit value to be printed in hexa (00-FF)
;
PRINTA:
	PUSH	AF
        ; Highest 4 bits first
        AND     $F0
        RRA
        RRA
        RRA
        RRA

        CALL    PRINTHEXA

        ; lower 4 bits of A
	POP	AF
        AND     $0F
        CALL    PRINTHEXA
        RET

;
; print lower 4 bits of A in hexa
; 
; INPUT: A holding in lower 4 bits value to be printed in hexa (0-F)
;
PRINTHEXA:
        PUSH    DE

        LD      E,$30           ; "0"
        CP      10
        JR      C,ISDIGIT
        LD      E,$37           ; "A" - 10
ISDIGIT:
        ADD    A,E
        RST     $10

        POP     DE
        RET

;
; DATA
;

;
; We store it in D'
; Cursor Joystick status of left direction
;CURS_TMP:  DEFB	0
;

;
;Array: Detection port
;       Value
;       Pointer to descriptive text
;       Pointer to bits behaviour 
;   

; First must come the two kempston ports
; ASM code counts on it

KEMPSTON_P:
           DEFW $1F		; kempston port
           DEFB $02		; left
           DEFW KEMPSTON_T	; "Kempston$"
           DEFW KEMPSTON_B	; array pointer of Kempston 8 bit behaviour
KEMPSTON_P2:
           DEFW $37             ; 2nd kempston port
           DEFB $02             ; left
           DEFW KEMPSTON_T      ; "Kempston$"
           DEFW KEMPSTON_B      ; array pointer of Kempston 8 bit behaviour

; joysticks with bits active low

FULLER_P:  DEFW $7F		; fuller port
           DEFB $04		; left
           DEFW FULLER_T	; "Fuller$"
           DEFW FULLER_B	; array pointer of Fuller 8 bit behaviour
CURSOR_P:  DEFW $F7FE		; Cursor first port (1-5 row)
           DEFB $10		; test for 5
           DEFW CURSOR_T	; "Cursor$"
CURSORP:   DEFW CURSOR_B	; array pointer of Cursor 8 bit behaviour
SINCLAIR:  DEFW $F7FE		; Sinclair port (1-5 row)
           DEFB $01		; "Sinclair$"
           DEFW SINCLAIR_T
           DEFW SINCLAIR_B	; array pointer of Sinclair 8 bit behaviour
SINCLAIR_2: 
           DEFW	$EFFE		; Sinclair port (6-0 row)
	   DEFB $10		; test for 6
	   DEFW SINCLAIR_T	; "Sinclair$"
           DEFW SINCLAIR_B2	; array pointer of Sinclair 8 bit behaviour
T2068_P1:  DEFW $01F6		; Timex 2068 port of 1st Joystick
           DEFB $04		; left
           DEFW T2068_T		; "Timex 2068$"
           DEFW T2068_B		; array pointer of Timex 2068 8 bit behaviour
T2068_P2:  DEFW $02F6		; Timex 2068 port of 2nd Joystick
           DEFB $04		; left
           DEFW T2068_T		; "Timex 2068$"
           DEFW T2068_B		; array pointer of Timex 2068 8 bit behaviour

; any new joystick with bits active low, can be added here

END_P:	   DEFW	0		; end of array

;
; name of joytick - need to print spaces after printing it
;

KEMPSTON_T:
           DEFB AT, 4, 8, "Kempston$"
FULLER_T:  DEFB AT, 4, 8, "Fuller$"
CURSOR_T:  DEFB AT, 4, 8, "Cursor$"
SINCLAIR_T:
           DEFB AT, 4, 8, "Sinclair$"
T2068_T:   DEFB AT, 4, 8, "Timex 2068$"

;
; Bits of select ports behaviour, 7 to 0
; CURSOR bits are read from $EFFE except left from $f7fe
; 0 no action
;

KEMPSTON_B: DEFB A_FIRE, 0, 0, A_FIRE, A_UP,    A_DOWN,  A_LEFT,  A_RIGHT
FULLER_B:   DEFB A_FIRE, 0, 0, 0,      A_RIGHT, A_LEFT,  A_DOWN,  A_UP
CURSOR_B:   DEFB 0,      0, 0, A_DOWN, A_UP,    A_RIGHT, A_LEFT,  A_FIRE
T2068_B:    DEFB 0,      0, 0, A_FIRE, A_RIGHT, A_LEFT,  A_DOWN,  A_UP
SINCLAIR_B: DEFB 0,      0, 0, A_FIRE, A_UP,    A_DOWN,  A_RIGHT, A_LEFT
SINCLAIR_B2:
            DEFB 0,      0, 0, A_LEFT, A_RIGHT, A_DOWN,  A_UP,    A_FIRE

;
; Main text screen
; LINE, COL positions are used to calculate video attribute addresses
;

MAIN_SCREEN: 
        DEFB	AT, 0, 4, "Joystick diagnostics v0.8"
        DEFB    AT, 4, 8, "Left on joystick"
        DEFB	AT, LINE1  , COL, ' ' , ' ', UUP, ' ', ' '
	DEFB    AT, LINE1+2, COL, ULEFT, ' ', UFIRE, ' ', URIGHT
	DEFB	AT, LINE1+4, COL, ' ' , ' ', UDOWN, ' ', ' '
        DEFB    AT, 19, 9, "CAPS+SPACE to quit"
	DEFB	AT, 21, 9, "(c) Rui 2020"
	DEFB	'$'

;
; Starting screen position for hex joystick port and reading
;

JOY_PORT:	
	DEFB	AT, LINE1+2, COL+8, '$'

;
; UDGs
;

UDGs:	
	; up
	DEFB    %00010000
	DEFB    %00111000
	DEFB    %01010100
	DEFB    %10010010
	DEFB	%00010000
	DEFB	%00010000
	DEFB	%00010000
	DEFB	%00010000

	; left
	DEFB	%00000000
	DEFB	%00010000
	DEFB	%00100000
	DEFB	%01000000
	DEFB	%11111111
	DEFB	%01000000
	DEFB	%00100000
	DEFB	%00010000

        ; fire
	DEFB	%00011000
	DEFB	%00111100
	DEFB	%01000010
	DEFB	%10011001
	DEFB	%10011001
	DEFB	%01000010
	DEFB	%00111100
	DEFB	%00011000

        ; right
	DEFB	%00000000
	DEFB	%00001000
	DEFB	%00000100
	DEFB	%00000010
	DEFB	%11111111
	DEFB	%00000010
	DEFB	%00000100
	DEFB	%00001000

        ; down
	DEFB	%00010000
	DEFB	%00010000
	DEFB	%00010000
	DEFB	%00010000
	DEFB	%10010010
	DEFB	%01010100
	DEFB	%00111000
	DEFB	%00010000

; Pasmo uses this directive to USR to this value in the BASIC loader
	END	32768	

