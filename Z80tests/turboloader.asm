; ------------------------------------
; Load header or block of information
; ------------------------------------
;   This routine is used to load bytes and on entry A is set to $00 for a 
;   header or to $FF for data.  IX points to the start of receiving location 
;   and DE holds the length of bytes to be loaded. 

	ORG	50000
        NOP
	LD	IX,16384
	LD	DE,6912
	LD	A,$FF
	BIT	0,C
	JR	Z,LD_ENTRY
        JP      SA_BYTES

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

;	LD	A,IXL
;	db $DD, $7D  
;	XOR	L
        AND     $07             ; isolate new colour bits.
        OR      $08             ; set bit 3 - MIC off.
        OUT     ($FE),A         ; send to port to effect the change of colour. 

        SCF                     ; set carry flag signaling edge found within
                                ; time allowed.
        RET                     ; return.



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

