/* Sna_Load.c : Snapshot and data transfer disk logic -- loading part.
 *
 * Copyright 1996-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

/*
 * History:
 *      4th April 96:
 *              . modified R register handling
 *              . corrected runaway condition in getbyte() with
 *              corrupted snapshots (would kill emulator)
 *              . Added .SIT and .BLK handling logic
 *
 *      18th April 96:
 *               . ED/FD load lever trap implemented
 */

//#include <windows.h>
//#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include <dir.h>
#include "../h/env.h"
#include "../h/snap.h"

// as defined in WSpecem.c
// A handle that identifies the main window
// extern HWND      hwndApp;

// String with location of our ROM file
extern char      szROMPath[260];

// extern HWND hwndKeyboard;
extern void save_sna(const char * file_name);

/* local functions prototypes */
static int sna_load(FILE * hfp);
static int snx_load(FILE * hfp);
static int snp_load(FILE * hfp);
static int sp_load(FILE  * hfp);
static int z80_load(FILE * hfp);
static int slt_load(FILE * hfp);
static int sit_load(FILE * hfp);
static int raw_load(FILE * hfp);
static int zx_load(FILE * hfp);
static int prg_load(FILE * hfp);
static int ach_load(FILE * hfp);
static int rom_load(FILE * hfp);
static int dck_load(FILE * hfp);
static int dat_load(FILE * hfp);
static int tap_load(FILE * hfp);
static int scr_load(FILE * hfp);
static int sem_load(FILE * hfp);
static int pok_load(FILE * hfp);
static int load_slt_level(FILE * hfp);
static int load_options(FILE * hfp);

/* name of file currently in use */
static char snapname[500];

// Name of tape file on use
static char TapName[500];
// Current file position of tapefile. We'll seek to this position when we
// open it
static long TapFilePos = 0;

// signal EOF
//static int is_eof = 0;

/* address of SLT_info block:
	if true, we're dealing with a SLT file.
 */
long SLT_info_block_ad = 0L;
// It will be active when we are inside the level loader trap. The level
// loader trap is also used to load tape blocks.
static BOOLEAN inside_level_trap = 0;

// pointer to the actual buffer
//static char * pbBuf = NULL;

// real file position, minus the bytes still in the buffer
//long file_pos = 0L;

// check for end-of-file
static int feof_file(FILE * hfp)
{
   return feof(hfp);
}

// get another byte from buffer
static UCHAR getbyte(FILE * hfp)
{
   return fgetc(hfp);
}

// seek in the file, using buffer
static void z_seek(FILE * hfp, long disp)
{
   fseek(hfp, disp, SEEK_CUR);
}

// close file
static void z_close(FILE * hfp)
{
   fclose(hfp);
}

char *strlwr(char *str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return str;
}

/* type of files */
static int snap_type(char * file_name)
{
   char * ext;
   //int flags;
   int val = 255;
    
   struct map
   {
      char * snap_extension;
      short snap_val;
   } map_ext[] =
   {
	{ "sna", SNA_FMT },
	{ "snx", SNX_FMT },
	{ "snp", SNP_FMT },
	{ "z80", Z80_FMT },
	{ "sit", SIT_FMT },
	{ "sp",  SP_FMT  },
	{ "raw", RAW_FMT },
	{ "zx",  ZX_FMT  },
	{ "prg", PRG_FMT },
	{ "ach", ACH_FMT },
	{ "tap", TAP_FMT },
	{ "blk", TAP_FMT },
	{ "rom", ROM_FMT },
	{ "dck", DCK_FMT },
	{ "dat", DAT_FMT },
	{ "scr", SCR_FMT },
    	{ "sem", SEM_FMT },
	{ "slt", SLT_FMT },
	{ "ini", INI_FMT },
        { "pok", POK_FMT }
   };

      short i;
      char ext_cp[4];

      ext = strrchr(file_name, '.');
      if (ext != NULL && (strlen(ext+1)<4) ) 
      {
      strcpy(ext_cp, ext+1);
      for(i = 0 ; i < (short)(sizeof(map_ext)/sizeof(struct map)) ; i++)
	if(!strcmp(strlwr(ext_cp),map_ext[i].snap_extension))
	{
	   val = map_ext[i].snap_val;
	   break;
	 }
   }
   return val;
}

// LIXO
extern short reset;

