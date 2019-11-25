; Turbo loader routing, adapted from the official
; ROM routines (Rui Ribeiro)
;
; LD0-LD9 timing of load routines
; SA0-SA9 timing of save routines
;
; RANDOMIZE USR 50000 - SCREEN LOAD ROUTINE
; RANDOMIZE USR 50001 - SCREEN SAVE ROUTING

; ------------------------------------
; Load header or block of information
; ------------------------------------
;   This routine is used to load bytes and on entry A is set to $00 for a 
;   header or to $FF for data.  IX points to the start of receiving location 
;   and DE holds the length of bytes to be loaded. 

	ORG	50000
        NOP
	LD	IX,16384	; START OF SCREEN ADDRESS
	LD	DE,6912		; LENGHT
	LD	A,$FF		; DATA BLOCK
	BIT	0,C
	JR	Z,LD_ENTRY	; IF 50000 (ODD), LOAD
        JP      SA_BYTES	; ELSE SAVE

LD_ENTRY:
        SCF

LD_BYTES:
        INC     D               ; reset the zero flag without disturbing carry.
        EX      AF,AF'          ; preserve entry flags.
        DEC     D               ; restore high byte of length.

        DI                      ; disable interrupts

;       LD      A,$0F          ; make the border white and mic off.
;       OUT     ($FE),A         ; output to port.

;       LD      HL,$053F        ; Address: SA/LD_RET
;       PUSH    HL              ; is saved on stack as terminating routine.

;   the reading of the EAR bit (D6) will always be preceded by a test of the 
;   space key (D0), so store the initial post-test state.

        IN      A,($FE)         ; read the ear state - bit 6.
        RRA                     ; rotate to bit 5.
        AND     $20             ; isolate this bit.
;       OR      $02             ; combine with red border colour.
	OR	$03
        LD      C,A             ; and store initial state long-term in C.
        CP      A               ; set the zero flag.

; 

LD_BREAK:
        RET     NZ              ; return if at any time space is pressed.

LD_START:
        CALL    LD_EDGE_1       ; routine LD_EDGE-1
;       JR      NC,LD_BREAK     ; back to LD_BREAK with time out and no
                                ; edge present on tape.
				; with aprox 14000T

;   but continue when a transition is found on tape.

LD0:    LD      HL,$0415        ; set up 16-bit outer loop counter for 
                                ; approx 1 second delay.
                                ; $0415

LD_WAIT:
        DJNZ    LD_WAIT         ; self loop to LD_WAIT (for 256 times)

        DEC     HL              ; decrease outer loop counter.
        LD      A,H             ; test for
        OR      L               ; zero.
        JR      NZ,LD_WAIT      ; back to LD_WAIT, if not zero, with zero in B.

;   continue after delay with H holding zero and B also.
;   sample 256 edges to check that we are in the middle of a lead-in section. 

        CALL    LD_EDGE_2       ; routine LD_EDGE-2
        JR      NC,LD_BREAK     ; back to LD_BREAK
                                ; if no edges at all.

LD_LEADER:
LD1:    LD      B,$4E           ; set timing value. $9C
        CALL    LD_EDGE_2       ; routine LD_EDGE-2
        JR      NC,LD_BREAK     ; back to LD_BREAK if time-out

LD2:    LD      A,$63           ; two edges must be spaced apart. $C6
        CP      B               ; compare
        JR      NC,LD_START     ; back to LD_START if too close together for a 
                                ; lead-in.

        INC     H               ; proceed to test 256 edged sample.
        JR      NZ,LD_LEADER    ; back to LD_LEADER while more to do.

;   sample indicates we are in the middle of a two or five second lead-in.
;   Now test every edge looking for the terminal sync signal.

LD_SYNC:
LD3:    LD      B,$65           ; initial timing value in B. $C9
        CALL    LD_EDGE_1       ; routine LD_EDGE-1
        JR      NC,LD_BREAK     ; back to LD_BREAK with time-out.

        LD      A,B             ; fetch augmented timing value from B.
LD4:    CP      $6A             ; compare $D4 
        JR      NC,LD_SYNC      ; back to LD_SYNC if gap too big, that is,
                                ; a normal lead-in edge gap.

;   but a short gap will be the sync pulse.
;   in which case another edge should appear before B rises to $FF

        CALL    LD_EDGE_1       ; routine LD_EDGE-1
;       RET     NC              ; return with time-out.

; proceed when the sync at the end of the lead-in is found.
; We are about to load data so change the border colours.

        LD      A,C             ; fetch long-term mask from C
;       XOR     $03             ; and make blue/yellow.
        XOR     $02

        LD      C,A             ; store the new long-term byte.

        LD      H,$00           ; set up parity byte as zero.
