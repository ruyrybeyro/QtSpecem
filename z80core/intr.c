/* Intr.c: Z80 interrupts -- basic support routines.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */


#include "env.h"

/*=========================================================================*
 *                           do_interrupt                                  *
 *=========================================================================*/
void do_interrupt()
{
   /* At the begin of a interrupt, interrupt signals are deactivated
	*/
   IFF1 = IFF2 = 0;

   push((USHORT)PC);

   /* Do interrupt acord to _IM
	 _IM can be 0, 1 or 2
   */
   switch(_IM)
   {
      case (UCHAR)1: /* IM 1 */
			/* If IM_1 calls 0x38
            */
            clock_ticks = 13; /* Time : Ian Collier */
            PutPC(0x38);
         break;
      case (UCHAR)2: /* IM 2 */
	    // clock_ticks = 19; /* Time : Ian Collier */
            // Ian Collier says 19, RAMSOFT in their floatspy say 29
            // emulation seems still to work better with 28
	    clock_ticks = 28;
            /* PC = readword( (USHORT) ( ((USHORT)I << 8) + DATA_BUS) );
            */
            PutPC(readword( (USHORT) ( ((USHORT)I << 8) | (USHORT)0x00ff) ));

            //PutPC( readbyte( (I << 8) | (USHORT)0x00ff ) | (readbyte((I+1)<<8) << 8) ); 
         break;
	  default:
	    /*
	     In IM_0 the Z80 processor waits for a instruction opcode
	     in the data bus (this mode was created for compatibility
	     with the 8080). In the Spectrum 0xFF is inserted in the
	     data bus, wich is 0x38 opcode, being so IM_0 = IM_1
            */
            clock_ticks = 17;
            /* execute_one_inst(DATA_BUS);
            */
            PutPC(0x38);
         break;
   }
   WZ = PC;
   R++;	     // having an interrupt increases R 
}

/*=========================================================================*
 *                           do_nmi_int                                    *
 *=========================================================================*/
void do_nmi_int()
{
   T(11); /* Acording to Ian Collier */

   /* Deactivate interrupts -- but IIF2 keeps copy of last IFF1 */
   IFF1 = 0;

   push((USHORT)PC);

   PutPC(WZ = 0x66);
}

/* EOF: Intr.C */