/* open file for reading */
int open_sna(const char * file_name)
{
   FILE * stream;
   int status = 0;
   char snapcopy[500];
   char *p = snapcopy;
   static int last_was_ach = 0;
   static int not_patched=1;

   strcpy(snapname, file_name);
   // if last file used was ach, reload the last active ROM
   if(last_was_ach == 1)
   {
      int inside_copy = inside_level_trap;

      inside_level_trap = 1;
      last_was_ach++;
      open_sna((char *)szROMPath);
      inside_level_trap = inside_copy;
      last_was_ach = 0;
   }
   // while(*p++ = *file_name++);
   strcpy(p, file_name);

   if(!inside_level_trap)
   {
      char Caption_name[260];
      short i;

      strcpy(Caption_name, "WSpecEm - ");

      i = strlen(snapcopy);
      while(snapcopy[i] != '/')
	 i--;
      strcat(Caption_name, snapcopy + i+1);
      //  put snapshot name to lower case -- but only for
      // display
      //strlwr(&Caption_name[0]+9);
      //SetWindowText(hwndApp, Caption_name);
      SLT_info_block_ad = 0;
   }
   build_F();
   R = (R & 0x7F) | BIT_7;

   #ifndef _WIN32
   if((stream = fopen(file_name, "r")))
   #else
   if(stream = _lopen(snapcopy, OF_READ))
   #endif
   {
      switch(snap_type(snapcopy))
      {
	 case SNA_FMT:
	    status = sna_load(stream);
	    break;

	 case SNX_FMT:
	    status = snx_load(stream);
	    break;

	 case SNP_FMT:
	    status = snp_load(stream);
            break;

	 case Z80_FMT:
	    status = z80_load(stream);
	    break;

	 case SLT_FMT:
	    if(inside_level_trap)
	       status = load_slt_level(stream);
	    else
	    {
	       status = slt_load(stream);
	    }
	    break;

	 case SIT_FMT:
	    status = sit_load(stream);
	    break;

	 case SP_FMT:
	    status = sp_load(stream);
	    break;

	 case RAW_FMT:
	    status = raw_load(stream);
	    break;

	 case ZX_FMT:
	    status = zx_load(stream);
	    break;

	 case PRG_FMT:
	    status = prg_load(stream);
	    break;

	 case ACH_FMT:
	    status = ach_load(stream);
	    if(!status)
	       last_was_ach = 1;
	    break;

	 case ROM_FMT:
	    strcpy(szROMPath, snapcopy);
	    status = rom_load(stream);
	    // LIXO
	    //if(reset)
	    //do_reset();
	    break;

         case DCK_FMT:
            strcpy(szROMPath, snapcopy);
            status = dck_load(stream);
            // LIXO
            //if(reset)
            //do_reset();
            break;

	 case SCR_FMT:
	    status = scr_load(stream);
	    break;

	 case SEM_FMT:
	    status = sem_load(stream);
            break;

	 case DAT_FMT:
	    status = dat_load(stream);
	    break;

	 case POK_FMT:
            status = pok_load(stream);
            break;

	 case TAP_FMT:
        
            if(not_patched)
            {
               not_patched=0;
               patch_rom(1);
            }
              
	    if (!inside_level_trap)
	    {
	       strcpy(TapName, snapcopy);
	       TapFilePos = 0;
	    }
	    else
	    {
	       status = tap_load(stream);
	    }
	    break;

	 case INI_FMT:
	    status = load_options(stream);
	    break;

	 default:
	    status = 2;
            break;
      }

      read_F();
      R_BIT7 = (R & BIT_7);

      if(!inside_level_trap)
	 strcpy(snapname, snapcopy);

      z_close(stream);
      /* if status is false a corrective action must be taken
       */
      if(!inside_level_trap)
      {
         if(status == 2)
	    Warning("Can't recognize this extension.");
	 else
	    if(status == 1)
            {
	       Panic("Corrupted data.");
	    }
      }
   }
   return status;
}

/* get a word from file in Intel format */
static unsigned short get2(FILE * hfp)
{
   unsigned char l=getbyte(hfp);
   unsigned char h=getbyte(hfp);

   return (h<<8)|l;
}

/* get a word from file in big-endian format */
static unsigned short getword(FILE * hfp)
{
   unsigned char h=getbyte(hfp);
   unsigned char l=getbyte(hfp);

   return (h<<8)|l;
}

static int load_raw(FILE * hfp, USHORT inic, USHORT end)
{
   USHORT i;

   for(i=inic ; i < end ; )
      writebyte(i++, getbyte(hfp));
   writebyte(i, getbyte(hfp));
   return 0;
}

/* loads .sna type */
static int sna_load(FILE * hfp)
{
   I=getbyte(hfp);                /* 00 */
   HL2=get2(hfp);                 /* 01 */
   DE2=get2(hfp);                 /* 03 */
   BC2=get2(hfp);                 /* 05 */
   AF2=get2(hfp);                 /* 07 */
   HL=get2(hfp);                  /* 09 */
   DE=get2(hfp);                  /* 0B */
   BC=get2(hfp);                  /* 0D */
   IY=get2(hfp);                  /* 0F */
   IX=get2(hfp);                  /* 11 */
   IFF1=IFF2=(getbyte(hfp)>>2)&1; /* 13 */
   R=getbyte(hfp);                /* 14 */
   F=getbyte(hfp);                /* 15 */
   A=getbyte(hfp);                /* 16 */
   SP=get2(hfp);                  /* 17 */
   _IM=getbyte(hfp);              /* 19 */

   /* get around a problem with some snaps --- perhaps a bug in a old version
     of xzx...or a bad snap?
    */
   if(_IM == 3)
   {
      _IM = 2;
   }
   writeport(254,(getbyte(hfp)&7)|8); /* 1A */
   load_raw(hfp, (USHORT)0x4000u, (USHORT)0xFFFFu);

   /* all .SNA files must have 49149 bytes of lenght and SP must point to RAM
    */
   if(/*(ftell(hfp) != 49179L) || */(SP < 0x4000))
   {
      return 1;
   }

   /* retn(); */
   PutPC(pop());

   /* added for compability with JPP -- now any file that works in JPP
      works here (well-fargo, bounder, batman and enduro racer
      saved at certain places)
      - just putting 0 at word in [SP-2] helps...
      I don't know why, but it works!
    */
   writeword(SP-2,0);

   return 0;
}

