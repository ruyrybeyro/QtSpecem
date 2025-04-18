
/* Stack.c: Z80's stack operations - basic support routines.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

#undef push
#undef pop

/*=========================================================================*
 *                            push                                         *
 *=========================================================================*/
void push(USHORT value)
{
   /* defined as a macro in z80.h */
   writeword(SP -= 2, value);
}

/*=========================================================================*
 *                            pop                                          *
 *=========================================================================*/
USHORT pop()
{
   /* defined as a macro in z80.h */
   SP += 2;
   return(readword(SP - 2) );
}

/* EOF: Stack.c */
