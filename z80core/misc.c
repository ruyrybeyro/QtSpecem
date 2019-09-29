/* Misc.c: Z80 misc arithmetic & CPU control instructions.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

/*=========================================================================*
 *                            daa                                          *
 *=========================================================================*/
void daa()
{
   LOCAL UCHAR t;
    
   t=0;
    
   T(4);
    
   if(flags._H || (((A & (UCHAR)0xF) > (UCHAR)9) ))
         t++;
    
   if(flags._C || ((UCHAR)A) > (UCHAR)0x99)
   {
            t += 2;
            flags._C = 1;
   }
    
    if ( flags._N && !flags._H )
      flags._H=0;
    else
    {
       if ( flags._N && flags._H )
          flags._H = (((UCHAR)(A & 0x0F)) < 6);
      else
           flags._H = (((UCHAR)(A & 0x0F)) >= 0x0A);
    }
    
    switch(t)
    {
        case 1:
            A += (flags._N)?(UCHAR)0xFA:(UCHAR)6; // -6:6
            break;
        case 2:
            A += (flags._N)?(UCHAR)0xA0:(UCHAR)0x60; // -0x60:0x60
            break;
        case 3:
            A += (flags._N)?(UCHAR)0x9A:(UCHAR)0x66; // -0x66:0x66
            break;
    }
    
   flags._S = ( A & (UCHAR)BIT_7);
   flags._Z = !A;
   flags._P = parity(A);
   flags._X = A & (UCHAR)BIT_5;
   flags._Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            cpl                                          *
 *=========================================================================*/
void cpl()
{
   T(4);
   A = (UCHAR)~A;
   flags._H = flags._N = 1;
   flags._X = A & (UCHAR)BIT_5;
   flags._Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            neg                                          *
 *=========================================================================*/
void neg()
{
   T(8);
   flags._S = ((A =(UCHAR)(~A + 1)) & (UCHAR)BIT_7);
   flags._P = (A == (UCHAR)BIT_7);
   flags._H = A & 0x0F;
   flags._C = A;
   flags._Z = !A;
   flags._N = 1;
   flags._X = A & (UCHAR)BIT_5;
   flags._Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            ccf                                          *
 *=========================================================================*/
void ccf()
{
   T(4);
   flags._C = !(flags._H = flags._C);
   flags._N = 0;
   if (Q)
   {
   flags._X = (A & (UCHAR)BIT_5) > 0;
   flags._Y = (A & (UCHAR)BIT_3) > 0;
   }
   else
   {
   flags._X = flags._X || (A & (UCHAR)BIT_5);
   flags._Y = flags._Y || (A & (UCHAR)BIT_3);
   }
   Q = 1;
}

/*=========================================================================*
 *                            scf                                          *
 *=========================================================================*/
void scf()
{
   T(4);
   flags._C = 1;
   flags._H = flags._N = 0;
   if (Q)
   {
   flags._X = (A & (UCHAR)BIT_5) > 0;
   flags._Y = (A & (UCHAR)BIT_3) > 0;
   }
   else
   {
   flags._X = flags._X || (A & (UCHAR)BIT_5);
   flags._Y = flags._Y || (A & (UCHAR)BIT_3);
   }
   Q = 1;
}

/*=========================================================================*
 *                            nop                                          *
 *=========================================================================*/
void nop()
{
   T(4);
   Q = 0;
}

/*=========================================================================*
 *                            halt                                         *
 *=========================================================================*/
void halt()
{
   if(!IFF1)
   {
      Panic("HALT with interrupts disabled!");
   }

   /* This loop is cut because timing software is already
      working
    */
   /*while(!int_time())
   {
      nop();
   } */
   clock_ticks = INT_TIME + (clock_ticks % 4 );
   Q = 0;
}

/*=========================================================================*
 *                            di                                           *
 *=========================================================================*/
void di()
{
   T(4);
   IFF1 = IFF2 = 0;
   Q = 0;
}

/*=========================================================================*
 *                            ei                                           *
 *=========================================================================*/
void ei()
{
   T(4);
   if(int_time())
      clock_ticks = INT_TIME - 1;

   IFF1 = IFF2 = 1;
   Q = 0;
}

/*=========================================================================*
 *                            im_0                                         *
 *=========================================================================*/
void im_0()
{
   T(8);
   _IM = 0;
   Q = 0;
}

/*=========================================================================*
 *                            im_1                                         *
 *=========================================================================*/
void im_1()
{
   T(8);
   _IM = 1;
   Q = 0;
}

/*=========================================================================*
 *                            im_2                                         *
 *=========================================================================*/
void im_2()
{
   T(8);
   _IM = 2;
   Q = 0;
}


/*** auxiliary routines  ***/

/*=========================================================================*
 *                            dummyED                                      *
 *=========================================================================*/
void dummyED()
{
   T(8);
   /* invalid opcode (non existant) */
   Q = 0;
}

void dummyIX()
{
   T(4);
   PC--;
   Q = 0;
}

void dummyIY()
{
   T(4);
   PC--;
   Q = 0;
}

/* No longer used! --- replace by an array
 */

/*=========================================================================*
 *                            parity                                       *
 *=========================================================================*/
/* parity -- in Z80 parity is even */
/* USHORT parity(num)
UCHAR num;
{
   LOCAL UCHAR parit;
   LOCAL UCHAR number;

   number = num;
   parit = 1;
   while(number)
   {
      if((UCHAR)0x01 & number)
	 parit ^= (UCHAR)1;
      number >>= 1;
   }
   return((USHORT)parit);
}
*/
/* Misc.c */