static int sit_load(FILE * hfp)
{
   //USHORT i;

   BC=get2(hfp);
   DE=get2(hfp);
   HL=get2(hfp);
   AF=get2(hfp);
   IX=get2(hfp);
   IY=get2(hfp);
   SP=get2(hfp);
   PC=get2(hfp);
   R=getbyte(hfp);
   I=getbyte(hfp);
   BC2=get2(hfp);
   DE2=get2(hfp);
   HL2=get2(hfp);
   AF2=get2(hfp);
   _IM=getbyte(hfp);
   writeport(254,getbyte(hfp));
   rom_load(hfp);
   load_raw(hfp, (USHORT)0x4000u, (USHORT)0xFFFFu);
   return 0;
}

static int snp_load(FILE * hfp)
{
   load_raw(hfp, (USHORT)0x4000u, (USHORT)0xFFFFu);
   AF=get2(hfp);
   (void)get2(hfp);
   BC=get2(hfp);
   DE=get2(hfp);
   HL=get2(hfp);
   PutPC(get2(hfp));
   SP=get2(hfp);
   IX=get2(hfp);
   IY=get2(hfp);
   (void)getbyte(hfp);
   IFF1 = IFF2 = (getbyte(hfp) != 0 );
   _IM=getbyte(hfp);
   R=getbyte(hfp);
   I=getbyte(hfp);
   AF2=getbyte(hfp);
   BC2=getbyte(hfp);
   DE2=getbyte(hfp);
   HL2=getbyte(hfp);
   return 0;
}

static int snx_load(FILE * hfp)
{
   USHORT len;
   USHORT addr;
   UCHAR val, compressed;

   // 0..3    "XSNA"                  Identification for emulator
   if(get2(hfp) != 'XS')
      return 0;
   if(get2(hfp) != 'NA')
      return 0;
   // 4,5     Header lenght           #Bytes in following header
   len = get2(hfp);
   I=getbyte(hfp);                /* 06 */
   HL2=get2(hfp);                 /* 07 */
   DE2=get2(hfp);                 /* 09 */
   BC2=get2(hfp);                 /* 0B */
   AF2=get2(hfp);                 /* 0D */
   HL=get2(hfp);                  /* 0F */
   DE=get2(hfp);                  /* 11 */
   BC=get2(hfp);                  /* 13 */
   IY=get2(hfp);                  /* 15 */
   IX=get2(hfp);                  /* 17 */
   IFF1=IFF2=(getbyte(hfp)>>2)&1; /* 19 */
   R=getbyte(hfp);                /* 1A */
   F=getbyte(hfp);                /* 1B */
   A=getbyte(hfp);                /* 1C */
   SP=get2(hfp);                  /* 1D */
   _IM=getbyte(hfp);              /* 1F */

   writeport(254,(getbyte(hfp)&7)|8); /* 20 */

   //  if1sw      1       switch Interface 1: 0=not emulated, 1=emulated
   //  flashsw    1       switch FLASH:       0=not emulated, 1=emulated
   //  attrsw     1       switch Attributes:  0=no attributes,1=attributes
   //  zy_sw      1       switch keys Z-Y:    Bit 7=0 QWERTY, 1=QWERZ
   //                           +joystick emulation Bit 0,1 00=Kempston
   //                                                       01=IF1/1
   //                                                       10=IF2/2
   //                                                       11=Cursor
   //                           Bit 6 contains ULA-emulation from Version 2.07 on.
   //                                                      (0=off, 1=on).
   //       r_sw       1       switch R-register:0=R not emulated, 1=R emulated
   //                               Bit 7 is used as value for the EAR-bit.
   //       int_sw     1       switch interrupt frequency: 0=50Hz, 1=100Hz
   //       rs232sw    1       switch RS232 redirection:  Bit 0=RS232, Bit 1=CENTR.
   //       sndsw      1       switch sound emulation:
   //                             Lower nibble:  0=OFF,1=direct,2=Interrupt
   //                             Higher nibble: frequency 0..4 for mode 2
   //       bordsw     1       switch border emulation: 0=OFF,1=direkt,2=Interrupt
   //       im2hw      1       switch IM2 hardware vector 0..255

   z_seek(hfp, (long)10);  // 21

   // headlen == ?

   //              2 Bytes count            Number of bytes in block ***)
   //              1 Byte  Flag             Mark if "compressed" block
   //  either (Flag = $ff):
   //              1 Byte  Fill value       Byte to be repeated count times
   // or     (Flag = 0):
   //              Count bytes data         without compression
   //
   // ***) Optimisation, if number of bytes in block <= 16 !!!
   //       The first byte of count contains a value >= $E0, that is to interpret
   //       in the following way:  the lower nibble contains the byte count - 1,
   //       the upper nibble is $E for an uncompressed
   //       block, $F for a compressed block.
   //       The second count byte and the flag byte are not present in this case !
   //
   // The blocks from byte 43 on are repeated for the whole ram dump.
   addr = 0x4000u;
   while(addr)
   {
      len = getbyte(hfp);
      if(len >= (UCHAR)0xE0)
      {
	 compressed = (len >= (UCHAR)0xF0);
	 len = (len & 0xF)+1;
      }
      else
      {
	 len = (len << 8) | getbyte(hfp);
	 compressed = (getbyte(hfp) == (UCHAR)0xFF);
      }
      if(compressed)
      {
	 val = getbyte(hfp);
	 while(len--)
	    writebyte(addr++, val);
      }
      else
	 while(len--)
	    writebyte(addr++, getbyte(hfp) );
   }
   /* retn(); */
   PutPC(pop());

   /* As usual, write 0 on corrupted elements [due to snapshot format]
    */
   writeword(SP-2,0);

   return 0;
}

