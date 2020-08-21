
;
; zx_circle: demo of Breseham's circle with integers using Z80 ASM
;
; (c) Rui Ribeiro 2020
;
; CIRCLE demo      59 bytes 
; CIRCLE routines 133 bytes
; RND              16 bytes

SEED		EQU $5C76	; seed for random numbers
CL_ALL          EQU $0DAF       ; ROM CLS routine


        ORG     32768

MAIN:
        CALL    $CL_ALL		; clear screen
        CALL    FOR_CIRCLE	; call demo routine - circles made w/ a for cycle

        CALL    $CL_ALL         ; clear screen
        CALL    RND_CIRCLE	; call demo routine - random circles

        ; return to BASIC
	EXX
	LD      HL,$2758	; quirk of ZX ROM ; needed for a successful BASIC return
	EXX
        RET

;
;  FOR_CIRCLE: prints circles with a for routine for radius
;
; INPUTS  : NONE
;
; MODIFIES: AF,E, HL
;
FOR_CIRCLE:

	; circle center
        ; H=CY , L=CX
        ; 
        ; CX,CY = (128,96)

        ;LD      L,128		; center X = 128
        ;LD      H,96           ; center Y = 96
	LD	HL,$6080	; less one byte

        ; for radius=94 ; radius != 0 ; radius=radius-2

        LD      E,94		; radius = 94
FOR_E:
        CALL    CIRCLE

        ; radius = radius - 2 (E=E-2)
	DEC	E		
	DEC	E

        ; radius != 0
        XOR     A		; A=0
        CP      E               ; CP A-E
        JR      NZ,FOR_E	; jump relative IF E not 0

        RET

;
; RND_CIRCLE: generates random centered/sized circles
;
; INPUTS  : NONE
;
; MODIFIES: B,AF, E, HL
;
RND_CIRCLE:

        ; 64 random circles
        LD      B,64			; i = 64
LOOP_Z:
	; save B
        PUSH    BC

        ; random number 0-255 into X center circle point
        CALL    RND
        LD      L,A                     ; CX=A center X

        ; random number 0-127 into Y center circle point
        CALL    RND
        AND     $7F			; limit A to 127
        LD      H,A                     ; CY=A  center Y

        ; random number 0-63 into radius
        CALL    RND
        AND     $3F			; limit A to 63
        LD      E,A			; E = RADIUS

        CALL    CIRCLE

	; restore B for loop counting
        POP     BC

        DJNZ    LOOP_Z		 ; i = i -1 ; while(i < 0)
        RET

;
; CIRCLE : Breseham's circle using integers
;
; INPUT:
;      L=(CX), H=(CY): center of circle
;      E  : radius
;
; MODIFIES: AF, HL',DE',BC'
;
; HL' = ERROR
; B'  = Y
; C'  = X
;

CIRCLE:
        LD      A,E		; A = RADIUS

	EXX

        ; int x = radius;
        LD      C,A             ; C'=E' is X = RADIUS

        ; int error = -radius;

        ; 16 bit HL' NEG
        ; of a known 8-bit positive number in A (RADIUS)
        CPL			; 1's CLP
				; HL' is ERROR
        LD      H,$FF		; H' = negation of 0
        LD      L,A		; HL' = A 1's CPL
        INC     HL		; HL'=1'CPL+1=2's CPL (NEG)
                                ; HL' has now -radius value

        ; int y = 0;
	LD	B,0             ; B' is Y=0

        ; while (x > y)
WHILE_C:
	LD	A,C		; A=C'= X
        CP	B               ; compare X-Y
	EXX
        RET     C		; carry=1, if Y>=X, leave routine

        ;plot8points (cx, cy, x, y);
        CALL    PLOT8

        ; error += y;
	EXX
        LD      E,B             ; E' = B' = Y

	; extend positive number E y
	; int 16-bit DE'
        LD      D,0             ; D'=0 -> DE'=E'

                                ; HL' is error
        ADD     HL,DE		; error=error+y
 
        ; ++y;
	INC	B		; B'=B'+1

        LD      E,B		; could be INC E

        ; error += y;
        ; extend positive number E y
        ; int 16-bit DE'
        ;LD      D,0             ; DE'=E' D' still zero

        ADD     HL,DE           ; error=error+y

        ; if (error >= 0)
        BIT     7,H             ; bit 15 of HL' = 1 if negative
        JR      NZ,WHILE_C	; if negative, return to cicle

        ; --x;
	DEC	C               ; C' is X (C'=C'-1)

        ; error -= x;
        ; error -= x;
        LD      E,C

	; extend positive E' x
	; into 16-bit DE'
        XOR     A                 ; A= 0 and carry = 0
        LD      D,A		  ; D'=0 -> DE'=E'
                                  ; HL' is error
        ;XOR     A                ; carry = 0
        SBC     HL,DE             ; error=error-x
        XOR     A                 ; carry = 0
        SBC     HL,DE             ; error=error-x
				  ; same as HL'=HL'-DE'*2

        JR      WHILE_C           ; jump relative to cycle


;
; PLOT8: PLOT 8 POINTS OF THE CIRCLE
;
; INPUT: (CX),(CY) as center of cirle
;        (X),(Y) as offsets to center
;
PLOT8:
        ; plot4points (cx, cy, x, y);
        CALL    PLOT4

        ; plot4points (cx, cy, y, x);
        CALL    SWAP_X_Y	  ; (X)<->(Y)
        CALL    PLOT4
        CALL    SWAP_X_Y          ; reverts previous invocation
        RET

