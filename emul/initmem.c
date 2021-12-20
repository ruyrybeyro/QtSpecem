
/* InitMem.c : Initialize Spectrum memory.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <process.h>
#include <stdlib.h>
#include "../h/env.h"

char szROMPath[260];
char szHELPath[260];

void init_emul()
{
   USHORT i = 0;
   //FILE * f;

   /* Open Z80 emulation with 64Kb of RAM */
   Init_Z80Emu();

   /* init RAM with random values -- just to remember the good old
     days
    */
   srand(NULL);
   // nice visual, but problematic to handle
   for(i=0x4000; i<0x5B00; i++)
      writebyte(i, rand()/256);

   /* Find WSpecem directory (it isn't the default if the programmed was called with a double-
    click in a associated icon
    */
   //GetModuleFileName(hInst, szROMPath, sizeof(szROMPath));
   strcpy(szROMPath, "./ ");

   i = strlen(szROMPath);
   while(szROMPath[i] != '/')
      i--;
	szROMPath[i] = '\0'; /* Directory name */

	strcpy(szHELPath, szROMPath);
	strcat(szHELPath, "/wspecem.hlp" );
	strcat(szROMPath, "/spectrum.rom" );

   /* open rom file --- spectrum.rom */
   //if(open_sna((LPSTR)szROMPath))
   //{
      Panic("Couldn't open file spectrum.rom!");
      /* cleans screen address */
      //for(i=0x4000; i<0xFFFE; i++)
      // writebyte(i, 0);
   //}
   //f = fopen("/Users/ruiribeiro/Desktop/wspecem/setup/data/snaps/tk95.rom", "r");
   //fread(mem, 16384, 1, f);
   //fclose(f);
}

/* EOF: InitMem.c */