/* loads .sp type */
static int sp_load(FILE * hfp)
{
   UCHAR byte;
   USHORT start, len;

   if((BC=get2(hfp)) != 'SP') /* 'SP' */   /* 00 */
   {
      start = 0x4000;
      len = 0xC000;
   }
   else
   {
      len=get2(hfp);               /* 02 */
      start=get2(hfp);             /* 04 */
      BC=get2(hfp);                /* 06 */
   }
   DE=get2(hfp);                   /* 08 */
   HL=get2(hfp);                   /* 0A */
   AF=get2(hfp);                   /* 0C */
   IX=get2(hfp);                   /* 0E */
   IY=get2(hfp);                   /* 10 */
   BC2=get2(hfp);                  /* 12 */
   DE2=get2(hfp);                  /* 14 */
   HL2=get2(hfp);                  /* 16 */
   AF2=get2(hfp);                  /* 18 */
   R=getbyte(hfp);                 /* 1A */
   I=getbyte(hfp);                 /* 1B */
   SP=get2(hfp);                   /* 1C */
   PutPC(get2(hfp));               /* 1E */

   /* 2 reserved bytes */
   (void)getbyte(hfp);
   (void)getbyte(hfp);

   writeport(254, getbyte(hfp));   /* 22 */

   /* reserved byte */
   (void)getbyte(hfp);
   byte=getbyte(hfp);              /* 24 */
   IFF1=IFF2=byte&1;
   _IM=(byte&2)?2:1;
   (void)getbyte(hfp);             /* 25 */
   load_raw(hfp, (USHORT)start, (USHORT)((USHORT)(start-1)+(USHORT)len));
   return 0;
}

static int z80_decompress(USHORT address, USHORT len, FILE * hfp)
{
   UCHAR byte;
   USHORT i;

   while(len--)
   {
      byte=getbyte(hfp);

      if((byte==0xed) && len)
      {
	 byte=getbyte(hfp);
	 len--;
	 if(byte==0xed)
	 {
	    /* if len==0 file is trashed */
	    i=getbyte(hfp);
	    len--;
	    /* if len==0 file is trashed */
	    byte=getbyte(hfp);
	    len--;
	    while(i--)
	       writebyte(address++, byte);
	 }
	 else
	 {
	    writebyte(address++, 0xed);
	    writebyte(address++, byte);
	 }
      }
      else
	 writebyte(address++, byte);
   }
   return 0;
}

static int z80_load(FILE * hfp)
{
   USHORT i;
   UCHAR byte;
   USHORT address;
   UCHAR compressed = 0;
   UCHAR page_skipped = 0;

   A=getbyte(hfp);                 /* 00 */
   F=getbyte(hfp);                 /* 01 */
   BC=get2(hfp);                   /* 02 */
   HL=get2(hfp);                   /* 04 */
   PutPC(get2(hfp));               /* 06 */
   SP=get2(hfp);                   /* 08 */
   I=getbyte(hfp);                 /* 0A */
   R=getbyte(hfp) & 0x7F;/* 0B */
   byte=getbyte(hfp);              /* 0C */
   if(byte == 255) byte = 1;
   if(byte & 1)
      R |= (UCHAR)0x80;
   writeport(254, (byte >> 1) & 7);
   if(byte & 0x20) compressed++;
   DE=get2(hfp);                   /* 0D */
   BC2=get2(hfp);                  /* 0F */
   DE2=get2(hfp);                  /* 11 */
   HL2=get2(hfp);                  /* 13 */
   A2=getbyte(hfp);                /* 15 */
   F2=getbyte(hfp);                /* 16 */
   IY=get2(hfp);                   /* 17 */
   IX=get2(hfp);                   /* 19 */
   IFF1=(getbyte(hfp) != 0);       /* 1B */
   IFF2=(getbyte(hfp) != 0);       /* 1C */
   byte=getbyte(hfp);              /* 1D */
   _IM=byte & 3;
   /* bit 2 == 1 --- issue 2 emulation */
   /* bit 6,7 0-Cursor/Protek/AGF joystick
	      1-Sinclair 1 joystick
	      2-Sinclair 2 joystick
    */
   if((USHORT)PC == 0)     /* we have a extended header if PC=0 */
   {
      USHORT len;

      len=get2(hfp);               /* 1E */
      PutPC(get2(hfp));            /* 20 */
      byte=getbyte(hfp); /* hardware mode */
      if(len)
	 z_seek(hfp, (long)(len-3));
      /*-----------------------------------------*/
      /* time to read pages */
      while(!feof_file(hfp))
      {
	 len=get2(hfp);  /* page len */
	 if(feof_file(hfp) || (len == 0))
	    break;
	 /* get 16k-page number */
	 byte=getbyte(hfp);
	 switch(byte)
	 {
	    case 4: address = (USHORT)0x8000;
		break;
	    case 5: address = (USHORT)0xC000;
		break;
	    case 8: address = (USHORT)0x4000;
		break;
	    default:
		/* hardware not implemented ... skiping page....
		 if it's a program wich only uses the 128k chip sound,
		 it'll work... and then, maybe not ; but it's worth a
		 try
		 */
		page_skipped = 1;
		z_seek(hfp, (long)len);
		continue;
	 }
	 if(len == (USHORT)0xFFFF)
	    {
	    short i;

	    // code to read v3.05 .Z80 files
	    for(i = 0; i < 16384; i++)
	       writebyte(address++, getbyte(hfp) );
	    }
	 else
	    z80_decompress(address, len, hfp);
      }
   }
   else
   {
      if(compressed)
      {
	 address = 0x4000;

	 while(address)
	 {
	    byte=getbyte(hfp);

	    if((byte==0xed) && (address != 0xFFFF))
	    {
	       byte=getbyte(hfp);
	       if(byte==0xed)
	       {
		  i=getbyte(hfp);
		  /* if i==0 file is trashed */
		  byte=getbyte(hfp);
		  while(i--)
		  writebyte(address++, byte);
	       }
	       else
	       {
		  writebyte(address++, 0xed);
		  writebyte(address++, byte);
	       }
	    }
	    else
	       writebyte(address++, byte);
	 }
      }
      else
	 load_raw(hfp, (USHORT)0x4000u, (USHORT)0xFFFFu);
   }
   if(page_skipped)
      Warning("This snapshot is requesting non emulated hardware.\n\
If it crashes, reset the emulator.");
   return 0;
}