;
; SWAP_X_Y: swaps contents of (X) and (Y)
;
; MODIFIES: A, BC'
;
SWAP_X_Y:
	EXX
	LD	A,B                ; A=Y (A =B')
	LD	B,C                ; Y=X (B'=C')
	LD	C,A                ; X=A (C'=A )
	EXX
        RET

;
; PLOT4: PLOT 4 POINTS OF THE CIRCLE
;
; INPUT: (CX),(CY) as center of cirle
;        (X),(Y) as offsets to center
;
; MODIFIES: BC, DE, AF
;
PLOT4:
        ; H=CY , L=CX

        ; plot(1, cx + x, cy + y);

	EXX
	LD	A,C             ; A=C'=X
        EXX
        ADD     A,L             ; A=CX+X
        LD      C,A		; PLOT_X C=CX+X
        LD      D,C             ; D = CX+X (backup to use again)

	EXX
	LD	A,B             ; A=B'=Y
	EXX
        ADD     A,H             ; A=CY+Y
        LD      B,A		; PLOT_Y B=CY+Y

        CALL    PLOT		; PLOT PLOT_X,PLOT_Y

        ; plot(1,cx - x, cy + y);
	EXX
	LD	A,C             ; A=C'=X
	EXX
	NEG
	ADD	A,L		; A=CX - X (-X+CX)
	LD	C,A		; PLOT_X C=CX-X

	EXX
        LD      E,A             ; E' = CX-X (backup to use again)
        ;EXX
                                ; reusing B CY+Y from previous call
                                ; as plot saves BC

        ; if (x != 0)	- not doing it before, because we reuse
        ;                calculations
	;EXX
	LD      A,C             ; A=C'=X
	EXX
	OR	A               ; CP 0
        CALL    NZ,PLOT         ; PLOT PLOT_X,PLOT_Y IF Z=0 (A!=0)

        ; plot(1,cx + x, cy - y);
        LD      C,D		; PLOT_X C=getting backup of CX+X

	EXX
	LD	A,B             ; A=B'=Y
	EXX
	LD	D,A		; D not needed now, reused to store B' copy
        NEG
        ADD	A,H             ; A=CY - Y (-Y+CY)
        LD      B,A             ; PLOT_Y B=CY-Y

        ; if (y != 0)    - not using it before, because we reuse
        ;                calculations
        LD      A,D             ; A=D=B'=Y
        OR      A		; CP 0
        CALL    NZ,PLOT		; PLOT PLOT_X,PLOT_Y IF Z=0 (A!=0)

        ; plot(1,cx - x, cy - y);
	EXX
	LD	A,E	        ; A=getting backup of CX-X (E')
	EXX
        LD      C,A             ; PLOT_X C=CX-X

				; reusing B=CY-Y from previous call
				; as plot saves BC

        CALL    PLOT		; PLOT PLOT_X,PLOT_Y

        RET

;
; PLOT: put one pixel at (C,B)
;
; INPUTS:
;           B=Y
;           C=X
;
; MODIFIES: AF
;
; OUTPUTS:
;
;           Carry/C=1 -  Y out of range
;

PLOT:
        ; test limits ; allows partial circles
	; as are jumping forwarding the rom limit test
	; this test is essential for things working smoothly
	; Y is between 0 and 191

        LD      A,191		; ZX Spectrum has 192 vertical 
				; pixel lines
        SUB     B		; invert Y coords, down to bottom.
        RET     C       	; if Y>191, leave

				; we do not test X range
				; has having 256 horizontal lines
                                ; 0-255
				; uses the full range of a byte

	; save registers
        PUSH    HL
        PUSH    DE
        PUSH    BC

	CALL     $22B0   	; HL = screen ADDRESS
				; 7-A (pixel position)

        
        ;CALL    $22EC	        ; replaced by a faster, shorter routine
                                ; that does not handle colours

	; Getting right value/bit/pixel in position
        LD      B,A		; B=7-A
        INC     B               ; B=8-A
        LD      A,$01		; rightmost bit
PLOT_LOOP:
        RRCA			; Rotates A to the right with bit 0 moved to bit 7 
        DJNZ    PLOT_LOOP

        OR      (HL)		; A = screen value in (HL) with bit/pixel added
        LD      (HL),A		; write A in (HL) screen address

	; restore registers
        POP     BC
        POP     DE
        POP     HL
        
        RET

;
; GENERATES A 8-BIT RANDOM NUMBER (A - 00 to FF)
;
; INPUT: NONE (or more correctly (SEED) )
;
; OUTPUT: A=RANDOM 8-BIT VALUE 
;
; MODIFIES: DE
;
RND:
	PUSH	HL
        LD      HL,(SEED)       ; get current seed
        LD      A,R		; get "random" value from R
        LD      E,A
        LD      D,(HL)		; get "random" value from ROM contents
        ADD     HL,DE		; add D=ROM+E=R contents to SEED
        LD      (SEED),HL	; save new SEED
       	ADD     A,L		; Add low L to R
        XOR     H		; XOR it H
        POP     HL
        RET

        END     32768

