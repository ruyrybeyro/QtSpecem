
;
; zx_circle: demo of Breseham's circle with integers using Z80 ASM
;
; (c) Rui Ribeiro 2020
;
; MIT License
;
; pasmo --tapbas zx_circle.asm zx_circle.tap
;
; CIRCLE routine is invoked with:
;
; Center of circle:
;
; H  = Y
; L  = X
; 
; E  = radius in pixels
;
;
; PLOT routine, used by CIRCLE. Can be directly invoked with:
;
; B=Y, C=X
;
; The coordinate system was changed to use full screen
; (0,0) is really at the bottom.
;
;
; RND routine returns a 8-bit pseudo-random number in A
;
; Sizes in RAM:
;
; CIRCLE calling demos 59 bytes 
;
;        MAIN          18 bytes
;        FOR_CIRCLE    15 bytes
;        RND_CIRCLE    26 bytes
;
; CIRCLE routines      99 bytes in total
;
;        CIRCLE        37 bytes
;        PLOT8          9 bytes
;        SWAP_X_Y       6 bytes
;        PLOT4         47 bytes
;
; PLOT   routine       23 bytes (faster) / 17 bytes (slower)
;
; RND                  16 bytes
;
; based in https://gist.github.com/bert/1085538/f288057c6fb08b61bf97e999d9237f6e04e4f444#file-circle-c

; ZX Spectrum system variable SEED
SEED		EQU $5C76	; seed for random numbers

; ROM routine for clearing the screen
CL_ALL          EQU $0DAF       ; ROM CLS routine


        ORG     32768

;
; MAIN: calls demo routines
;
; size: 18 bytes
;

MAIN:
        CALL    CL_ALL		; clear screen
        CALL    FOR_CIRCLE	; call demo routine - circles made w/ a for cycle

        CALL    CL_ALL          ; clear screen
        CALL    RND_CIRCLE	; call demo routine - random circles

        ; return to BASIC
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'


	LD      HL,$2758	; quirk of ZX ROM ; HL'=$2758 needed for a successful BASIC return
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
        RET			; return to BASIC

;
;  FOR_CIRCLE: prints circles with a for routine for radius
;
; INPUTS  : NONE
;
; MODIFIES: AF,E, HL
;
; size: 15 bytes
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
        CP      E               ; CP A-E - compare radius with 0
        JR      NZ,FOR_E	; jump relative IF E/radius not 0

        RET

;
; RND_CIRCLE: generates random centered/sized circles
;
; INPUTS  : NONE
;
; MODIFIES: B,AF, E, HL
;
; size: 26 bytes
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

        DJNZ    LOOP_Z		 ; i = i -1 ; while(i > 0) 
                                 ; B=B-1; jump to LOOP_Z if B not zero 0
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
; size: 37 bytes
;

CIRCLE:
        LD      A,E		; A = RADIUS

	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'

        ; int x = radius;
        LD      C,A             ; C'=E' is X = RADIUS

        ; int error = -radius;
        ; HL' = -RADIUS (we need a 16 bit number,
        ;              8 bits overflows for larger circles)

	CPL
        LD      L,A		; L'=8 bit, 1 CPL RADIUS

	LD	H,$FF           ; extend 8 bit CPL into 16 bit
	INC	HL		; HL = 16 bit NEG HL = - RADIUS
				; Z80 negative numbers are
				; 2 complement (NEG)

        ; int y = 0;
	XOR	A               ; A=0 (temporary)
	LD	B,A             ; B' is Y=0

        ; used ahead for extending E to 16 bits positive number DE
	LD	D,A             ; D'=0 ; D' will be always 0

        ; while (x > y)