/* loads .slt type */
static int slt_load(FILE * hfp)
{
   UCHAR byte;
   UCHAR state = 'S';
   UCHAR i;

   z80_load(hfp);
   /* Will have to test if new .SLT extensions here.
    */

   i = 10; /* allow for 10 bytes for searching for .SLT */
   while(i--)
   {
      // implement here a finite-state machine to find SLT keyword
      byte = getbyte(hfp);
      if(state == byte)
      {
	 switch(byte)
	 {
	    case 'S':
	       state = 'L';
	       break;
	    case 'L':
	       state = 'T';
	       break;

	    case 'T':
	       state = 0;
	       break;
	 }
      }
      else
	 state = 'S';
      if(!state)
      {
	 SLT_info_block_ad = ftell(hfp);
	 break;
      }
   }
   if(state)
   {
      Warning("This snapshot is a .Z80, not a .SLT.");
   }
   return 0;
}

/* loads .raw type */
static int raw_load(FILE * hfp)
{
   /* if error in header, could give the message 'should be saved with
	"SAVE *\"b\' CODE 16384, 49152 '
    */
   if(getbyte(hfp)!=3)     /* CODE */      /* 00 */
      return 0;
   if(get2(hfp)!=0xC000)                   /* 01 */
      return 0;
   if(get2(hfp)!=0x4000)                   /* 03 */
      return 0;
   (void)get2(hfp);                        /* 05 */
   (void)get2(hfp);                        /* 07 */

   load_raw(hfp, (USHORT)0x4000u, (USHORT)0xFFFFu);
   PutPC(0x1BF4);  /* entry of next statement */
   AF = 0x5F99;
   BC = 0x1FF0;
   DE = 0x5D0C;
   HL = 0x5d0E;
   AF2 = 0x0044;
   BC2 = 0x1820;
   DE2 = 0x0007;
   HL2 = 0x5CF1;
   IX = 0x03D4;
   IY = 0x5C3A;
   I = 0x3F;
   R = 0;
   _IM = 1;
   IFF1 = IFF2 = 1;
   /* set SP by RAMTOP */
   SP = readword(0x5CB2);
   /* Reset ERR NR to no error */
   writebyte(0x5C3A, 0xFF);
   /* Set border by means of BORDCR */
   writeport(254, (readbyte(0x5C48)&0x38)>>3);
   /* put return adress to MAIN_4 on stack */
   push(0x1303);
   return 0;
}

static int zx_load(FILE * hfp)
{
   UCHAR byte;

   z_seek(hfp, 132L);
   load_raw(hfp, (USHORT)0x4000u, (USHORT)0xFFFFu);
   z_seek(hfp, 0x8EL);
   IFF1 = IFF2 = ((getbyte(hfp)&1)!=0);
   z_seek(hfp, 7L);
   BC  = getword(hfp);
   BC2 = getword(hfp);
   DE  = getword(hfp);
   DE2 = getword(hfp);
   HL  = getword(hfp);
   HL2 = getword(hfp);
   IX  = getword(hfp);
   IY  = getword(hfp);
   I   = getbyte(hfp);
   R   = getbyte(hfp);
   (void)getword(hfp); getbyte(hfp);
   A2  = getbyte(hfp);
   (void)getbyte(hfp);
   A   = getbyte(hfp);
   (void)getbyte(hfp);
   F2  = getbyte(hfp);
   (void)getbyte(hfp);
   F   = getbyte(hfp);
   (void)getword(hfp);
   PutPC(getword(hfp));
   (void)getword(hfp);
   SP  = getword(hfp);
   z_seek(hfp, 5L);
   byte = getbyte(hfp);
   if(byte == (UCHAR)0xFFu)
      _IM = 2;
   else
       _IM = (byte == 1)?2:1;
   return 0;
}

