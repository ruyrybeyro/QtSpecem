
; call it with PRINT USR 50000

	ORG	50000
	LD      BC,  $27FF   ; After this WZ will be $2800, bit 13 = 1; bit 11 = 1
        LD      A, (BC)     ; WZ = $27FF + 1
        BIT     4, (HL)    ; F[3]=WZ[11]; F[5]=WZ[13]
        PUSH    AF        ; push AF onto the stack
        POP     BC         ; pop it into BC
        LD      A, C        ; Take the lower byte (i.e. F)
        AND     $20        ; AND with $20 - i.e. mask out bit 5
        LD      B, A        ; Push into B (for return to BASIC)
        LD      A, C        ; Take F again
        AND     $08        ; AND with $08 - i.e. mask out bit 3
        LD      C, A        ; Push into C (for return to BASIC) should return 8200
        RET
