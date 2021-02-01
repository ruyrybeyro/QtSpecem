
;
; Routine for the 128K+2/2A/3 to load bank 7 ROM
; and map it on top of ROM/address $0000
;
; Invoked from 128K mode BASIC
;
; By default memory banks will be ROM 0->Bank 5 (screen)->Bank 2->Bank 0
; Each RAM bank is 16KB
;
; Requirements before calling it, CLEAR 30999 ; if it not done it will crash 
; as stack needs to be bellow 32768 by the end of this routine, as
; bank 0 and bank 2 will shift positions
;
; see https://www.worldofspectrum.org/faq/reference/128kreference.htm
;
; CLEAR 30999: LOAD "THIS" CODE : RAND USR 31000 : LOAD "ROM" CODE : RAND USR 31001 
;
; Short note: USR BASIC routine loads BC with called address

	ORG	31000
	NOP
	LD	A,C               ; point of entry for 2nd time calling
	AND	1		  
	JR	NZ,SPECIAL_MODE   ; if BC odd, go to SPECIAL_MODE


; First moves bank 4 to $C000 (49152) replacing  Bank 0
;
; called with RANDOMIZE USR 31000 (first time)
;
; Paging variable from 128K model onwards
; $7ffd - Bits 0-2: RAM page (0-7) to map into memory at 0xc000.

	LD      A,($5b5c)      ; system copy of $7ffd

	AND     $f8	       ; reset bit 0-2
			       ; RAM page/bank to load at $C000
	OR      3              ; Select bank 3 (non contended)

	LD      BC,$7ffd
	DI
	LD      ($5b5c),A      ; save copy of the new value

	OUT     (C),A	       ; change bank 0 for bank 4 at $C000
	RET

; After LOAD "" CODE 49152
; called with RANDOMIZE USR 31001
;
;
; for loading the "ROM" in Bank 4 at $C000
; 
; shift for special mode 2
;
; bank 3 - C000->$FFFF which we LOADed"" will stay at same address 
;
; Bank 4->Bank 5->Bank 6->Bank 3
;
; Paging variable from 128K +2A Onwards
; $1ffd - Bit 0: Paging mode. 0=normal, 1=special
;         Bit 2=1 and Bit 1=0 : Banks 4->5->6->3

SPECIAL_MODE:

	LD      A,($5b67)	; system copy of $1ffd

	OR      $07		
	XOR     $02A		; Special mode, mode 2

	LD      BC,$1ffd
	DI
	LD      ($5b67),A	; save copy of the new value

	OUT     (C),A		; change for special mode 2
				; 4->5->6->3



	JP      $FE51	; JUMP to the beginning of the "new" "ROM"

	END	31000