static int prg_load(FILE * hfp)
{
   z_seek(hfp, 0xDCL);
   IY   = get2(hfp);                       /* DC */
   IX   = get2(hfp);                       /* DE */
   DE2  = get2(hfp);                       /* E0 */
   BC2  = get2(hfp);                       /* E2 */
   HL2  = get2(hfp);                       /* E4 */
   AF2  = get2(hfp);                       /* E6 */
   DE   = get2(hfp);                       /* E8 */
   BC   = get2(hfp);                       /* EA */
   HL   = get2(hfp);                       /* EC */
   IFF1 = IFF2 = (getbyte(hfp)& BIT_7);    /* EE */
   I    = getbyte(hfp);                    /* EF */
   _IM   = (I == 0x3F)?1:2;                /* F0 */
   SP   = get2(hfp);                       /* F1 */
   z_seek(hfp, 0x0EL);
   load_raw(hfp, (USHORT)0x4000u, (USHORT)0xFFFFu);
   /* Set border by means of BORDCR */
   writeport(254,(readbyte(0x5C48)& 0x38) >> 3 );
   R    = readbyte(SP + 1);
   AF   = readword(SP + 2);
   PutPC(readword(SP + 4));
   /*writebyte(SP+1, 0);
   writeword(SP+2, 0);
   writeword(SP+4, 0);*/
   SP += 6;
   return 0;
}

static int ach_load(FILE * hfp)
{
   A = getbyte(hfp);               /* 00 */
   (void)getword(hfp); (void)getbyte(hfp);
   F = getbyte(hfp);               /* 04 */
   (void)getword(hfp); (void)getbyte(hfp);
   B = getbyte(hfp);               /* 08 */
   (void)getword(hfp); (void)getbyte(hfp);
   C = getbyte(hfp);               /* 0C */
   (void)getword(hfp); (void)getbyte(hfp);
   D = getbyte(hfp);               /* 10 */
   (void)getword(hfp); (void)getbyte(hfp);
   E = getbyte(hfp);               /* 14 */
   (void)getword(hfp); (void)getbyte(hfp);
   H = getbyte(hfp);               /* 18 */
   (void)getword(hfp); (void)getbyte(hfp);
   L = getbyte(hfp);               /* 1C */
   (void)getword(hfp); (void)getbyte(hfp);
   PutPC(get2(hfp));               /* 20 */
   z_seek(hfp, 6L);
   SP = get2(hfp);                 /* 28 */
   z_seek(hfp, 0x94L-0x2AL);
   R = getbyte(hfp);               /* 94 */
   z_seek(hfp, 0x9CL-0x95L);
   writeport(254, getbyte(hfp));   /* 9C */
   z_seek(hfp, 0xA4L-0x9DL);
   _IM = getbyte(hfp) & 3; /* A4 */
   if(_IM == 3)
      _IM = 0;
   z_seek(hfp, 0xBEL - 0xA5L);
   I = getbyte(hfp);               /* BE */
   IFF1 = IFF2 = (getbyte(hfp)!=0);/* BF */
   z_seek(hfp, 0xECL-0xC0L);
   AF2 = getword(hfp);     /* EC */
   (void)getword(hfp);
   BC2 = getword(hfp);     /* F0 */
   (void)getword(hfp);
   DE2 = getword(hfp);     /* F4 */
   HL2 = getword(hfp);     /* F6 */
   IX  = get2(hfp);        /* F8 */
   (void)getword(hfp);
   IY  = get2(hfp);        /* FC */
   (void)getword(hfp);
			   /* FE */
   rom_load(hfp);
   load_raw(hfp, (USHORT)0x4000u, (USHORT)0xFFFFu);
   return 0;
}

// patch it for LOAD/SAVE routines for .TAP files
// do_it == 1 patch it, do_it = 0 unpatch
void patch_rom(int do_it)
{
   USHORT crc = 0, i;
   // vars to save ROM bytes patched
   static int v0556, v0557, v056c, v056d, v056e, v059e, v05c8, v05c9,v050b,v050c;

   if(do_it)
   {
      /* check if it is Spectrum ROM first!
        If it is a different ROM, we can't patch it
	[Shadow of Unicorn]
       */

      /* patch Spectrum ROM -- patchs for LOAD/SAVE */
      /* verify SA-BYTES */
      for(i = 0x04C2 ; i < 0x053C ; i++)
	 crc += readbyte(i);

      /* verify LD-BYTES */
      for(i = 0x0556 ; i < 0x0605 ; i++)
         crc += readbyte(i);

      /* verify SA-ALL --- shorter CRC cycle to work with
         Spanish and Brazilian translations of ROMs
       */
      for(i = 0x075A ; i < 0x078A ; i++)
	 crc += readbyte(i);

      v0556 = readbyte(0x0556);
      v0557 = readbyte(0x0557);
      v056c = readbyte(0x056C);
      v056d = readbyte(0x056D);
      v056e = readbyte(0x056E);
      v059e = readbyte(0x059E);
      v05c8 = readbyte(0x05C8);
      v05c9 = readbyte(0x05C9);
      v050b = readbyte(0x050B);
      v050c = readbyte(0x050C);

      if( /* crc == 38151 */ 1)
      {
	 /* jumps to relevant routine */

         /* patches for LD-BYTES */
         *(mem+0x0556) = 0xED;  /* install handler */
         *(mem+0x0557) = 0xFB;
	 *(mem+0x056C) = 0xC3;   /* jp */
         *(mem+0x056D) = 0x9f;
	 *(mem+0x056E) = 0x05;

	 *(mem+0x059E) = 0x00;  /* nop */

	 *(mem+0x05c8) = 0xED;  /* install handler */
	 *(mem+0x05c9) = 0xFB;

	 /* handler save */
         *(mem+0x050b) = 0xED;
         *(mem+0x050c) = 0xFB;
      }
   }
   else
   {
      // unpatch ROM
      *(mem+0x056C) = v056c;
      *(mem+0x056D) = v056d;
      *(mem+0x056E) = v056e;

      *(mem+0x059E) = v059e;

      *(mem+0x05C8) = v05c8;
      *(mem+0x05C9) = v05c9;
      *(mem+0x050B) = v050b;
      *(mem+0x050C) = v050c;
   }
}

