/* Shutdown.c: 'shutdown' Z80's emulation.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

/*=========================================================================*
 *                            shutdown_Z80                                 *
 *=========================================================================*/
void shutdown_Z80()
{
   Close_Z80debugger();
   Close_Z80Emu();
}

/* EOF: Shutdown.C */