LD5:    LD      B,$58           ; timing. Non-turbo $B0
        JR      LD_MARKER       ; forward to LD_MARKER 
                                ; the loop mid entry point with the alternate 
                                ; zero flag reset to indicate first byte 
                                ; is discarded.

; --------------
;   the loading loop loads each byte and is entered at the mid point.

LD_LOOP:
        EX      AF,AF'          ; restore entry flags and type in A.
        JR      NZ,LD_FLAG      ; forward to LD_FLAG if awaiting initial flag
                                ; which is to be discarded.

;       JR      NC,LD_VERIFY    ; forward to LD_VERIFY if not to be loaded.

        LD      (IX+$00),L      ; place loaded byte at memory location.
        JR      LD_NEXT         ; forward to LD_NEXT

; ---

LD_FLAG:
        RL      C               ; preserve carry (verify) flag in long-term
                                ; state byte. Bit 7 can be lost.

        XOR     L               ; compare type in A with first byte in L.
        RET     NZ              ; return if no match e.g. CODE vs. DATA.

;   continue when data type matches.

        LD      A,C             ; fetch byte with stored carry
        RRA                     ; rotate it to carry flag again
        LD      C,A             ; restore long-term port state.

        INC     DE              ; increment length ??
        JR      LD_DEC          ; forward to LD_DEC.
                                ; but why not to location after ?

; ---
;   for verification the byte read from tape is compared with that in memory.

;LD_VERIFY:
;       LD      A,(IX+$00)      ; fetch byte from memory.
;       XOR     L               ; compare with that on tape
;       RET     NZ              ; return if not zero. 

LD_NEXT:
        INC     IX              ; increment byte pointer.

LD_DEC:
        DEC     DE              ; decrement length.
        EX      AF,AF'          ; store the flags.
LD6:    LD      B,$59           ; timing. Non-turbo: $B2

;   when starting to read 8 bits the receiving byte is marked with bit at right.
;   when this is rotated out again then 8 bits have been read.

LD_MARKER:
        LD      L,$01           ; initialize as %00000001

LD_8_BITS:
        CALL    LD_EDGE_2       ; routine LD_EDGE-2 increments B relative to
                                ; gap between 2 edges.
;       RET     NC              ; return with time-out.

LD7:    LD      A,$66           ; the comparison byte. Non-turbo: $CB
        CP      B               ; compare to incremented value of B.
                                ; if B is higher then bit on tape was set.
                                ; if <= then bit on tape is reset. 

        RL      L               ; rotate the carry bit into L.

LD8:    LD      B,$58           ; reset the B timer byte. Non-turbo: $B0
        JP      NC,LD_8_BITS    ; JUMP back to LD_8-BITS

;   when carry set then marker bit has been passed out and byte is complete.

        LD      A,H             ; fetch the running parity byte.
        XOR     L               ; include the new byte.
        LD      H,A             ; and store back in parity register.

        LD      A,D             ; check length of
        OR      E               ; expected bytes.
        JR      NZ,LD_LOOP      ; back to LD_LOOP 
                                ; while there are more.

;   when all bytes loaded then parity byte should be zero.

        LD      A,H             ; fetch parity byte.
        CP      $01             ; set carry if zero.
        EI			; NEW
        RET                     ; return
                                ; in no carry then error as checksum disagrees.
; -------------------------
; Check signal being loaded
; -------------------------
;   An edge is a transition from one mic state to another.
;   More specifically a change in bit 6 of value input from port $FE.
;   Graphically it is a change of border colour, say, blue to yellow.
;   The first entry point looks for two adjacent edges. The second entry point
;   is used to find a single edge.
;   The B register holds a count, up to 256, within which the edge (or edges) 
;   must be found. The gap between two edges will be more for a '1' than a '0'
;   so the value of B denotes the state of the bit (two edges) read from tape.

; ->

LD_EDGE_2:
        CALL    LD_EDGE_1       ; call routine LD-EDGE-1 below.
        RET     NC              ; return if space pressed or time-out.
                                ; else continue and look for another adjacent 
                                ; edge which together represent a bit on the 
                                ; tape.

; -> 
;   this entry point is used to find a single edge from above but also 
;   when detecting a read-in signal on the tape.

LD_EDGE_1:
LD9:	LD      A,$0B           ; a delay value of twenty two.

				; $16 - Wait 358 T states before entering 
				; the sampling loop.
LD_DELAY:  
	DEC     A               ; decrement counter
        JR      NZ,LD_DELAY     ; loop back to LD-DELAY 22 times.

        AND      A              ; clear carry.

