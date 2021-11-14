
/* Video.c: Translates hardware screen addresses to coord
 * 	system, fill screen buffer and implements Flash.
 * 
 * Copyright 1991-2021 Rui Fernando Ferreira Ribeiro.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "../h/env.h"

extern USHORT colours_8x1;
extern USHORT hires;
USHORT FullScreen=0;
extern UCHAR hires_ink;
extern UCHAR hires_paper;
/* Timex alternate video */
extern UCHAR alt_video;
extern UCHAR ULAplus;

/* buffer caching the Spectrum attributes state */
static char attrib_z80[24][32];

// #include <stdio.h>

//  Write a byte on Spectrum memory, at the attribute cache,
// and at the video buffer if needed
void writebyte(unsigned short adress, unsigned char byte)
{
   unsigned char i;
   USHORT x, y;      /* coords */
   static unsigned char colour = 0;  /* last attribute     */
   static unsigned char ink = 0;     /* ink   (with flash) */
   static unsigned char paper = 0;   /* paper (with flash) */

   /* each memory acess = 3T */

   /* if ROM defined can't write in lower adresses
    */
   if(adress < 0x4000)
      return; 
   /*
   if ( (adress > readword(0x8002)) && (adress < (readword(0x8002)+793)))
   {
      FILE * out;

      out = fopen("/tmp/out", "a");
      fprintf(out, "%04X\n", PC );
      fclose(out);
   }
   */


   // if ( adress == 0xFDBB )
   //   TraceOn = 3;

   *(mem + adress) = byte;
   if( ( (adress >= 0x4000 && (adress < 0x5800) ) && !hires && !alt_video)
||
 ( alt_video && (adress >= 0x6000 ) && (adress < 0x7800) ) 
) /* If address lower than attributes adress */
   {
      static USHORT ladress = 0;
      static UCHAR lbyte = 0;

      WindowDirty = 1;
      if( ladress != adress )
      { 
         /* put adress in univ x,y coords and read attribs corresponding with
	   that coords i.e. x=0,y=0 ecran top
          */
         y = ((ladress >> 8) & 7) | ((ladress >> 2) & 0x38) |
               ((ladress >> 5) & 0xc0);
      /* if attrib different recalculates ink&paper */

      if (!colours_8x1)
      {
         if(colour != attrib_z80[y>>3][x = (ladress & 0x1F)])
         {
            colour = attrib_z80[y>>3][x];
            if (!ULAplus)
            {
	       paper = (colour >> 3) & 0xF;
	       ink = (colour & 7) | ((colour >> 3) & 8);
            }
            else
            {
               // ULAplus
               // paper_colour = (FLASH * 2 + BRIGHT) * 16 + PAPER + 8
               // ink_colour   = (FLASH * 2 + BRIGHT) * 16 + INK
               paper = ((colour & 0xC0) >> 2) | ((colour >> 3)&7) | 8;
               ink   = ((colour & 0xC0) >> 2) | (colour&7);
            }
         }
      }
      else
      {
         // improve later
         x = (ladress & 0x1F);
         //if(colour != *(mem + (ladress | 0x2000)))
         //{  
            colour = *(mem + (ladress | 0x2000));
            if (!ULAplus)
            {
               paper = (colour >> 3) & 0xF;
               ink = (colour & 7) | ((colour >> 3) & 8);
            }
            else
            {
               // ULAplus
               // paper_colour = (FLASH * 2 + BRIGHT) * 16 + PAPER + 8
               // ink_colour   = (FLASH * 2 + BRIGHT) * 16 + INK
               paper = ((colour & 0xC0) >> 2) | ((colour >> 3)&7) | 8;
               ink   = ((colour & 0xC0) >> 2) | (colour&7);
            }
         //}
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
      if ( ( (!colours_8x1) && (!hires) && (adress < 0x5B00) ) ||
           ( alt_video && (adress >= 0x7800 ) && (adress < 0x7B00) ) 
         )
         /* If adress in attrib zone */
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
            if (!ULAplus)
            {  
               paper = (colour >> 3) & 0xF;
               ink = (colour & 7) | ((colour >> 3) & 8);
            }
            else
            {  
               // ULAplus
               // paper_colour = (FLASH * 2 + BRIGHT) * 16 + PAPER + 8
               // ink_colour   = (FLASH * 2 + BRIGHT) * 16 + INK
               paper = ((colour & 0xC0) >> 2) | ((colour >> 3)&7) | 8;
               ink   = ((colour & 0xC0) >> 2) | (colour&7);
            }

	    /* Transform text coords in screen adress
	     */
	    adress = ((y & 7)<<5) | ((y & 0x18)<<8) | x | 0x4000;

	    /* put text coords in graphic coords
	     */
	    y <<= 3;
	    x <<= 3;

	    /* Print corresponding attribute square (8 * 8 pixels)
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
   else
   if( colours_8x1 && (adress >= 0x6000) && (adress < 0x7800 ) )
   {
       WindowDirty = 1;
       // quick hack, improve later on
       writebyte(adress ^ 0x2000, *(mem+(adress ^ 0x2000)));
   }
   // else
   if ( hires && ((adress < 0x5800) || ((adress >= 0x6000) && (adress < 0x7800 ))  ) )
   {
       int oddcol;
       USHORT addrcp;

       ink = 0;
       paper = 7;
       WindowDirty = 1;
       if ( adress & 0x2000 )
       {
          oddcol = 8;
          addrcp = adress ^ 0x2000;
       }
       else
       {
          oddcol = 0;
          addrcp = adress;
       }

       y = ((addrcp >> 8) & 7) | ((addrcp >> 2) & 0x38) |
               ((addrcp >> 5) & 0xc0);
       x = ( (addrcp & 0x1F) << 3 );
       
       if ( x > 7)
          x = x * 2;

       x = x + oddcol;

       for(i = 0 ; i < 8 ; i++)
       {
           pixel_host(x++, y, (byte & 0x80)?hires_ink:hires_paper);
           byte <<= 1;
        } 
   }
}


// Do the real work for flash happening on the video buffer
void do_flash()
{
   UCHAR colour, x1, y1, x, y, k, paper, ink, byte, i;
   USHORT adress;

   if (colours_8x1 || hires || ULAplus)
      return;

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

	    /* Print corresponding attribute square (8 * 8 pixels)
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
