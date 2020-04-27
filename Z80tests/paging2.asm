
; (C) Rui Ribeiro Fev 2020
;
; Invoked from 128K mode BASIC

        ORG     31000

; Paging variable from 128K model onwards
; $7ffd - Bits 0-2: RAM page (0-7) to map into memory at 0xc000.

	LD	C,0	       ; page
        LD      A,($5b5c)      ; system copy of $7ffd

        AND     $f8            ; reset bit 0-2
                               ; RAM page/bank to load at $C000
        OR      C              ; Select bank 4

        LD      BC,$7ffd
        DI
        LD      ($5b5c),A      ; save copy of the new value

        OUT     (C),A          ; change bank 0 for bank 4 at $C000
        RET

	END	31000