// load Spectrum ROM
static int rom_load(FILE * hfp)
{
   USHORT i = 0;

   for(i = 0 ; i  < 0x4000 ; i++)
   {
      if(feof_file(hfp))
         break;
      *(mem+i) = getbyte(hfp);
   }
   //patch_rom(1);
   return ((i == 0x4000)?0:3);
}

// load Timex dock cartridge
static int dck_load(FILE * hfp)
{
   USHORT i,j;
   UCHAR  cm,type, subtype = 0;
   UCHAR  map[8];
   USHORT mempos;
  
   type = getbyte(hfp);
   fread(map, 1, 8, hfp);
   if ( ( type == 0xFF ) || (type == 0x00))
   {
      PC = 0;
      cm=getbyte(hfp);
      subtype = getbyte(hfp);
      if ( ((cm == 0) && (subtype == 1)) || ((cm == 2) && (subtype == 2))  ) 
      {
         if (subtype == 1) // LROS
         {
            di();
            SP=0x61FE;
            IX=DE=PC =  getbyte(hfp) + (getbyte(hfp) * 256);
            C=getbyte(hfp);
            Z80_Z = Z80_P = 1;
         }
         else
            if (subtype == 2) // AROS
            {
               if ( cm != 2 ) // machine code
                  return 1;
               PC =  getbyte(hfp) + (getbyte(hfp) * 256);
               (void)getbyte(hfp);
               (void)getbyte(hfp);  // should be 1
               (void)getbyte(hfp);
               (void)getbyte(hfp);
	       mempos = 0x8008;
               while(!feof_file(hfp))
               {
                  *(mem+mempos++) = getbyte(hfp);
               }
               return 0;
            }
            else
               return 1;
      }
      else
         fseek(hfp, -2, SEEK_CUR);
      mempos = 0;
      for (j = 0 ; j < 8 ; j++)
      {
         if (map[j])
            for(i = 0 ; i  < 8192 ; i++)
            {
               if(feof_file(hfp))
                  break;
               *(mem+mempos+i) = getbyte(hfp);
            }
         mempos += 8192;
      }
      if ( PC == 0 )
      {
         do_reset();
         AF = AF2 = SP = 0xFFFF;
      }
   }
   return 0;
}

static int scr_load(FILE * hfp)
{
   load_raw(hfp, (USHORT)0x4000, (USHORT)(0x4000+6912-1) );
   return 0;
}

static int sem_load(FILE * hfp)
{
   z_seek(hfp, 6L);
   load_raw(hfp, (USHORT)0x4000, (USHORT)0xFFFF );
   AF  = get2(hfp);
   BC  = get2(hfp);
   DE  = get2(hfp);
   HL  = get2(hfp);
   AF2 = get2(hfp);
   BC2 = get2(hfp);
   DE2 = get2(hfp);
   HL2 = get2(hfp);
   PC  = get2(hfp);
   SP  = get2(hfp);
   IX  = get2(hfp);
   IY  = get2(hfp);
   I   = getbyte(hfp);
   (void)getbyte(hfp);
   R = getbyte(hfp);
   z_seek(hfp, 5L);
   _IM = getbyte(hfp);
   IFF1 = IFF2 = 0;
   /* Set border by means of BORDCR */
   writeport(254,(readbyte(0x5C48)& 0x38) >> 3 );
   return 0;
}

static int dat_load(FILE * hfp)
{
   USHORT i = HL;

   while(!feof_file(hfp))
   {
      writebyte(i++, getbyte(hfp));
   }
   return 0;
}

static int pok_load(FILE * hfp)
{
   char s[20];
   int  addr;

   while(!feof_file(hfp))
   {  
      if ( fgets(s,20,hfp) != NULL )
      {
         if ( ( (s[0]=='M') || (s[0]=='Z') ) && (s[2] == '8') )
         { 
            addr= atoi(s+4);
            if ( addr >= 16384 )
            {
               writebyte(addr, atoi(s+10));      
            }
         }
      }
   }
   return 0;
}

/* This will load / verify the next block of a .TAP file
  to do:
  --- TAPE rewind implemented... 
  when we arrive at the end of the virtual tape, we'll rewind back
  to start automaticaly
 */
