/* Flags.c: Z80 flags --  basic support routines.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "../h/env.h"

#undef build_F
#undef read_F

/*=========================================================================*
 *                            build_flags                                  *
 *=========================================================================*/
void build_F()
{
   /* defined as macro in z80.h */
   /* Build F from interpreter flags only we need them */
   F = (Z80_C != 0) | ((Z80_N != 0) << 1) | ((Z80_P != 0) << 2) |
   ((Z80_Y != 0) << 3) | ((Z80_H != 0) << 4) | ((Z80_X != 0) << 5) |
   ((Z80_Z != 0) << 6) | ((Z80_S != 0) << 7);
}

/*=========================================================================*
 *                            read_flags                                   *
 *=========================================================================*/
void read_F()
{
   /* defined as macro in z80.h */
   /* Build interpreter flags with F */
   Z80_S = (F & (UCHAR)BIT_7);
   Z80_Z = (F & (UCHAR)BIT_6);
   Z80_X = (F & (UCHAR)BIT_5);
   Z80_H = (F & (UCHAR)BIT_4);
   Z80_Y = (F & (UCHAR)BIT_3);
   Z80_P = (F & (UCHAR)BIT_2);
   Z80_N = (F & (UCHAR)BIT_1);
   Z80_C = (F & (UCHAR)BIT_0);
}

/* EOF: Flags.c */
