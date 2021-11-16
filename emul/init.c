/* Init.c: Init emulation.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include <stdlib.h>

#include "../h/z80.h"
#include "../h/iglobal.h"

unsigned char * alloc_speccy_shared_ram(void);
unsigned char * alloc_speccy_shared_vars(void);
unsigned char * dealloc_shared(unsigned char *, unsigned char *);

/* vars to keep states of options for emulator */
unsigned char 	     bModel3 = 1;   /* Model 3 or Model 2   */
unsigned char        bFlashOn = 1;  /* Flash on or off      */
unsigned char        bSoundOn = 1;  /* Sound on or off      */
unsigned char        Scale = 1;     /* Scale of main window */

/* vars needed to keep track of state of emulator */
unsigned char  WindowDirty  = 1;     /* Have we to redraw window?          */
unsigned char  FlashState   = 1;     /* Are colours inverted or normal?    */
unsigned char  ScreenUpdate = 2;     /* Screen update: 50 fps/ScreenUpdate */
unsigned char   keyb_template = 0;   /* keyb template
				      */

/* Main registers   */
union Z80Regs  * Z80Regs;

/* Alternative registers */
union Z80Regs  * Z80Regs2;

union Z80IX * Z80IX;
union Z80IY * Z80IY; 

struct CPU_flags  * flags;
struct Z80vars * Z80vars;

/* 'ticks' counter ; SPECTRUM Z80A - 3,5469MHz - 70938 ticks between each INT */
// unsigned long clock_ticks;

/* Interrupt mode - 0, 1 or 2 */
// UCHAR _IM;

/* Interrupt flag & copy
 */
// UCHAR IFF1, IFF2;

/* Used in DDCB and FDCB to keep a indice for IX and IY */
UCHAR lastbyte;

/* Memory 64k */
UCHAR * mem;
UCHAR * vars;

/* ROM flag:
 *  0 : 16k of ROM, 48k of RAM
 *  1 : 64k of RAM
 */
UCHAR WriteRom = 1;

/* TraceOn defined in z80debug/z80debug.c & z80/ndebgz80.c
 */
static UCHAR inited = 0;

extern UCHAR ULAplus;


/*=========================================================================*
 *                            Init_Z80Emu                                  *
 *=========================================================================*/
void Init_Z80Emu(void)
{
	if(inited++)
	{
	  Warning("Z80 emulation already inited");
	}
	else
        {
	/* Alloc mem for emulation --- allways 64Kb, so do
	 not have to test 'out-of-bounds' reading and
	 writing memory
	i.e. PC = unsigned short (0-65535) ;
	*/


	  // mem =  (UCHAR *)calloc( 65536, 1 );
	  mem =  alloc_speccy_shared_ram();
          vars = alloc_speccy_shared_vars();
          Z80vars  = (struct Z80vars *)  vars;
          Z80Regs  = (union Z80Regs *)   vars + sizeof(struct Z80vars);
          Z80Regs2 = (union Z80Regs *)   vars + sizeof(struct Z80vars) + sizeof(union Z80Regs);
          flags    = (struct CPU_flags *)vars + sizeof(struct Z80vars) + sizeof(union Z80Regs) + sizeof(union Z80Regs);
          Z80IX    = (union Z80IX *)     vars + sizeof(struct Z80vars) + sizeof(union Z80Regs) + sizeof(union Z80Regs) + sizeof(struct CPU_flags);
          Z80IY    = (union Z80IY *)     vars + sizeof(struct Z80vars) + sizeof(union Z80Regs) + sizeof(union Z80Regs) + sizeof(struct CPU_flags) + sizeof(union Z80IX);
	}
        do_fullreset();
}

/*=========================================================================*
 *                            do_fullreset                                 *
 *=========================================================================*/
void do_fullreset()
{
        BreakPoint = -1;
        TraceOn    = 0;

	ULAplus    = 0;

	// reset Timex 2048 extra video modes
        writeport(255,0);
 
        do_reset();
}

/*=========================================================================*
 *                            do_warmreset                                 *
 *=========================================================================*/
void do_warmreset()
{
   di(); 
   writeport(0xFE, 7);
   I=0x3F;
   // Set the system variables RASP and PIP
   writebyte(0x5c38,0x0040);
   // Set RAMTOP
   writeword(0x5CB2, 0xFF57);
   //  CHARS
   writeword(0x5C36, 0x3C00);
   // The top location (RAMTOP) is made to hold +3E
   writebyte(0xFF57, 0x3E);
   // RAMTOP-1 = 0
   writebyte(0xFF56,0);
   SP=0xFF56;
   // ERR-SP
   writeword(0x5C3D,0xFF54);
   im_1();
   IY=0x5C3A;
   // The system variable CHANS is set to the base address of the channel information area.
   writeword(0x5C4F,0x5CB6);
   // The initial channel data is moved from the table (CHANINFO) to the channel information area.
   HL=0x15AF;
   DE=0x5CB6;
   BC=0x15;
   ldir();
   ei();
   // E_LINE
   HL=readbyte(0x5C59);
   PC=0x1256;
}


/*=========================================================================*
 *                            Close_Z80Emu                                 *
 *=========================================================================*/
void Close_Z80Emu()
{
   if(inited)
   {
      inited--;
   }
   dealloc_shared(mem, vars);   
}

/* EOF: Init.C */

