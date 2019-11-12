
/* Video.c: Translates hardware screen addresses to coord
 * 	system, fill screen buffer and implements Flash.
 * 
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "../z80core/env.h"


/* buffer caching the Spectrum attributes state */
static char attrib_z80[24][32];

//  Write a byte on Spectrum memory, at the attribute cache,
// and at the WinG buffer if needed
void writebyte(unsigned short adress, unsigned char byte)
{
   unsigned char i;
   UCHAR x, y;      /* coordenadas */
   static unsigned char colour = 0;  /* ultimo atributo   */
   static unsigned char ink = 0;     /* ink   (com flash) */
   static unsigned char paper = 0;   /* paper (com flash) */

   /* each memory acess = 3T */

   /* if ROM defined can't write in lower adresses
    */
   if(adress < 0x4000)
      return; 

   *(mem + adress) = byte;

   // if ( adress == 0x8786 )
   //   TraceOn = 3;

   if(adress < 0x5800) /* If adress lower than attributes adress */
   {
      static USHORT ladress = 0x4000;
      static UCHAR lbyte = 0;

      WindowDirty = 1;
      if((ladress != adress) || (adress == 0x4000))
      { 
         /* put adress in univ x,y coords and read attribs corresponding with
	   that coords i.e. x=0,y=0 ecran top
          */
         y = ((ladress >> 8) & 7) | ((ladress >> 2) & 0x38) |
               ((ladress >> 5) & 0xc0);
      /* if attrib different recalculates ink&paper */

      if(colour != attrib_z80[y>>3][x = (ladress & 0x1F)])
      {
         colour = attrib_z80[y>>3][x];
	 paper = (colour >> 3) & 0xF;
	 ink = (colour & 7) | ((colour >> 3) & 8);
      }

      x <<= 3;
      /* put in host 8 pixels corresponding to Spectrum pixels
       */
      for(i = 0 ; i < 8 ; i++)
      {
         pixel_host(x++, y, (lbyte & 0x80)?ink:paper);
	 lbyte <<= 1;
      }
      ladress = adress;
      }
      lbyte = byte;
   }
   else
      if(adress < 0x5B00) /* If adress in attrib zone */
      {
         unsigned char k;

	 WindowDirty = 1;
	 /* if attrib changed */
	 if(attrib_z80[y = (adress >> 5) & 0x1f][x = adress & 0x1f] != byte)
	 {
	    /* keep it
	     */
	    colour = attrib_z80[y][x] = byte;
	    /* recalculate ink&paper
	     */
	    paper = (colour >> 3) & 0xF;
	    ink = (colour & 7) | ((colour >> 3) & 8);

	    /* Transform text coords in screen adress
	     */
	    adress = ((y & 7)<<5) | ((y & 0x18)<<8) | x | 0x4000;

	    /* put ïtextï coords in graphic coords
	     */
	    y <<= 3;
	    x <<= 3;

	    /* Print corresponding attribut square (8 * 8 pixels)
	     */
	    for(k = 0 ; k < 8 ; k++)
	    {
	       byte = readbyte(adress);
	       for(i = 0 ; i < 8 ; i++)
	       {
	          pixel_host(x++, y, (byte & 0x80)?ink:paper);
		  byte <<= 1;
	       }
	       /* go to next scan
	        */
	       adress += 256;
	       x -= 8;
	       y++;
	    }
	 }
      }
}


// Do the real work for flash happening on the WinG buffer
void do_flash()
{
   UCHAR colour, x1, y1, x, y, k, paper, ink, byte, i;
   USHORT adress;

   // hack for flushing byte buffer
   writebyte(0x4000, readbyte(0x4000) );

   for(y1 = 0 ; y1 < 24 ; y1++)
   {
      for(x1 = 0 ; x1 < 32 ; x1++)
      {
	 x = x1;
	 y = y1;
	 if((colour = attrib_z80[y][x]) & 0x80)
	 {
	    if(!FlashState)
            {
	       paper = (colour >> 3) & 0xF;
	       ink = (colour & 7) | ((colour >> 3) & 8);
	    }
	    else
	    {
	       ink   = (colour >> 3) & 0xF;
	       paper = (colour & 7) | ((colour >> 3) & 8);
            }

	    /* Transform text coords in screen adress
	     */
	    adress = ((y & 7)<<5) | ((y & 0x18)<<8) | x | 0x4000;

	    /* put text coords in graphic coords
	     */
	    y <<= 3;
	    x <<= 3;

	    /* Print corresponding attribut square (8 * 8 pixels)
	     */
	    for(k = 0 ; k < 8 ; k++)
	    {
	       byte = readbyte(adress);
	       for(i = 0 ; i < 8 ; i++)
	       {
	          pixel_host(x++, y, (byte & 0x80)?ink:paper);
		  byte <<= 1;
	       }
	       /* go to next scan
	        */
	       adress += 256;
	       x -= 8;
	       y++;
	    }
         }
      }	 
   }
}

/* Video.c */