WHILE_C:
	LD	A,C		; A=C'= X
        CP	B               ; compare X-Y
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
        RET     C		; carry=1, if Y>=X
                                ; circle finished, leave routine

        ;plot8points (cx, cy, x, y);
        CALL    PLOT8

        ; error += y;
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
        LD      E,B             ; E' = B' = Y

	; extend positive number E y
	; int 16-bit DE'
        ;LD      D,0             ; D'=0 -> DE'=E'
	; D' always 0, not needed

                                ; HL' is error
        ADD     HL,DE		; error=error+y
 
        ; ++y;
	INC	B		; B'=B'+1

        INC	E 		; E was still B' 
                                ; (not DE on purpose. D' will always be 0)

        ; error += y;
        ; extend positive number E y
        ; int 16-bit DE'

        ADD     HL,DE           ; error=error+y

        ; if (error >= 0)
        BIT     7,H             ; bit 15 of HL' = 1 if negative
        JR      NZ,WHILE_C	; if negative, return to cicle

        ; --x;
	DEC	C               ; C' is X (C'=C'-1)

        ; error -= x;
        LD      E,C

	; extend positive E' x
	; into 16-bit DE'
        XOR     A                 ; carry = 0
        ;LD      D,A		  ; D'=0 -> DE'=E' 
                                  ; D' always 0

                                  ; HL' is error

        SBC     HL,DE             ; error=error-x

        ; error -= x;
        ;XOR     A                ; carry = 0 
                                  ; carry=1 not happening.
                                  ; we are extending an 8 bit number already.

        SBC     HL,DE             ; error=error-x
				  ; same as HL'=HL'-DE'*2

        JR      WHILE_C           ; jump relative to cycle


;
; PLOT8: PLOT 8 POINTS OF THE CIRCLE
;
; INPUT: (CX),(CY) as center of cirle
;        (X),(Y) as offsets to center
;
; size: 9 bytes
;
PLOT8:
        ; plot4points (cx, cy, x, y);
        CALL    PLOT4

        ; plot4points (cx, cy, y, x);
        CALL    SWAP_X_Y	  ; (X)<->(Y)
        CALL    PLOT4
; falls through
;        CALL    SWAP_X_Y          ; reverts previous invocation 
;        RET

;
; SWAP_X_Y: swaps contents of (X) and (Y)
;
; MODIFIES: A, BC'
;
; size: 6 bytes
;
SWAP_X_Y:
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
	LD	A,B             ; A=Y (A =B')
	LD	B,C             ; Y=X (B'=C')
	LD	C,A             ; X=A (C'=A )
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
        RET


;
; PLOT4: PLOT 4 POINTS OF THE CIRCLE
;
; INPUT: (CX),(CY) as center of cirle
;        (X),(Y) as offsets to center
;
; MODIFIES: BC, DE, AF
;
; size: 47 bytes 
;
PLOT4:
        ; H=CY , L=CX

        ; plot(1, cx + x, cy + y);

	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
	LD	A,C             ; A=C'=X
        EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
        ADD     A,L             ; A=CX+X
        LD      C,A		; PLOT_X C=CX+X
        LD      D,C             ; D = CX+X (backup to use again)

	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
	LD	A,B             ; A=B'=Y
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
        ADD     A,H             ; A=CY+Y
        LD      B,A		; PLOT_Y B=CY+Y

        CALL    PLOT		; PLOT PLOT_X,PLOT_Y

        ; plot(1,cx - x, cy + y);
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
	LD	A,C             ; A=C'=X
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
	NEG			; A=-C'=-X
	ADD	A,L		; A=CX - X (-X+CX)
	LD	C,A		; PLOT_X C=CX-X

	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
        LD      E,A             ; E' = CX-X (backup to use again)
        ;EXX
                                ; reusing B CY+Y from previous call
                                ; as plot saves BC

        ; if (x != 0)	- not doing it before, because we reuse
        ;                calculations
	;EXX
	LD      A,C             ; A=C'=X
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
	OR	A               ; CP 0 ; A compared to zero
        CALL    NZ,PLOT         ; PLOT PLOT_X,PLOT_Y IF Z=0 (X!=0)

        ; plot(1,cx + x, cy - y);
        LD      C,D		; PLOT_X C=getting backup of CX+X

	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
	LD	A,B             ; A=B'=Y
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
	LD	D,A		; D not needed now, reused to store B' copy
        NEG                     ; A=-B'=-Y
        ADD	A,H             ; A=CY - Y (-Y+CY)
        LD      B,A             ; PLOT_Y B=CY-Y

        ; if (y != 0)    - not using it before, because we reuse
        ;                calculations
        LD      A,D             ; A=D=B'=Y
        OR      A		; CP 0  ; A compared to zero
        CALL    NZ,PLOT		; PLOT PLOT_X,PLOT_Y IF Z=0 (Y!=0)

        ; plot(1,cx - x, cy - y);
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
	LD	A,E	        ; A=getting backup of CX-X (E')
	EXX                     ; exchanges BC, DE, and HL with shadow registers BC', DE', and HL'
        LD      C,A             ; PLOT_X C=CX-X

				; reusing B=CY-Y from previous call
				; as plot saves BC

; falls through
;        CALL    PLOT		; PLOT PLOT_X,PLOT_Y
;        RET

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
; size: 23 bytes
;

PLOT:
        ; test limits ; allows partial circles
	; as we are jumping bypassing the rom limit test
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

	CALL     $22B0   	; INPUT: B=Y,C=X
                                ; returns:
                                ; HL = screen ADDRESS
				; 7-A (pixel position)

        
        ;CALL    $22EC	        ; replaced by faster, shorter routine commentend
                                ; that does not handle colours (+6 bytes)

	; Getting right value/bit/pixel in position
        LD      B,A		; B=7-A
        INC     B               ; B=8-A
        LD      A,$01		; rightmost bit
PLOT_LOOP:
        RRCA			; Rotates A to the right with bit 0 moved to bit 7 
        DJNZ    PLOT_LOOP	; Loop until B=0 - 1 is in the right bit for plotting X

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
; size: 16 bytes
;
RND:
	PUSH	HL		; save HL
        LD      HL,(SEED)       ; get current seed
        LD      A,R		; get "random" value from R
        LD      D,A		; D = R
        LD      E,(HL)		; get "random" value from ROM contents
        ADD     HL,DE		; add E=ROM,D=R contents to SEED
        LD      (SEED),HL	; save new SEED
       	ADD     A,L		; A= R + L
        XOR     H		; A= R + L ^ H
        POP     HL              ; restore HL
        RET

        END     32768