LD_SAMPLE:  
	INC     B               ; increment the time-out counter.
        RET     Z               ; return with failure when $FF passed.

        LD      A,$7F           ; prepare to read keyboard and EAR port
        IN      A,($FE)         ; row $7FFE. bit 6 is EAR, bit 0 is SPACE key.
        RRA                     ; test outer key the space. (bit 6 moves to 5)
;       RET     NC              ; return if space pressed.  >>>

        XOR     C               ; compare with initial long-term state.
        AND     $20             ; isolate bit 5
        JR      Z,LD_SAMPLE     ; back to LD-SAMPLE if no edge.

;   but an edge, a transition of the EAR bit, has been found so switch the
;   long-term comparison byte containing both border colour and EAR bit. 

        LD      A,C             ; fetch comparison value.
;       CPL                     ; switch the bits
	XOR	$FB
        LD      C,A             ; and put back in C for long-term.

;	LD	A,IXL		; multicolor
;	db $DD, $7D  		; makes difficult to debug
;	XOR	L		; tape loading problems

        AND     $07             ; isolate new colour bits.
        OR      $08             ; set bit 3 - MIC off.
        OUT     ($FE),A         ; send to port to effect the change of colour. 

        SCF                     ; set carry flag signaling edge found within
                                ; time allowed.
        RET                     ; return.

;	Turbo saving routine

SA_BYTES:
        LD      HL,$053F        ; address: SA/LD_RET
        PUSH    HL              ; is pushed as common exit route.
                                ; however there is only one non-terminal exit 
                                ; point.

SA0:    LD      HL,$0FC0        ; a timing constant H=$1F, L=$80
                                ; inner and outer loop counters
                                ; a five second lead-in is used for a header.

        BIT     7,A             ; test one bit of accumulator.
                                ; (AND A ?)
        JR      Z,SA_FLAG       ; skip to SA-FLAG if a header is being saved.

;   else is data bytes and a shorter lead-in is used.

SA1:    LD      HL,$064C        ; another timing value H=$0C, L=$98.
                                ; a two second lead-in is used for the data.


SA_FLAG:
        EX      AF,AF'          ; save flag
        INC     DE              ; increase length by one.
        DEC     IX              ; decrease start.

        DI                      ; disable interrupts

        LD      A,$02           ; select red for border, microphone bit on.
        LD      B,A             ; also does as an initial slight counter value.

SA_LEADER:
        DJNZ    SA_LEADER       ; self loop to SA-LEADER for delay.
                                ; after initial loop, count is $A4 (or $A3)

        OUT     ($FE),A         ; output byte $02/$0D to tape port.

        XOR     $0F             ; switch from RED (mic on) to CYAN (mic off).

SA3:    LD      B,$52           ; hold count. also timed instruction. $A4

        DEC     L               ; originally $80 or $98.
                                ; but subsequently cycles 256 times.
        JR      NZ,SA_LEADER    ; back to SA-LEADER until L is zero.

;   the outer loop is counted by H

        DEC     B               ; decrement count
        DEC     H               ; originally  twelve or thirty-one.
        JP      P,SA_LEADER     ; back to SA-LEADER until H becomes $FF

;   now send a sync pulse. At this stage mic is off and A holds value
;   for mic on.
;   A sync pulse is much shorter than the steady pulses of the lead-in.

SA4:    LD      B,$18           ; another short timed delay. $2F

SA_SYNC_1:
        DJNZ    SA_SYNC_1       ; self loop to SA-SYNC-1
				; MIC off for 667 T states from 'OUT to OUT'.

        OUT     ($FE),A         ; switch to mic on and red.
        LD      A,$0D           ; prepare mic off - cyan
SA5:    LD      B,$1C           ; another short timed delay. Non-turbo: $37

SA_SYNC_2:
        DJNZ    SA_SYNC_2       ; self loop to SA-SYNC-2
				; MIC on for 735 T States from 'OUT to OUT'.

        OUT     ($FE),A         ; output mic off, cyan border.
SA6:    LD      BC,$1E0E        ; B=$3B time(*), C=$0E, YELLOW, MIC OFF. $3B

; 

        EX      AF,AF'          ; restore saved flag
                                ; which is 1st byte to be saved.

        LD      L,A             ; and transfer to L.
                                ; the initial parity is A, $FF or $00.
        JP      SA_START        ; JUMP forward to SA-START     ->
                                ; the mid entry point of loop.

; -------------------------
;   During the save loop a parity byte is maintained in H.
;   the save loop begins by testing if reduced length is zero and if so
;   the final parity byte is saved reducing count to $FFFF.

