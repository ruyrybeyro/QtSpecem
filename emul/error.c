/* Error.c : Dialog boxes for emulator errors.
 *
 * Copyright 1996-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include <string.h>
#include <ctype.h>
#include "env.h"

/*=========================================================================*
 *                            warning                                      *
 *                                                                         *
 * Non fatal error, just gives the message                                 * 								
 *=========================================================================*/
void Warning(const char * msg)
{
   // MessageBox(NULL, msg, "WARNING", MB_OK | MB_ICONSTOP);
}

/*=========================================================================*
 *                            panic                                        *
 *                                                                         *
 * Critical error, reset the emulation                                     *
 *=========================================================================*/
void Panic(const char * msg)
{
   Warning(msg);
   do_reset();
}

/* EOF: Error.c */
