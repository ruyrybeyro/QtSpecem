 /* Ports.c: Z80 I/O - low level routines.
 *
 * Copyright 1991-1996 Rui Fernando Ferreira Ribeiro.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "../h/env.h"

void border_updated(uint8_t color, unsigned long ticks);

/* keeps last out to ula --- current border colour */
static UCHAR out_ula = 190;
static UCHAR Port255 = 0;
/* keeps the current border color */
UCHAR borderColor;
/* Timex screen mode */
USHORT colours_8x1 = 0;
/* Timex screen mode */
USHORT hires = 0;
UCHAR hires_ink;
UCHAR hires_paper;
/* Timex alternate video */
UCHAR alt_video;

UCHAR ULAplus = 0;

/* returns colour of border */
UCHAR get_sbrdr(void)
{
	return out_ula & 7;
}

// LIXO
short  reset = 1;

/*=========================================================================*
 *                            writeport                                    *
 *=========================================================================*/
void writeport(USHORT port, UCHAR value)
{
       static UCHAR is_colour = 0;

	/* ULA -- b4 - ALTF  b3 - MIC b2b1b0 - BORDER
	 */
	  /*if((port & 0xFF ) == 0xF4)
		{
	 reset = 0;
	 reset = 1;
		}
	 else
	  */
if ( (port & 0xFF ) == (USHORT)0x00FF)
       {
          int i;

          Port255 = value;
          if ((alt_video      = value & 1))
             for (i = 0x6000 ; i< 0x7800 ; i++ )
             {  
                int tmp;
                
                tmp = *(mem+i);
                writebyte(i, tmp^255);
                writebyte(i, tmp);
             }

          colours_8x1    = ( ( value & 7 ) == 2 );

	  if((hires       = ( ( value & 7 ) == 6 )))
          {
             hires_ink   = ( value >> 3 ) & 7;
             hires_paper = hires_ink ^ 7;
             resize_host(512, 192);
             for (i = 0x6000 ; i< 0x7800 ; i++ )
             {
                int tmp;
              
                tmp = *(mem+i);
                writebyte(i, tmp^255);
                writebyte(i, tmp);
             } 
          }
	  else
             resize_host(256, 192);

           if (!alt_video)
           for (i = 0x4000 ; i< 0x5800 ; i++ )
           {
              int tmp;

              tmp = *(mem+i);
              writebyte(i, tmp^255);
              writebyte(i, tmp);
           }
       }
    else
    if ( (port == (USHORT)0xBF3F) && ULAplus )
    {
       static UCHAR palettePos;

       if (!is_colour && ((value & 0xC0) == 0 ) )
       {
          palettePos = value & 0x3F;
          is_colour = 1;
       }
       else
       {
          set_palette(palettePos, value);
          is_colour = 0;
       }
    }
    else
    if ( port == (USHORT)0xFF3B )
    {
       ULAplus = value & 1;  
       is_colour = 0;
    }
    else
    if (!(port & 1)) {       // ULA

        // ULA Bits:  0-2:  border
        //              3:  0 activates MIC output
        //              4:  1 activates EAR / Internal Speaker

        borderColor = value & 7;

        border_updated(borderColor, clock_ticks);

        out_ula = value;
       
        // in side, not out
        //if (out_ula & 0x10)
        //   out_ula |= 0x40;
        //else
        //   out_ula &= 0xBF;
                  
    }
}

/* keyboard buffer -- if key pressed corresponding bit = 1 */
UCHAR keybd_buff[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/* Spectrum keyboard :
	 port high byte (low level active)
	 multiples rows can be active at the same time

		  Scan disposition:

			 b3   b4
			 b2   b5
			 b1   b6
			 b0   b7

each scan line has 5 bits (5 keys, low level active):

	 Left side: b0b1b2b3b4       Right side: b4b3b2b1b0

 When multiple scan lines are active, it will be made a 'and' operation between
their values to get the final value ; ghost keys are generated with that
operation


KEYS:

*/

// Kempston joystick:
// Bits 0:  Left
//      1:  Right
//      2:  Down
//      3:  Up
//      4:  Fire
UCHAR joystick = 0x00;      /*     FIRE  UP  DOWN  RIGHT LEFT */
				 /* 000  b4   b3   b2    b1    b0  */

#include <stdio.h>

/*=========================================================================*
 *                            readport                                     *
 *=========================================================================*/
UCHAR readport(USHORT port)
{
	UCHAR value = 0xff;
	static UCHAR lport;

	/* ULA --  b7  b6    b5 b4b3b2b1b0 */
	/*         1   1    EAR  KEY_CODE  */


                    if ((port&0xFF) == 0xFF )
                    {
                       return(Port255);
                    }
	// Just here to run USSPIRITS.Z80
	//if(port == (USHORT)0xFFFD)
	//	return 0;
	lport = ~(UCHAR)port;
	/* Joystick has precedence over keyboard -- Street Hawk, Command4
		Every major emulator got this wrong except Specem (aka Irish emulator)
	 */
        /* joystick */
	if ( lport & 0x20 )
    {
        		value = joystick;
    }
	else 
		/* Port par - ULA */
		if(lport & 1)
		{
	 UCHAR  i = 0, tmp_value, t_val = 1;
	 UCHAR  scan_stat = ~((UCHAR)(port >> 8));
     int key_read = 0;

	 value = 0;

	 // Spectrum keyboard :
	 // Port high byte (low level active)
	 // multiples rows can be active at the same time

	 //      Scan disposition:
	 //      b3   b4
	 //      b2   b5
	 //      b1   b6
	 //      b0   b7

	 // Each scan line has 5 bits (5 keys, low level active):

	 //      Left side: b0b1b2b3b4       Right side: b4b3b2b1b0

	 // When multiple scan lines are active, it will be made a 'and' operation
	 // between their values to get the final value ; ghost keys are generated
	 // with that operation


	 /* scan keyboard -- multiple rows can be active
	  */
	 for(i = 0 ; i < 8 ; i++)
	 {
		 if(t_val & scan_stat)
		 {
			 value |= keybd_buff[i];
                         key_read = 1;
		 }
		 t_val <<= 1;
	 }

	 /* Now it's time to test for ghost keys.
	  */
     if (key_read)
     {
	 tmp_value = value;
	 for(i = 0 ; i < 8 ; i++)
	 {
		 if(tmp_value & *(keybd_buff+i))
			 value |= *(keybd_buff+i);
	  }
	  value = ~value;
          // value &= 0xbf;
     }
         
         /*
         I should implement this also:
         if( !ear_on && mic_on )
         code &= 0xbf;
         where earon = bit 4 of the last OUT to the 0xFE port
         and   micon = bit 3 of the last OUT to the 0xFE port
       */
       if ( out_ula & 0x10 )
          value |= 0x40;
       else
          value &= 0xbf;
     

            
      //if (value == 255)
      //    value = out_ula;
            
	  // And finally express the difference in models 2 and 3:
	  // On Spectrum models previous to model 3 the EAR bit was 1 when
	  // the line was silent.

		//f (!bModel3)
		 //		value ^= BIT_5;
                 } 
		 else
                 {
		 // Any other port - floating bus
		 if((clock_ticks  > 14346) && (clock_ticks < 57246))
                 {
                        value = floating_bus(clock_ticks - 14347);
                 }
         else
		    value = 0xFF;
                 }
	return(value);	// default: 0xff
}

/* EOF: Ports.c */