static int tap_load(FILE * hfp)
{
   USHORT len;
   int verify_error = 0;
   int load_block = 1;

   /* this i
   only works with a hack at
       0x05C8	(TWO-BYTE OPCODE EDFB)
    */

   fseek(hfp, TapFilePos, SEEK_SET);
   len = get2(hfp);

   if ( PC == (0x0556+2))
   {
      Z80_Z = ( D == 0xFF);
      ex_af_af2();
      di();
      push(0x053F);
   }

   if(/*F'Z = 0*/ !(AF2 & BIT_6))
   {
      len--;  /* we are already subtracting the space used by the flag
	       */
      H = getbyte(hfp);
      load_block = (AF2>>8) == H;
   }

   if (load_block)
   {
      while(DE && len)
      {
         L = getbyte(hfp);
	 len--;
         H ^= L;
	 if (/*F'C != 0 */ AF2 & BIT_0)	// if not verify
	    writebyte(IX, L);
	 else
	    if(readbyte(IX) != L)
	    {
	       verify_error = 1;
	       break;
            }
	 IX++;
	 DE--;
      }
      if (len && !verify_error)
      {
	 H ^= getbyte(hfp);
	 len--;
      }
   }

   if (len)
      z_seek(hfp, len);

   if(!DE)
      {
         B = 0xB0;
      }

   if(!verify_error)
   {
      AF2 &= (AF2 | BIT_6); /* Z' = 1 */
      A = H;

      /* if A is not 0, we'll have here a tape loading error...
       but we must not interfere with it. Can be a proteccion
        method and not a error, it depends on the caller.
       */
      PC = 0x05E0;
   }
   else        
   {
      Z80_Z = 0;
      ret();
   }

   build_F();

   TapFilePos = ftell(hfp);

   while(getbyte(hfp) == (UCHAR)0)
      if(feof_file(hfp))
      {
	 TapFilePos = 0;
     fseek(hfp, 0L, SEEK_SET);
	 break;
      } 

   if(feof_file(hfp))
   {
      TapFilePos = 0;
      fseek(hfp, 0L, SEEK_SET);
   }
      
   /* force a redraw --- tell it's interrupt time */
   Force_interrupt();
   return 0;
}


void reload_snap(void)
{
   if(snapname[0])
      (void)open_sna(snapname);
}

/* load emulator options */
static int load_options(FILE * hfp)
{
   Scale         = getbyte(hfp);
   bSoundOn      = getbyte(hfp);
   bFlashOn      = getbyte(hfp);
   bModel3       = getbyte(hfp);
   ScreenUpdate  = getbyte(hfp);
   //DelayEmVal    = get2(hfp);
   //joystate      = getbyte(hfp);
   keyb_template = getbyte(hfp);

   if(Scale == 0)
   {
      Scale    = 1;
      bSoundOn = 1;
      bFlashOn = 1;
      bModel3  = 1;
   }
   /* Not an error, routine done to be compatible with .INI files of
     previous versions
    */
   if(ScreenUpdate == 0)
   {
       ScreenUpdate = 3;
       //DelayEmVal = 0;
   }
   return 0;
}

void save_tapp()
{
   FILE * f;
   UCHAR  checksum;

   f=fopen("/tmp/tap.tap", "a");
   //fseek(f, 0, SEEK_END);
   fputc((DE+1) % 256, f);
   fputc((DE+1) / 256, f);
   fputc(L, f);
   checksum = L;
   while(--DE)
   {
      IX++;
      fputc(*(mem+IX),f);
      checksum ^= *(mem+IX);
   }
   fputc(checksum, f); 
   fclose(f);
   Z80_Z = 1;
   Z80_C = 1;
   IX=IX+DE;
   DE=0xFFFF;
   A=0;
   BC=0x000E;
   HL=0x0000;
   ret();
}

// --------------------------------------------------------
void level_loader_trap(void)
{
   char s_copy[260];
   short pos;

   inside_level_trap = 1;
   if( (PC == (0x0556+2)) || (PC == (0x05C8+2)) )
   {
      (void)open_sna(TapName);
   }
   else if (PC == (0x050B+2))
   {
      save_tapp();
   }
   else
   {
      strcpy(s_copy, snapname);

      // If it is a .SLT file, we don't need to find the .DAT files
      if(!SLT_info_block_ad)
      {
	 // Steps to find the .DAT files
	 // Cut extension of snap name and dot
	 pos = strlen(s_copy);
	 while(--pos)
	    if(!pos || (s_copy[pos] == '.')|| (s_copy[pos] == '/')||
		(s_copy[pos] == '\\'))
	       break;
	 if(s_copy[pos] == '.')
	    s_copy[pos] = '\0';
	 // append itoa(A)and append ".DAT"
	 sprintf(s_copy+strlen(s_copy), "%u.DAT", A);
      }
      (void)open_sna(s_copy);
   }
   inside_level_trap = 0;
}

/* LOAD data levels of .SLT files
 */
static int load_slt_level(FILE * hfp)
{
   /* Offset of the level on file */
   long Off_level = 0;
   USHORT data_type;  /* 0 for end of table, 1 for level data */
   long length;       /* length of block being read           */
   USHORT len,        /* length of sougth block               */
	  level;      /* level sougth                         */
   BOOLEAN found = 0; /* level found?                         */

   z_seek(hfp, SLT_info_block_ad);
   while((data_type = get2(hfp)))
   {
      /* avoid a possible infinite loop here
       */
      if(feof_file(hfp))
	 break;

      level  = get2(hfp);
      length = get2(hfp) | (((long)get2(hfp)) << 16);

      if((data_type == 1) && (level == A))
      {
	 found = 1;
	 len = length;
      }
      if(!found)
	 Off_level += length;
   }

   if(found)
   {
      (void)getword(hfp);
      (void)getword(hfp);
      (void)getword(hfp);
      z_seek(hfp, Off_level);
      z80_decompress(HL, len, hfp);
   }
   return 0;
}

/* EOF : Sna_Load.c */
