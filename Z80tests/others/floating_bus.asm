	org	50000

	ld 	a,9 	;INK 1; PAPER 1; BRIGHT 0; FLASH 0
	ld 	b,32
	ld 	hl,$5a40 ;draw a 32 char-wide strip
loop1: 	ld 	(hl),a
	inc 	l
	djnz 	loop1

	ld 	bc,6143
	ld 	hl,$4000
	ld 	de,$4001
	ld 	a,9 	;fill pattern
	ld 	(hl),a
	ldir
fl_bus:
	ld 	de,$940 ;attr into D, MSB of port addr into E
loop2: 	dec 	hl 	;[6]padding instruction
	ld 	a,e 	;[4]MSB of port addr into A
	in 	a,($ff) ;[11]read port 0x40FF into A
	cp 	d ;[4]is it D (i.e. INK 1, PAPER 1, BRIGHT 0; FLASH 0)?
	jp 	nz,loop2 ;[10]no? keep trying
	
	ld 	a,1 	;border blue
	out 	(254),a

	ld 	b,128 	;wait
wait:	djnz 	wait

	ld 	a,7 	;border white
	out 	(254),a

	jr 	fl_bus

	end 	50000
