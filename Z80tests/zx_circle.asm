
;
; zx_cirle: demo of Breseham's circle using integers using Z80 ASM
;
; (c) Rui Ribeiro 2020
;

SEED		EQU $5C76	; seed for random numbers
; ROM CLS routine
CL_ALL          EQU $0DAF       ; ROM


        ORG     32768

MAIN:
        CALL    $CL_ALL		; clear screen
        CALL    FOR_CIRCLE	; call demo routine - circles made w/ a for cycle

        CALL    $CL_ALL         ; clear screen
        CALL    RND_CIRCLE	; call demo routine - random circles

        ; RETURN TO BASIC
	EXX
	LD      HL,$2758	; quirk of ZX ROM ; needed for a succeful BASIC return
	EXX
        RET

;
;  FOR_CIRCLE: prints circles with a for routine for radius
;
; INPUTS  : NONE
;
; MODIFIES: AF,E
;
FOR_CIRCLE:

	; circle center
        ; (CX),(CY) = (128,96)

        LD      A,128
        LD      (CX),A

        LD      A,96
        LD      (CY),A

        ; for radius=94 ; radius != 0 ; radius=radius-2

        LD      A,94		; radius = 94
FOR_A:
        LD      (RADIUS),A	; store radius in memory
        CALL    CIRCLE

        ; radius = radius - 2
        LD      A,(RADIUS)
        SUB     2

        ; radius != 0
        LD      E,0
        CP      E
        JR      NZ,FOR_A

        RET
;
; RND_CIRCLE: generates random centered/sized circles
;
; INPUTS  : NONE
;
; MODIFIES: B,AF
;
RND_CIRCLE:

        ; 64 random circles
        LD      B,64			; i = 64
LOOP_Z:
	; save B
        PUSH    BC

        ; random number 0-255 into X center circle point
        CALL    RND
        LD      (CX),A

        ; random number 0-127 into Y center circle point
        CALL    RND
        AND     $7F			; limit A to 127
        LD      (CY),A

        ; random number 0-63 into radius
        CALL    RND
        AND     $3F			; limit A to 63
        LD      (RADIUS),A

        CALL    CIRCLE

	; restore B for loop counting
        POP     BC

        DJNZ    LOOP_Z		 ; i = i -1 ; while(i < 0)
        RET

;
; CIRCLE : Breseham's circle using integers
;
; INPUT:
;      (CX), (CY): center of circle
;      (RADIUS)  : radius
;
; MODIFIES: AF, HL',DE',BC'
;
; HL' = ERROR
; B'  = Y
; C'  = X
;

CIRCLE:
        ; int error = -radius;

	; 16 bit NEG
	; of a known positive number (RADIUS)
        LD      A,(RADIUS)
        CPL			; 1's CLP

	EXX
				; HL'=ERROR
        LD      H,$FF		; negation of 0
        LD      L,A		; HL = A 1's CPL
        INC     HL		; 1'CPL+1=2's CPL (NEG)

        ; int x = radius;
        LD      A,(RADIUS)
	LD	C,A             ; C'=X

        ; int y = 0;
        XOR     A
	LD	B,A             ; B'=Y

        ; while (x > y)
WHILE_C:
	LD	A,C		; A=C' - X
        CP	B               ; compare X-Y
	EXX
        RET     C		; carry=1, if Y>=X, leave routine

        ;plot8points (cx, cy, x, y);
        CALL    PLOT8

        ; error += y;
	EXX
        LD      E,B             ; E = B' = Y

	; extend positive number E y
	; int 16-bit DE
        LD      D,0

                                ; HL' is error
        ADD     HL,DE		; error=error+y
  

        ; ++y;
	INC	B

        LD      E,B

        ; error += y;
        ; extend positive number E y
        ; int 16-bit DE
        LD      D,0

        ADD     HL,DE           ; error=error+y

        ; if (error >= 0)
        LD      A,H

        AND     $80             ; bit 15 of ERROR = 1 if negative
        JR      NZ,WHILE_C	; if negative, return to cicle

        ; --x;
	DEC	C               ; C' is X

        ; error -= x;
        ; error -= x;
        LD      E,C


	; extend positive E x
	; into 16-bit DE
        LD      D,0
                                  ; HL' is error
        XOR     A                 ; carry = 0
        SBC     HL,DE             ; error=error-x
        XOR     A                 ; carry = 0
        SBC     HL,DE             ; error=error-x

        JR      WHILE_C           ; jump to cycle

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
; MODIFIES: (X),(Y), A, E
;
SWAP_X_Y:
	EXX
	LD	A,B                ; A=Y (B')
	LD	B,C                ; Y=X (B'=C')
	LD	C,A                ; X=A (C'=A)
	EXX
        RET
;
; PLOT4: PLOT 4 POINTS OF THE CIRCLE
;
; INPUT: (CX),(CY) as center of cirle
;        (X),(Y) as offsets to center
;
; MODIFIES: BC, DE, HL, AF
;
PLOT4:
        LD      HL,(CX)         ; H=CY , L=CX

        ; plot(1, cx + x, cy + y);

	EXX
	LD	A,C             ; A=C'=X
        EXX
        LD      C,A
        LD      A,L             ; LD A,(CX)
        ADD     A,C             ; CX + X
        LD      C,A
        LD      D,C             ; D = CX+X (backup to use again)

	EXX
	LD	A,B             ; A=B'=Y
	EXX
        LD      B,A
        LD      A,H             ; LD A,(CY)
        ADD     A,B             ; CY + Y      
        LD      B,A

        CALL    PLOT

        ; plot(1,cx - x, cy + y);
	EXX
	LD	A,C             ; A=C'=X
	EXX
        LD      C,A
        LD      A,L             ; LD A,(CX)
        SUB     C               ; CX - X
        LD      C,A
        LD      E,C             ; E = CX-X (backup to use again)

                                ; reusing B CY+Y from previous call
                                ; as plot saves BC

        ; if (x != 0)	- not doing it before, because we reuse
        ;                calculations
	EXX
	LD      A,C             ; A=C'=X
	EXX
        CP      0
        CALL    NZ,PLOT

        ; plot(1,cx + x, cy - y);
        LD      C,D		; getting backup of CX+X

	EXX
	LD	A,B             ; A=B'=Y
	EXX
        LD      B,A
        LD      A,H             ; LD A,(CY)
        SUB     B               ; CY - Y
        LD      B,A

        ; if (y != 0)    - not using it before, because we reuse
        ;                calculations
        EXX
        LD      A,B             ; A=B'=Y
        EXX
        CP      0
        CALL    NZ,PLOT

        ; plot(1,cx - x, cy - y);
        LD      C,E             ; getting backup of CX-X

				; reusing B CY-Y from previous call
				; as plot saves BC
        CALL    PLOT

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
;           C=1 -  Y out of range
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

	; Getting right value/bit in position
        LD      B,A
        INC     B
        LD      A,$01
PLOT_LOOP:
        RRCA
        DJNZ    PLOT_LOOP

        OR      (HL)		; A = screen value in (HL) with bit added
        LD      (HL),A		; write A in (HL)

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
; MODIFIES: DE,HL
;
RND:
        LD      HL,(SEED)       ; get current seed
        LD      A,R		; get "random" value from R
        LD      E,A
        LD      D,(HL)		; get "random" value from ROM contents
        ADD     HL,DE		; add D=ROM+E=R contents to SEED
        LD      (SEED),HL	; save new SEED
       	ADD     A,L		; Add low L to R
        XOR     H		; XOR it H
        RET

; center of circle
CX      DB      0
CY      DB      0
; radius
RADIUS  DB      0

        END     32768