SA_LOOP:
        LD      A,D             ; fetch high byte
        OR      E               ; test against low byte.
        JR      Z,SA_PARITY     ; forward to SA-PARITY if zero.

        LD      L,(IX+$00)      ; load currently addressed byte to L.

SA_LOOP_P:
        LD      A,H             ; fetch parity byte.
        XOR     L               ; exclusive or with new byte.

; -> the mid entry point of loop.

SA_START:
        LD      H,A             ; put parity byte in H.
        LD      A,$01           ; prepare blue, mic=on.
        SCF                     ; set carry flag ready to rotate in.
        JP      SA_8_BITS       ; JUMP forward to SA-8-BITS            -8->

; ---

SA_PARITY:
        LD      L,H             ; transfer the running parity byte to L and
        JR      SA_LOOP_P       ; back to SA-LOOP-P 
                                ; to output that byte before quitting normally.

; ---

;   The entry point to save yellow part of bit.
;   A bit consists of a period with mic on and blue border followed by 
;   a period of mic off with yellow border. 
;   Note. since the DJNZ instruction does not affect flags, the zero flag is 
;   used to indicate which of the two passes is in effect and the carry 
;   maintains the state of the bit to be saved.

SA_BIT_2:
        LD      A,C             ; fetch 'mic on and yellow' which is 
                                ; held permanently in C.
        BIT     7,B             ; set the zero flag. B holds $3E.

;   The entry point to save 1 entire bit. For first bit B holds $3B(*).
;   Carry is set if saved bit is 1. zero is reset NZ on entry.

SA_BIT_1:
        DJNZ    SA_BIT_1        ; self loop for delay to SA-BIT-1

        JR      NC,SA_OUT       ; forward to SA-OUT if bit is 0.

;   but if bit is 1 then the mic state is held for longer.

SA7:    LD      B,$21           ; set timed delay. (66 decimal) 
			        ; Non-turbo: $42
				; However if saving a '1' then add 855 T states.

SA_SET:
        DJNZ    SA_SET          ; self loop to SA-SET 
                                ; (roughly an extra 66*13 clock cycles)

SA_OUT:
        OUT     ($FE),A         ; blue and mic on OR  yellow and mic off.

SA8:    LD      B,$1F           ; set up delay non-turbo: $3E
        JR      NZ,SA_BIT_2     ; back to SA-BIT-2 if zero reset NZ (first pass)

;   proceed when the blue and yellow bands have been output.

        DEC     B               ; change value $3E to $3D.
        XOR     A               ; clear carry flag (ready to rotate in).
        INC     A               ; reset zero flag i.e. NZ.

; -8-> 

SA_8_BITS:
        RL      L               ; rotate left through carry
                                ; C<76543210<C  
        JP      NZ,SA_BIT_1     ; JUMP back to SA-BIT-1 
                                ; until all 8 bits done.

;   when the initial set carry is passed out again then a byte is complete.

        DEC     DE              ; decrease length
        INC     IX              ; increase byte pointer
SA9:    LD      B,$1F           ; set up timing. no-turbo: $31

        LD      A,$7F           ; test the space key and
        IN      A,($FE)         ; return to common exit (to restore border)
        RRA                     ; if a space is pressed
        RET     NC              ; return to SA/LD_RET.   - - >

;   now test if byte counter has reached $FFFF.

        LD      A,D             ; fetch high byte
        INC     A               ; increment.
        JP      NZ,SA_LOOP      ; JUMP to SA-LOOP if more bytes.

;SA10:  LD      B,$3B           ; a final delay. $3B 

;SA_DELAY:
;       DJNZ    SA_DELAY        ; self loop to SA-DELAY
        RET                     ; return - - >


