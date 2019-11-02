/* Kernel.c: Z80 initialization and main cycle - basic support routines.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"
#include "ivars.h"

#include <stdio.h>
#include <string.h>

void save_sna(const char * file_name);

/* Increment the lower 7 bits of R in each M1 cycle
*/
#define inc_R() (R++)

/* Opcode being interpreted - in IX or IY */
//static UCHAR opcode;

int ChangeFlashTime = 0;

/*=========================================================================*
 *                            do_reset                                     *
 *=========================================================================*/
void do_reset()
{
   /* CPU internal flags  */
   _IM = IFF1 = IFF2 =

	/* CPU registers */
   R_BIT7 = R = I = 0;

   HL = BC = DE = AF = IX = IY = SP =

   /* alternative registers */
   HL2 = BC2 = DE2 = AF2 = WZ = WZ2 = 0xFFFF;

   /* flags CPU */
   Z80_S = Z80_Z = Z80_X = Z80_H = Z80_Y =
   Z80_P = Z80_N = Z80_C = 0;
   /* Interrupt counter */
   ResetTickCounter();
   T(3);
   /* Program Counter */
   PutPC(0);

   ChangeFlashTime = 0;
}

/*=========================================================================*
 *                            execute                                      *
 *=========================================================================*/
void execute()
{
  /* Z80 main cycle */
  /* --> 0xDD e 0xFD are only 'switches' wich map IX or IY in HL
      till instruction end [but not in instructions prefixed by ED]
     --> 0xED, 0xCB are 'gates' to another sets of instructions
   */
        do
	{

        if (!TraceOn)
        {
	   inc_R();
	   /* Call funtion indexed by opcode */
	   (*instruc_tabl[Getnextbyte()])();
            /* if (PC > 16383)
                 {
                    static int n = 0;
                    char s[200];

                    sprintf(s, "/tmp/w%06d.z80", n++);
                    save_sna(s);
                 }   */

        }
        else
        {
           if (TraceOn >  2)
           {
              inc_R();
              (*instruc_tabl[Getnextbyte()])();
                 TraceOn--;
           }
        }
	}
        while( (clock_ticks < INT_TIME) && !TraceOn );
   
  /* do_int_tasks(); */
  /* if interrupts activated */
  if( clock_ticks >= INT_TIME )
  {
  if(IFF1)
     {
     do_interrupt();
     }
  else
     ResetTickCounter();

  // ULA inverts FLASH every 16 frames
  if(++ChangeFlashTime == 16)
  {
     ChangeFlashTime = 0; // reset counter
     FlashState ^= 1;     // signal colours inverted
     do_flash();
  }
  }
}

/*=========================================================================*
 *                            execute_IX                                   *
 *=========================================================================*/
static void execute_IX()
{
   inc_R();	/* It appears to be this way */

   (*instruc_tablIX[Getnextbyte()])();
}

/*=========================================================================*
 *                            execute_IY                                   *
 *=========================================================================*/
static void execute_IY()
{
   inc_R();	/* It appears to be this way */

   /* Call function acording to opcode */
   (*instruc_tablIY[Getnextbyte()])();
}

/*=========================================================================*
 *                            execute_CB                                   *
 *=========================================================================*/
static void execute_CB()
{
   inc_R();
   (*instruc_tablCB[Getnextbyte()])(); 
}  

/*=========================================================================*
 *                          execute_IXCB                                   *
 *=========================================================================*/
static void execute_IXCB()
{
   /* If IX or IY is active, then the next byte is not
	 a instruction, but a displacement for IX or IY
   */
   // inc_R(); // ??
   lastbyte = Getnextbyte();
   (*instruc_tablIXCB[Getnextbyte()])();
}

/*=========================================================================*
 *                            execute_CB                                   *
 *=========================================================================*/
static void execute_IYCB()
{
   /* If IX or IY is active, then the next byte is not
	 a instruction, but a displacement for IX or IY
   */
   // inc_R(); // ??
   lastbyte = Getnextbyte();
   (*instruc_tablIYCB[Getnextbyte()])();
}

/*=========================================================================*
 *                            execute_ED                                   *
 *=========================================================================*/
static void execute_ED()
{
   inc_R();
   (*instruc_tablED[Getnextbyte()])();
}

/* EOF: Kernel.c */
