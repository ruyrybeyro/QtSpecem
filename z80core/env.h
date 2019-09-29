
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
extern union Z80Regs  Z80Regs;

/* Alternative registers */
extern union Z80Regs  Z80Regs2;

extern union Z80IX Z80IX;
extern union Z80IY Z80IY;

extern USHORT PC;

extern USHORT  SP;
extern UCHAR   R, R_BIT7,  I;

extern struct CPU_flags  flags;

/* 'ticks' counter ; SPECTRUM Z80A - 3,5469MHz -
  70938 'ticks' between each int (50 per second)
*/
extern unsigned long  clock_ticks;

/* Interrupt mode - 0, 1 or 2 */
extern UCHAR _IM;

/* interrupt 'switch' interrupts and copy
 *
 *            IFF1  IFF2
 *  ----------------------
 *  | RESET  |  0  |  0  |
 *  |--------+-----+-----|
 *  | DI     |  0  |  0  |
 *  |--------+-----+-----|
 *  | EI     |  1  |  1  |
 *  |--------+-----+-----|
 *  | LD A,I |  .  |  .  | - P/V = IFF2
 *  |--------+-----+-----|
 *  | LD A,R |  .  |  .  | - P/V = IFF2
 *  |--------+-----+-----|
 *  | NMI    |  0  |  .  |
 *  |--------+-----+-----|
 *  | RETN   | IFF2|  .  |
 *  |--------+-----+-----|
 *  | INT    |  0  |  0  |
 *  |--------+-----+-----|
 *  | RETI   |  .  |  .  | - same as RET. (This instruction only exists
 *  ----------------------              to be recognized by external
 *                                      devices, so they know int is over)
 */
extern UCHAR IFF1, IFF2;

/* Used in DDCB or FDCB to keep shift of IX or IY
 */
extern UCHAR lastbyte;

/* 64k Mem
 */
extern UCHAR * mem;

/* 'flag' WriteRom :
 *  0 : 16k of ROM, 48k of RAM
 *  1 : 64k of RAM
 */
extern UCHAR WriteRom;

/* 'flag' trace
 */
extern UCHAR TraceOn;

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
#define build_F() F = (UCHAR)((flags._C != 0) | ((flags._N != 0) << 1) | \
((flags._P != 0) << 2) | ((flags._Y != 0) << 3)| ((flags._H != 0) << 4) | \
((flags._X != 0) << 5) | ((flags._Z != 0) << 6) | ((flags._S != 0) << 7));

/* build interpreter flags from F
 */
#define read_F() flags._S=F & (UCHAR)BIT_7, flags._Z=F & (UCHAR)BIT_6, \
flags._X=F & (UCHAR)BIT_5, flags._H=F & (UCHAR)BIT_4, flags._Y=F & \
(UCHAR)BIT_3, flags._P=F & (UCHAR)BIT_2, flags._N = F & (UCHAR)BIT_1, \
flags._C =F & (UCHAR)BIT_0;




/* stack management - push and pop
 */
#define push(value) writeword((USHORT)(SP -=2), value)
#define pop() (SP += 2, (USHORT)readbyte(SP - 2) | \
	      (((USHORT)readbyte(SP - 1)) << 8)  )

/* instead of a function, now we have a array
 */
#define   parity(x) (parity_table[x])

/* EOF: Env.h */