;
; T times calculation (not necessarily 100% exact) 
; for SA-BYTES and LD-BYTES
; (study of those routines for writing the turbo routines)
;
;6675672T LEADER 2132T OFF / ON
;667T off SYNC1		1>0>SYNC2<SYNC1
;735T on  SYNC2
;855T off + 855T on for 0
;855T * 2 off + 855T * 2 on for 1 (HEADER + DATA + CHECKSUM)
;762T off
;
;; =========================================
;;
;; PORT 254 ($FE)
;;
;;                      spk mic { border  }  
;;          ___ ___ ___ ___ ___ ___ ___ ___ 
;; PORT    |   |   |   |   |   |   |   |   |
;; 254     |   |   |   |   |   |   |   |   |
;; $FE     |___|___|___|___|___|___|___|___|
;;           7   6   5   4   3   2   1   0
;;
;
;; ----------------------------------
;; Save header and program/data bytes
;; ----------------------------------
;;   This routine saves a section of data. It is called from SA-CTRL to save the
;;   seventeen bytes of header data. It is also the exit route from that routine
;;   when it is set up to save the actual data.
;;   On entry -
;;   HL points to start of data.
;;   IX points to descriptor.
;;   The accumulator is set to  $00 for a header, $FF for data.
;
;;; SA-BYTES
;L04C2:  LD      HL,L053F        10T; address: SA/LD-RET
;        PUSH    HL              11T; is pushed as common exit route.
;                                ; however there is only one non-terminal exit 
;                                ; point.
;
;        LD      HL,$1F80        10T; a timing constant H=$1F, L=$80
;                                ; inner and outer loop counters
;                                ; a five second lead-in is used for a header.
;
;        BIT     7,A             8; test one bit of accumulator.
;                                ; (AND A ?)
;        JR      Z,L04D0         12/7; skip to SA-FLAG if a header is being saved.
;
;;   else is data bytes and a shorter lead-in is used.
;
;        LD      HL,$0C98        10; another timing value H=$0C, L=$98.
;                                ; a two second lead-in is used for the data.
;
;
;;; SA-FLAG
;L04D0:  EX      AF,AF'          4; save flag
;        INC     DE              6; increase length by one.
;        DEC     IX              10; decrease start.
;
;        DI                      4; disable interrupts
;
;        LD      A,$02           7; select red for border, microphone bit on.
;        LD      B,A             4; also does as an initial slight counter value.
;
;;; SA-LEADER
;L04D8:  DJNZ    L04D8           13/8; self loop to SA-LEADER for delay.
;                                ; after initial loop, count is $A4 (or $A3)
;                                ; 13 + 8T
;                                ; $A4*13-5 T
;                                ; $A3*13-5 T
;
;        OUT     ($FE),A         11; output byte $02/$0D to tape port.
;
;        XOR     $0F             7; switch from RED (mic on) to CYAN (mic off).
;
;        LD      B,$A4           7; hold count. also timed instruction.
;
;        DEC     L               4; originally $80 or $98.
;                                ; but subsequently cycles 256 times.
;        JR      NZ,L04D8        12/7; back to SA-LEADER until L is zero.
;
;;   the outer loop is counted by H
;
;        DEC     B               4; decrement count
;        DEC     H               4; originally  twelve or thirty-one.
;        JP      P,L04D8         10; back to SA-LEADER until H becomes $FF
;
;; A4 0xFF 0c
;((((164*13)+11+7+7+4+12)*256)+4+4+10)*12 = 3 963 768 == 6675672 = 1.9s
;
;;   now send a sync pulse. At this stage mic is off and A holds value
;;   for mic on.
;;   A sync pulse is much shorter than the steady pulses of the lead-in.
;
;        LD      B,$2F           7; another short timed delay.
;
;;; SA-SYNC-1
;L04EA:  DJNZ    L04EA           13/8; self loop to SA-SYNC-1
;
;	0x2F
;	47*13-5+7+11 T = 624T  
;
;        OUT     ($FE),A         11; switch to mic on and red.
;        LD      A,$0D           7; prepare mic off - cyan
;        LD      B,$37           7; another short timed delay.
;
;;; SA-SYNC-2
;L04F2:  DJNZ    L04F2           13/8; self loop to SA-SYNC-2
;				$37 * 13 -5 T
; 
;	; 7 + 7 + $37 * 13 -5 = 724T
;
;
;        OUT     ($FE),A         11; output mic off, cyan border.
;        LD      BC,$3B0E        10; B=$3B time(*), C=$0E, YELLOW, MIC OFF.
;
;; 
;
;        EX      AF,AF'          4; restore saved flag
;                                ; which is 1st byte to be saved.
;
;        LD      L,A             4; and transfer to L.
;                                ; the initial parity is A, $FF or $00.
;        JP      L0507           10; JUMP forward to SA-START     ->
;                                ; the mid entry point of loop.
;
;; -------------------------
;;   During the save loop a parity byte is maintained in H.
;;   the save loop begins by testing if reduced length is zero and if so
;;   the final parity byte is saved reducing count to $FFFF.
;
;;; SA-LOOP
;L04FE:  LD      A,D             4; fetch high byte
;        OR      E               4; test against low byte.
;        JR      Z,L050E         12/7; forward to SA-PARITY if zero.
;
;        LD      L,(IX+$00)      19; load currently addressed byte to L.
;
;;; SA-LOOP-P
;L0505:  LD      A,H             4; fetch parity byte.
;        XOR     L               4; exclusive or with new byte.
;
;; -> the mid entry point of loop.
;
;;; SA-START
;L0507:  LD      H,A             4; put parity byte in H.
;        LD      A,$01           7; prepare blue, mic=on.
;        SCF                     4; set carry flag ready to rotate in.
;        JP      L0525           10; JUMP forward to SA-8-BITS            -8->
;
;; ---
;
;;; SA-PARITY
;L050E:  LD      L,H             4; transfer the running parity byte to L and
;        JR      L0505           12; back to SA-LOOP-P 
;                                ; to output that byte before quitting normally.
;
;; ---
;
;;   The entry point to save yellow part of bit.
;;   A bit consists of a period with mic on and blue border followed by 
;;   a period of mic off with yellow border. 
;;   Note. since the DJNZ instruction does not affect flags, the zero flag is 
;;   used to indicate which of the two passes is in effect and the carry 
;;   maintains the state of the bit to be saved.
;
;;; SA-BIT-2
;L0511:  LD      A,C             4; fetch 'mic on and yellow' which is 
;                                ; held permanently in C.
;        BIT     7,B             8; set the zero flag. B holds $3E.
;
;;   The entry point to save 1 entire bit. For first bit B holds $3B(*).
;;   Carry is set if saved bit is 1. zero is reset NZ on entry.
;
;;; SA-BIT-1
;L0514:  DJNZ    L0514           13/8; self loop for delay to SA-BIT-1
;                                $3E*13-5 T = 801T
;                                $3B*13-5 T
;
;        JR      NC,L051C        12/7; forward to SA-OUT if bit is 0.
;
;;   but if bit is 1 then the mic state is held for longer.
;
;        LD      B,$42           7; set timed delay. (66 decimal)
;
;;; SA-SET
;L051A:  DJNZ    L051A           13/8; self loop to SA-SET 
;                                ; (roughly an extra 66*13 clock cycles)
;                                ; $42*13-5 T = 853T
;
;;; SA-OUT
;L051C:  OUT     ($FE),A         11; blue and mic on OR  yellow and mic off.
;
;        LD      B,$3E           7; set up delay
;        JR      NZ,L0511        12/7; back to SA-BIT-2 if zero reset NZ (first pass)
;
;;   proceed when the blue and yellow bands have been output.
;
;        DEC     B               4; change value $3E to $3D.
;        XOR     A               4; clear carry flag (ready to rotate in).
;        INC     A               4; reset zero flag i.e. NZ.
;
;; -8-> 
;
;;; SA-8-BITS
;L0525:  RL      L               8; rotate left through carry
;                                ; C<76543210<C  
;        JP      NZ,L0514        12/7; JUMP back to SA-BIT-1 
;                                ; until all 8 bits done.
;
;;   when the initial set carry is passed out again then a byte is complete.
;
;        DEC     DE              6; decrease length
;        INC     IX              10; increase byte pointer
;        LD      B,$31           7; set up timing.
;
;        LD      A,$7F           7; test the space key and
;        IN      A,($FE)         11; return to common exit (to restore border)
;        RRA                     4; if a space is pressed
;        RET     NC              11/5; return to SA/LD-RET.   - - >
;
;;   now test if byte counter has reached $FFFF.
;
;        LD      A,D             4; fetch high byte
;        INC     A               4; increment.
;        JP      NZ,L04FE        12/7; JUMP to SA-LOOP if more bytes.
;
;        LD      B,$3B           7; a final delay. 
;
;;; SA-DELAY
;L053C:  DJNZ    L053C           13/8; self loop to SA-DELAY $3B*13-5 T
;
;				762T
;        RET                     10; return - - >
;;;				179T + 762T silencio
;
;
;; ------------------------------------
;; Load header or block of information
;; ------------------------------------
;;   This routine is used to load bytes and on entry A is set to $00 for a 
;;   header or to $FF for data.  IX points to the start of receiving location 
;;   and DE holds the length of bytes to be loaded. If, on entry the carry flag 
;;   is set then data is loaded, if reset then it is verified.
;
;;; LD-BYTES
;L0556:  INC     D               4; reset the zero flag without disturbing carry.
;        EX      AF,AF'          4; preserve entry flags.
;        DEC     D               4; restore high byte of length.
;
;        DI                      4; disable interrupts
;
;        LD      A,$0F           7; make the border white and mic off.
;        OUT     ($FE),A         11; output to port.
;
;        LD      HL,L053F        10; Address: SA/LD-RET
;        PUSH    HL              11; is saved on stack as terminating routine.
;
;;   the reading of the EAR bit (D6) will always be preceded by a test of the 
;;   space key (D0), so store the initial post-test state.
;
;        IN      A,($FE)         11; read the ear state - bit 6.
;        RRA                     4; rotate to bit 5.
;        AND     $20             7; isolate this bit.
;        OR      $02             7; combine with red border colour.
;        LD      C,A             4; and store initial state long-term in C.
;        CP      A               4; set the zero flag.
;
;; 
;
;;; LD-BREAK
;L056B:  RET     NZ              11/5; return if at any time space is pressed.
;
;;; LD-START
;L056C:  CALL    L05E7           10; routine LD-EDGE-1
;        JR      NC,L056B        12/7; back to LD-BREAK with time out and no
;                                ; edge present on tape.
;
;;   but continue when a transition is found on tape.
;
;        LD      HL,$0415        10; set up 16-bit outer loop counter for 
;                                ; approx 1 second delay.
;
;;; LD-WAIT
;L0574:  DJNZ    L0574           13/8; self loop to LD-WAIT (for 256 times)
;
;        DEC     HL              6; decrease outer loop counter.
;        LD      A,H             4; test for
;        OR      L               4; zero.
;        JR      NZ,L0574        12/7; back to LD-WAIT, if not zero, with zero in B.
;
;;   continue after delay with H holding zero and B also.
;;   sample 256 edges to check that we are in the middle of a lead-in section. 
;
;        CALL    L05E3           10; routine LD-EDGE-2
;        JR      NC,L056B        12/7; back to LD-BREAK
;                                ; if no edges at all.
;
;;; LD-LEADER
;L0580:  LD      B,$9C           7; set timing value.
;        CALL    L05E3           10; routine LD-EDGE-2
;        JR      NC,L056B        12/7; back to LD-BREAK if time-out
;
;        LD      A,$C6           4; two edges must be spaced apart.
;        CP      B               4; compare
;        JR      NC,L056C        12/7; back to LD-START if too close together for a 
;                                ; lead-in.
;
;        INC     H               4; proceed to test 256 edged sample.
;        JR      NZ,L0580        12/7; back to LD-LEADER while more to do.
;
;;   sample indicates we are in the middle of a two or five second lead-in.
;;   Now test every edge looking for the terminal sync signal.
;
;;; LD-SYNC
;L058F:  LD      B,$C9           7; initial timing value in B.
;        CALL    L05E7           10; routine LD-EDGE-1
;        JR      NC,L056B        12/7; back to LD-BREAK with time-out.
;
;        LD      A,B             4; fetch augmented timing value from B.
;        CP      $D4             7; compare 
;        JR      NC,L058F        12/7; back to LD-SYNC if gap too big, that is,
;                                ; a normal lead-in edge gap.
;
;;   but a short gap will be the sync pulse.
;;   in which case another edge should appear before B rises to $FF
;
;        CALL    L05E7           10; routine LD-EDGE-1
;        RET     NC              11/5; return with time-out.
;
;; proceed when the sync at the end of the lead-in is found.
;; We are about to load data so change the border colours.
;
;        LD      A,C             4; fetch long-term mask from C
;        XOR     $03             7; and make blue/yellow.
;
;        LD      C,A             4; store the new long-term byte.
;
;        LD      H,$00           7; set up parity byte as zero.
;        LD      B,$B0           7; timing.
;        JR      L05C8           10; forward to LD-MARKER 
;                                ; the loop mid entry point with the alternate 
;                                ; zero flag reset to indicate first byte 
;                                ; is discarded.
;
;; --------------
;;   the loading loop loads each byte and is entered at the mid point.
;
;;; LD-LOOP
;L05A9:  EX      AF,AF'          4; restore entry flags and type in A.
;        JR      NZ,L05B3        12/7; forward to LD-FLAG if awaiting initial flag
;                                ; which is to be discarded.
;
;        JR      NC,L05BD        12/7; forward to LD-VERIFY if not to be loaded.
;
;        LD      (IX+$00),L      19; place loaded byte at memory location.
;        JR      L05C2           10; forward to LD-NEXT
;
;; ---
;
;;; LD-FLAG
;L05B3:  RL      C               8; preserve carry (verify) flag in long-term
;                                ; state byte. Bit 7 can be lost.
;
;        XOR     L               4; compare type in A with first byte in L.
;        RET     NZ              11/5; return if no match e.g. CODE vs. DATA.
;
;;   continue when data type matches.
;
;        LD      A,C             4; fetch byte with stored carry
;        RRA                     4; rotate it to carry flag again
;        LD      C,A             4; restore long-term port state.
;
;        INC     DE              6; increment length ??
;        JR      L05C4           10; forward to LD-DEC.
;                                ; but why not to location after ?
;
;; ---
;;   for verification the byte read from tape is compared with that in memory.
;
;;; LD-VERIFY
;L05BD:  LD      A,(IX+$00)      19; fetch byte from memory.
;        XOR     L               4; compare with that on tape
;        RET     NZ              11/5; return if not zero. 
;
;;; LD-NEXT
;L05C2:  INC     IX              10; increment byte pointer.
;
;;; LD-DEC
;L05C4:  DEC     DE              6; decrement length.
;        EX      AF,AF'          4; store the flags.
;        LD      B,$B2           7; timing.
;
;;   when starting to read 8 bits the receiving byte is marked with bit at right.
;;   when this is rotated out again then 8 bits have been read.
;
;;; LD-MARKER
;L05C8:  LD      L,$01           7; initialize as %00000001
;
;;; LD-8-BITS
;L05CA:  CALL    L05E3           10; routine LD-EDGE-2 increments B relative to
;                                ; gap between 2 edges.
;        RET     NC              11/5; return with time-out.
;
;        LD      A,$CB           7; the comparison byte.
;        CP      B               4; compare to incremented value of B.
;                                ; if B is higher then bit on tape was set.
;                                ; if <= then bit on tape is reset. 
;
;        RL      L               8; rotate the carry bit into L.
;
;        LD      B,$B0           7; reset the B timer byte.
;        JP      NC,L05CA        12/7; JUMP back to LD-8-BITS
;
;;   when carry set then marker bit has been passed out and byte is complete.
;
;        LD      A,H             4; fetch the running parity byte.
;        XOR     L               4; include the new byte.
;        LD      H,A             4; and store back in parity register.
;
;        LD      A,D             4; check length of
;        OR      E               4; expected bytes.
;        JR      NZ,L05A9        12/7; back to LD-LOOP 
;                                ; while there are more.
;
;;   when all bytes loaded then parity byte should be zero.
;
;        LD      A,H             4; fetch parity byte.
;        CP      $01             7; set carry if zero.
;        RET                     10; return
;                                ; in no carry then error as checksum disagrees.
;
;; -------------------------
;; Check signal being loaded
;; -------------------------
;;   An edge is a transition from one mic state to another.
;;   More specifically a change in bit 6 of value input from port $FE.
;;   Graphically it is a change of border colour, say, blue to yellow.
;;   The first entry point looks for two adjacent edges. The second entry point
;;   is used to find a single edge.
;;   The B register holds a count, up to 256, within which the edge (or edges) 
;;   must be found. The gap between two edges will be more for a '1' than a '0'
;;   so the value of B denotes the state of the bit (two edges) read from tape.
;
;; ->
;
;;; LD-EDGE-2
;L05E3:  CALL    L05E7           10; call routine LD-EDGE-1 below.
;        RET     NC              11/5; return if space pressed or time-out.
;                                ; else continue and look for another adjacent 
;                                ; edge which together represent a bit on the 
;                                ; tape.
;
;; -> 
;;   this entry point is used to find a single edge from above but also 
;;   when detecting a read-in signal on the tape.
;
;;; LD-EDGE-1
;L05E7:  LD      A,$16           7; a delay value of twenty two.
;
;;; LD-DELAY
;L05E9:  DEC     A               4; decrement counter
;        JR      NZ,L05E9        12/7; loop back to LD-DELAY 22 times.
;
;        AND      A              4; clear carry.
;
;;; LD-SAMPLE
;L05ED:  INC     B               4; increment the time-out counter.
;        RET     Z               11/5; return with failure when $FF passed.
;
;				(4+12)*22+4+4+5+7+11 aprox 380T (400T)
;
;        LD      A,$7F           7; prepare to read keyboard and EAR port
;        IN      A,($FE)         11; row $7FFE. bit 6 is EAR, bit 0 is SPACE key.
;        RRA                     4; test outer key the space. (bit 6 moves to 5)
;        RET     NC              11/5; return if space pressed.  >>>
;
;        XOR     C               4; compare with initial long-term state.
;        AND     $20             7; isolate bit 5
;        JR      Z,L05ED         12/7; back to LD-SAMPLE if no edge.
;
;;   but an edge, a transition of the EAR bit, has been found so switch the
;;   long-term comparison byte containing both border colour and EAR bit. 
;
;        LD      A,C             4; fetch comparison value.
;        CPL                     4; switch the bits
;        LD      C,A             4; and put back in C for long-term.
;
;        AND     $07             7; isolate new colour bits.
;        OR      $08             7T; set bit 3 - MIC off.
;        OUT     ($FE),A         11T; send to port to effect the change of colour. 
;
;        SCF                     4T; set carry flag signaling edge found within
;                                ; time allowed.
;        RET        		10T
;
;
