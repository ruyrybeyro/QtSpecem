/* Flags.c: Z80 flags --  basic support routines.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

#undef build_F
#undef read_F

/*=========================================================================*
 *                            build_flags                                  *
 *=========================================================================*/
void build_F()
{
   /* defined as macro in z80.h */
   /* Build F from interpreter flags only we need them */
   F = (flags._C != 0) | ((flags._N != 0) << 1) | ((flags._P != 0) << 2) |
   ((flags._Y != 0) << 3) | ((flags._H != 0) << 4) | ((flags._X != 0) << 5) |
   ((flags._Z != 0) << 6) | ((flags._S != 0) << 7);
}

/*=========================================================================*
 *                            read_flags                                   *
 *=========================================================================*/
void read_F()
{
   /* defined as macro in z80.h */
   /* Build interpreter flags with F */
   flags._S = (F & (UCHAR)BIT_7);
   flags._Z = (F & (UCHAR)BIT_6);
   flags._X = (F & (UCHAR)BIT_5);
   flags._H = (F & (UCHAR)BIT_4);
   flags._Y = (F & (UCHAR)BIT_3);
   flags._P = (F & (UCHAR)BIT_2);
   flags._N = (F & (UCHAR)BIT_1);
   flags._C = (F & (UCHAR)BIT_0);
}

/* EOF: Flags.c */
