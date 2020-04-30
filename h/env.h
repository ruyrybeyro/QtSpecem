
/* Env.h : Z80 global environment.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "z80.h"
#include "iglobal.h"

#define BEGIN_RAM   16384
#define BEGIN_VIDEO BEGIN_RAM

/* Len of description of Z80 files */
#define LEN_DESCRIPT 30

/* For external declarations, see INIT.C */

/* vars to keep states of options for emulator */
extern unsigned char 	    bModel3;    /* Model 3 or Model 2   */
extern unsigned char        bFlashOn;   /* Flash on or off      */
extern unsigned char        bSoundOn;   /* Sound on or off      */
extern unsigned char        Scale;      /* Scale of main window */
extern unsigned char        ScreenUpdate;/* Screen update: 50 fps
					      /ScreenUpdate
					  */

extern unsigned short       joystate;   /* Spectrum joystick configuration
					 */

extern unsigned char    keyb_template;	/* keyb template
					 */

/* vars needed to keep track of state of emulator */
extern unsigned char WindowDirty;      /* Have we to redraw window?         */
extern unsigned char FlashState;       /* Are colours inverted or normal?   */


/* Main registers        */
extern union Z80Regs  * Z80Regs;

/* Alternative registers */
extern union Z80Regs  * Z80Regs2;

extern struct Z80vars * Z80vars;

extern union Z80IX * Z80IX;
extern union Z80IY * Z80IY;

extern struct CPU_flags  * flags;

/* Used in DDCB or FDCB to keep shift of IX or IY
 */
extern UCHAR lastbyte;

/* 64k Mem
 */
extern UCHAR * mem;

extern UCHAR * vars;

/* 'flag' WriteRom :
 *  0 : 16k of ROM, 48k of RAM
 *  1 : 64k of RAM
 */
extern UCHAR WriteRom;

extern USHORT parity_table[256];

#define ResetTickCounter() clock_ticks = 0L;
#define int_time() (clock_ticks >= INT_TIME)
#define Force_interrupt() (clock_ticks = INT_TIME+1)

/* Increment counter by (cycles)
 */
#define T(cycles) (clock_ticks += cycles)

/* Decrement counter by (cycles)
 */
#define dT(cycles) (clock_ticks -= cycles)

/* Macro to put adress in PC
 */
#define PutPC(adress) PC = (USHORT)(adress)

/*  memory adressing - readbyte()
 */
#define readbyte(adress) (*(mem+(USHORT)(adress)))

/* read word pointed to PC and sum 2 to PC
 */
#define Getnextword() (PC += 2, readword((USHORT)(PC - 2)) )


/* read byte pointed to PC and increment it
 */
#define Getnextbyte() readbyte(PC++)

/* Convert unsigned char to signed short (with signal extension)
 */
#define ucharToUshort(uchar) ((signed short)((signed char)(uchar)))

#define pIX   (WZ=IX+ucharToUshort(Getnextbyte()))
#define pIY   (WZ=IY+ucharToUshort(Getnextbyte()))
#define pCBIX (WZ=IX+ucharToUshort(lastbyte))
#define pCBIY (WZ=IY+ucharToUshort(lastbyte))

/* build F from interpreter flags when needed
 */
#define build_F() F = (UCHAR)((Z80_C != 0) | ((Z80_N != 0) << 1) | \
((Z80_P != 0) << 2) | ((Z80_Y != 0) << 3)| ((Z80_H != 0) << 4) | \
((Z80_X != 0) << 5) | ((Z80_Z != 0) << 6) | ((Z80_S != 0) << 7));

/* build interpreter flags from F
 */
#define read_F() Z80_S=F & (UCHAR)BIT_7, Z80_Z=F & (UCHAR)BIT_6, \
Z80_X=F & (UCHAR)BIT_5, Z80_H=F & (UCHAR)BIT_4, Z80_Y=F & \
(UCHAR)BIT_3, Z80_P=F & (UCHAR)BIT_2, Z80_N = F & (UCHAR)BIT_1, \
Z80_C =F & (UCHAR)BIT_0;




/* stack management - push and pop
 */
#define push(value) writeword((USHORT)(SP -=2), value)
#define pop() (SP += 2, (USHORT)readbyte(SP - 2) | \
	      (((USHORT)readbyte(SP - 1)) << 8)  )

/* instead of a function, now we have a array
 */
#define   parity(x) (parity_table[x])

/* EOF: Env.h */
