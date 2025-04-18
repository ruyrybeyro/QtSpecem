
/* Sna_Save.c : Snapshot and data transfer disk logic -- saving part.
 *
 * Copyright 1996-2019 Rui Fernando Ferreira Ribeiro.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "env.h"
#include "snap.h"

//extern HWND hwndKeyboard;

static char snapname[260]; 

/* built-in software cache logic */
//static int is_eof = 0;

//#define BUFFER_SIZE 2048

/* position of the buffer being read */
//static short BufferPos = BUFFER_SIZE;
/* Bytes read from the buffer */
//static unsigned char * pbBuf;

/* local functions prototypes */
static int sna_save(FILE * fp);
static int snp_save(FILE * fp);
static int snx_save(FILE * fp);
static int sp_save(FILE * fp);
static int sit_save(FILE * fp);
static int z80_save(FILE * fp);
static int zx_save(FILE * fp);
static int prg_save(FILE * fp);
static int ach_save(FILE * fp);
static int tap_save(FILE * fp);
static int rom_save(FILE * fp);
static int scr_save(FILE * fp);
static int sem_save(FILE * fp);
static int save_options(FILE * fp);


/* put a byte in file */
void putbyte(UCHAR c, FILE * fp)
{
   fputc(c, fp);
}

// seek inside the buffer disp bytes from the current position
static void w_seek(FILE * hfp, long disp)
{
   while(disp--)
      putbyte(0, hfp);
}

// close file and buffer
static void w_close(FILE * hfp)
{
   fclose(hfp);
}

extern char *strlwr(char *str); 

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
        { ".sna", SNA_FMT },
        { ".snx", SNX_FMT },
        { ".snp", SNP_FMT },
        { ".z80", Z80_FMT },
        { ".sit", SIT_FMT },
        { ".sp",  SP_FMT  },
        { ".raw", RAW_FMT },
        { ".zx",  ZX_FMT  },
        { ".prg", PRG_FMT },
        { ".ach", ACH_FMT },
        { ".tap", TAP_FMT },
        { ".blk", TAP_FMT },
        { ".rom", ROM_FMT },
        { ".dat", DAT_FMT },
        { ".scr", SCR_FMT },
    { ".sem", SEM_FMT },
        { ".slt", SLT_FMT },
        { ".ini", INI_FMT }
   };

      short i;

      ext = strrchr(file_name, '.');
      if (ext != NULL )
      {
      for(i = 0 ; i < (short)(sizeof(map_ext)/sizeof(struct map)) ; i++)
        if(!strcmp(strlwr(ext),map_ext[i].snap_extension))
        {
           val = map_ext[i].snap_val;
           break;
         }
   }
   return val;
}


/* open file for saving */
void save_sna(const char * file_name)
{
   FILE *  stream = NULL;
   UCHAR tmp;
   int status = 1;
   char snapname[500];

   strcpy(snapname, file_name); 
   tmp = snap_type(snapname);
   if(tmp == 255)
      status = 2;
   else
   {
   //if((tmp == TAP_FMT) || (tmp == BLK_FMT))
   //{
   //   #ifndef _WIN32
   //   if((stream = fopen(file_name, "w")))
   //   #else
   //   if((stream = _lopen(file_name, OF_READWRITE))!= HFILE_ERROR)
   //   #endif
   //	 /* move to end of file */
   //   {
   //	 USHORT size;
   //      USHORT bytes;
   //
   //	 /* hack needed because of dangling 0 at the end of .TAP
   //	   files...
   //	  */
   //	 while((bytes = _lread(stream, &size, 2)) == 2)
   //	 {
   //         if(size)
   //	       _llseek(stream, (long)size, 1);
   //	    else
   //	       break;
   //	 }
   //	 /* We've already read the 0(s), so we want to go
   //	   back
   //	  */
   //	 if((bytes == 1) || (bytes == 2))
   //	    _llseek(stream, (long)(-bytes), 2);
   //   }
   //   else
   //	 stream = NULL;
   //}

   if(stream == NULL)
   {
#if defined(_WIN32) || defined(WIN32)
      stream = fopen(file_name, "wb");
#else
      stream = fopen(file_name, "w");
#endif
   }

   R = (R & ~BIT_7) | R_BIT7;
   build_F();
   if(stream)
   {
      switch(tmp)
      {
	 //case BMP_FMT:
	 //   status = save_dib(stream);
	 //   break;
	 case SNA_FMT:
	    status = sna_save(stream);
	    break;
	 case SNP_FMT:
            status = snp_save(stream);
	    break;
	 case SNX_FMT:
	    status = snx_save(stream);
	    break;
	 case Z80_FMT:
	    status = z80_save(stream);
	    break;
	 case SIT_FMT:
	    status = sit_save(stream);
	    break;
	 case SP_FMT:
	    status = sp_save(stream);
	    break;
	 case ZX_FMT:
	    status = zx_save(stream);
	    break;
	 case PRG_FMT:
	    status = prg_save(stream);
	    break;
	 case ACH_FMT:
	    status = ach_save(stream);
	    break;
	 //case PCX_FMT:
         //   status = save_pcx(stream);
	 //   break;
	 case TAP_FMT:
	 case BLK_FMT:
	    status = tap_save(stream);
	    break;
	 case ROM_FMT:
	    status = rom_save(stream);
	    break;

	 case SCR_FMT:
	    status = scr_save(stream);
	    break;

	 case SEM_FMT:
	    status = sem_save(stream);
            break;

	 case INI_FMT:
	    status = save_options(stream);
	    break;
      }
      w_close(stream);
   }
   }
   if(status)
   {
      if(status == 2)
	 Warning("Can't recognize this extension.");
      else
	 Warning("Could not save data:\ntry another format"
	 	 " or do it later");
   }
}

/* put a word in file -- Intel format */
void put2(USHORT w, FILE * fp)
{
   putbyte(w & 0xFF, fp);
   putbyte(w >> 8, fp);
}

/* put a word in a file -- Motorola format */
static void putword(USHORT w, FILE * fp)
{
   putbyte(w >> 8, fp);
   putbyte(w & 0xFF, fp);
}


static void save_raw(FILE * fp, USHORT inic, USHORT end)
{
   USHORT i;

   for(i=inic ; i < end ; )
      putbyte(readbyte(i++), fp);
   putbyte(readbyte(i), fp);
}

/* save .SNA type of file */
static int sna_save(FILE * fp)
{
   USHORT tmp_val;

   /* PC can't be pushed in ROM */
   if(SP < 0x4002)
      return 1;

   /* save word to be corrupted */
   tmp_val = readword(SP-2);

   push((USHORT)PC);
   putbyte(I, fp);
   put2(HL2, fp);
   put2(DE2, fp);
   put2(BC2, fp);
   put2(AF2, fp);
   put2(HL, fp);
   put2(DE, fp);
   put2(BC, fp);
   put2(IY, fp);
   put2(IX, fp);
   putbyte(IFF1<<2, fp);
   putbyte(R, fp);
   putbyte(F, fp);
   putbyte(A, fp);
   put2(SP, fp);
   putbyte(_IM, fp);
   putbyte(get_sbrdr(), fp);
   save_raw(fp, (USHORT)0x4000, (USHORT)0xFFFF);
   PutPC(pop());
   /* restore word corrupted */
   writeword(SP-2, tmp_val);
   return 0;
}

static int snp_save(FILE * fp)
{
   save_raw(fp, (USHORT)0x4000, (USHORT)0xFFFF);
   put2(AF, fp);
   put2(0, fp);
   put2(BC, fp);
   put2(DE, fp);
   put2(HL, fp);
   put2(PC, fp);
   put2(SP, fp);
   put2(IX, fp);
   put2(IY, fp);
   putbyte(0, fp);
   putbyte(IFF1, fp);
   putbyte(_IM, fp);
   putbyte(R, fp);
   putbyte(I, fp);
   put2(AF2, fp);
   put2(BC2, fp);
   put2(DE2, fp);
   put2(HL2, fp);
   return 0;
}


// save rom file
static int rom_save(FILE * fp)
{
   patch_rom(0);
   save_raw(fp, (USHORT)0, (USHORT)0x3FFF);
   patch_rom(1);
   return 0;
}

// save screen file
static int scr_save(FILE * fp)
{
   save_raw(fp, (USHORT)0x4000, (USHORT)(0x4000+6912-1) );
   return 0;
}

static int sem_save(FILE * fp)
{
   putbyte(5,   fp);
   putbyte('S', fp);
   putbyte('P', fp);
   putbyte('E', fp);
   putbyte('C', fp);
   putbyte('1', fp);
   save_raw(fp, (USHORT)0x4000, (USHORT)0xFFFF );
   put2(AF,  fp);
   put2(BC,  fp);
   put2(DE,  fp);
   put2(HL,  fp);
   put2(AF2, fp);
   put2(BC2, fp);
   put2(DE2, fp);
   put2(HL2, fp);
   put2(PC,  fp);
   put2(SP,  fp);
   put2(IX,  fp);
   put2(IY,   fp);
   putbyte(I, fp);
   putbyte(0, fp);
   putbyte(R, fp);
   put2(0,      fp);
   put2(0,      fp);
   putbyte(0,   fp);
   putbyte(_IM, fp);
   putbyte(0,   fp);
   return 0;
}

/* save .SNX type of file */
static int snx_save(FILE * fp)
{
   USHORT tmp_val;
   USHORT addr, addr2, addr3, cnt, cnt2;
   UCHAR oldbyte, byte;

   /* PC can't be pushed in ROM */
   if(SP < 0x4002)
      return 1;

   /* save word to be corrupted */
   tmp_val = readword(SP-2);

   push((USHORT)PC);
   put2('XS', fp);
   put2('NA', fp);
   put2(774, fp); /* header len */
   putbyte(I, fp);
   put2(HL2, fp);
   put2(DE2, fp);
   put2(BC2, fp);
   put2(AF2, fp);
   put2(HL, fp);
   put2(DE, fp);
   put2(BC, fp);
   put2(IY, fp);
   put2(IX, fp);
   putbyte(IFF1<<2, fp);
   putbyte(R, fp);
   putbyte(F, fp);
   putbyte(A, fp);
   put2(SP, fp);
   putbyte(_IM, fp);
   putbyte(get_sbrdr(), fp);

   //  if1sw      1       switch Interface 1: 0=not emulated, 1=emulated
   putbyte(0, fp);
   //  flashsw    1       switch FLASH:       0=not emulated, 1=emulated
   putbyte(1, fp);
   //  attrsw     1       switch Attributes:  0=no attributes,1=attributes
   putbyte(1, fp);
   //  zy_sw      1       switch keys Z-Y:    Bit 7=0 QWERTY, 1=QWERZ
   //                           +joystick emulation Bit 0,1 00=Kempston
   //                                                       01=IF1/1
   //                                                       10=IF2/2
   //                                                       11=Cursor
   //                           Bit 6 contains ULA-emulation from Version 2.07 on.
   //                                                      (0=off, 1=on).
   putbyte(0x80, fp);
   //       r_sw       1       switch R-register:0=R not emulated, 1=R emulated
   //                               Bit 7 is used as value for the EAR-bit.
   putbyte(1, fp);
   //       int_sw     1       switch interrupt frequency: 0=50Hz, 1=100Hz
   putbyte(1, fp);
   //       rs232sw    1       switch RS232 redirection:  Bit 0=RS232, Bit 1=CENTR.
   putbyte(0, fp);
   //       sndsw      1       switch sound emulation:
   //                             Lower nibble:  0=OFF,1=direct,2=Interrupt
   //                             Higher nibble: frequency 0..4 for mode 2
   putbyte(2, fp);
   //       bordsw     1       switch border emulation: 0=OFF,1=direkt,2=Interrupt
   putbyte(2, fp);
   //       im2hw      1       switch IM2 hardware vector 0..255
   putbyte(0xFF, fp);
   addr = 0x4000u;
   while(addr)
   {
      cnt = 0;
      addr3 = addr2 = addr;
      oldbyte = readbyte(addr2) + 1;
      while((cnt < 4) && addr2)
      {
	 byte = readbyte(addr2);
	 if(oldbyte == byte)
	    cnt++;
	 else
	 {
	    oldbyte = byte;
	    addr3 = addr2;
	    cnt=1;
	 }
	 addr2++;
      }
      if(addr != addr3)
      {
	 cnt2 = addr3 - addr + (!addr2);
	 if(cnt2 < 17)
	    putbyte(0xE0 | (cnt2-1), fp);
	 else
	 {
	    putword(cnt2, fp);
	    putbyte(0, fp);
	 }
	 save_raw(fp, addr, addr3-1 );
      }

      if(cnt>=4)
      {
	 while((readbyte(addr2) == oldbyte) && addr2)
	 {
	    cnt++;
	    addr2++;
	 }
	 /*if(addr != addr3)
	  { */
	 if(cnt < 17)
	    putbyte(0xF0 | (cnt-1), fp);
	 else
	 {
	    putword(cnt, fp);
	    putbyte(0xFF, fp);
	 }
	 putbyte(oldbyte, fp);
      }
      addr = addr2;
   }
   PutPC(pop());

   /* restore word corrupted */
   writeword(SP-2, tmp_val);
   return 0;
}

// save .SIT snapshot
static int sit_save(FILE * hfp)
{
   put2(BC, hfp);
   put2(DE, hfp);
   put2(HL, hfp);
   put2(AF, hfp);
   put2(IX, hfp);
   put2(IY, hfp);
   put2(SP, hfp);
   put2(PC, hfp);
   putbyte(R, hfp);
   putbyte(I, hfp);
   put2(BC2, hfp);
   put2(DE2, hfp);
   put2(HL2, hfp);
   put2(AF2, hfp);
   putbyte(_IM, hfp);
   putbyte(get_sbrdr(), hfp);
   save_raw(hfp, (USHORT)0, (USHORT)0xFFFFu);
   return 0;
}

/* saves .sp type */
static int sp_save(FILE * fp)
{
   /********************************/
   short vgaspec = 0;
   /********************************/

   if(!vgaspec)
   {
      putbyte('S', fp);
      putbyte('P', fp);

      put2((USHORT)0xC000, fp); /* length of data block */
      put2((USHORT)0x4000, fp); /* begin of block */
   }

   put2(BC, fp);
   put2(DE, fp);
   put2(HL, fp);
   put2(AF, fp);
   put2(IX, fp);
   put2(IY, fp);
   put2(BC2, fp);
   put2(DE2, fp);
   put2(HL2, fp);
   put2(AF2, fp);
   putbyte(R, fp);
   putbyte(I, fp);
   put2(SP, fp);
   put2((USHORT)PC, fp);

   /* reserved word */
   put2(0, fp);

   putbyte(get_sbrdr(), fp);

   /* reserved byte */
   putbyte(0, fp);

   putbyte(IFF1|((_IM==2)?2:0), fp);

   /* reserved byte */
   putbyte(0, fp);

   save_raw(fp, (USHORT)0x4000, (USHORT)0xFFFF);

   return 0;
}

// save .ZX snapshot
static int zx_save(FILE *  fp)
{
   /* this format also saves the last ROM 132 bytes */
   save_raw(fp, (USHORT)0x4000u-132u, (USHORT)0xFFFFu);
   w_seek(fp, 0x8EL);
   putbyte(IFF1, fp);
   w_seek(fp, 7L);
   putword(BC,  fp);
   putword(BC2, fp);
   putword(DE,  fp);
   putword(DE2, fp);
   putword(HL,  fp);
   putword(HL2, fp);
   putword(IX,  fp);
   putword(IY,  fp);
   putbyte(I,fp);
   putbyte(R,fp);
   putword(0, fp); putbyte(0, fp);
   putbyte(A2, fp);
   putbyte(0, fp);
   putbyte(A, fp);
   putbyte(0, fp);
   putbyte(F2, fp);
   putbyte(0, fp);
   putbyte(F, fp);
   putword(0, fp);
   putword((USHORT)PC, fp);
   putword(0,  fp);
   putword(SP, fp);
   w_seek(fp, 7L);
   putbyte((_IM == 2)?0xFF:0, fp);
   return 0;
}

/* saves .prg type */
static int prg_save(FILE * fp)
{
   UCHAR tmp1;
   USHORT tmp2, tmp3;

   putbyte(5, fp);
   /* write code to include module name! here! */

   w_seek(fp, 0xDBL);

/*DC*/  put2(IY,  fp);
/*DE*/  put2(IX,  fp);
/*E0*/  put2(DE2, fp);
/*E2*/  put2(BC2, fp);
/*E4*/  put2(HL2, fp);
/*E6*/  put2(AF2, fp);
/*E8*/  put2(DE, fp);
/*EA*/  put2(BC, fp);
/*EC*/  put2(HL, fp);
/*EE*/  putbyte(IFF1, fp);
/*EF*/  putbyte(I, fp);
/*F0 */ put2(SP-6, fp);

   /* save the 5 bytes corrupted */
   tmp1 = readbyte(SP + 1);
   tmp2 = readword(SP + 2);
   tmp3 = readword(SP + 4);

   writebyte(SP+1-6, R);
   writeword(SP+2-6, AF);
   writeword(SP+4-6, (USHORT)PC);

   w_seek(fp, 0x0EL);
   save_raw(fp, (USHORT)0x4000u, (USHORT)0xFFFFu);

   /* restore the 5 bytes corrupted */
   writebyte(SP+1, tmp1);
   writeword(SP+2, tmp2);
   writeword(SP+4, tmp3);
   return 0;
}

// save .ACH format
int ach_save(FILE *  fp)
{
   putbyte(A, fp);                 /* 00 */
   put2(0, fp); putbyte(0, fp);
   putbyte(F, fp);                 /* 04 */
   put2(0, fp); putbyte(0, fp);
   putbyte(B, fp);                 /* 08 */
   put2(0, fp); putbyte(0, fp);
   putbyte(C, fp);                 /* 0C */
   put2(0, fp); putbyte(0, fp);
   putbyte(D, fp);                 /* 10 */
   put2(0, fp); putbyte(0, fp);
   putbyte(E, fp);                 /* 14 */
   put2(0, fp); putbyte(0, fp);
   putbyte(H, fp);                 /* 18 */
   put2(0, fp); putbyte(0, fp);
   putbyte(L, fp);                 /* 1C */
   put2(0, fp); putbyte(0, fp);

   put2((USHORT)PC, fp);           /* 20 */
   w_seek(fp, 6L);
   put2(SP, fp);                   /* 28 */
   w_seek(fp, 0x94L-0x2AL);
   putbyte(R, fp);                 /* 94 */
   w_seek(fp, 0x9CL-0x95L);
   putbyte(get_sbrdr(), fp);       /* 9C */
   w_seek(fp, 0xA4L-0x9DL);
   putbyte(_IM, fp);               /* A4 */
   w_seek(fp, 0xBEL - 0xA5L);
   putbyte(I, fp);                 /* BE */
   putbyte(IFF1, fp);              /* BF */
   w_seek(fp, 0xECL-0xC0L);
   putword(AF2, fp);               /* EC */
   put2(0, fp);
   putword(BC2, fp);               /* F0 */
   put2(0, fp);
   putword(DE2, fp);               /* F4 */
   putword(HL2, fp);               /* F6 */
   put2(IX, fp);           /* F8 */
   put2(0, fp);
   put2(IY, fp);           /* FC */
   put2(0, fp);
   save_raw(fp, (USHORT)0u, (USHORT)0xFFFFu);
   return 0;
}

// save .Z80 format
static int z80_save(FILE * fp)
{
   UCHAR byte;
   /*********************************/
   USHORT compressed = 1;
   /*********************************/

   putbyte(A, fp);
   putbyte(F, fp);
   put2(BC, fp);
   put2(HL, fp);
   put2((USHORT)PC, fp);
   put2(SP, fp);
   putbyte(I, fp);
   putbyte(R&0x7F, fp);
   byte = get_sbrdr() << 1;
   byte |= ((R&0x80)?1:0) | (compressed << 5);
   putbyte(byte, fp);
   put2(DE, fp);
   put2(BC2, fp);
   put2(DE2, fp);
   put2(HL2, fp);
   putbyte(A2, fp);
   putbyte(F2, fp);
   put2(IY, fp);
   put2(IX, fp);
   putbyte(IFF1, fp);
   putbyte(IFF2, fp);
   putbyte(_IM, fp);
   /* bit 2 == 1 --- issue 2 emulation */
   /* bit 6,7 0-Cursor/Protek/AGF joystick
	      1-Sinclair 1 joystick
	      2-Sinclair 2 joystick
    */
   if (compressed)
   {
      USHORT address;
      UCHAR oldbyte;
      USHORT cnt;

      address = 0x4000u;
      oldbyte = readbyte(address++);
      cnt = 1;
      do
      {
	 byte = readbyte(address++);
	 if(address == 1)
	    byte = oldbyte+1; /* hack for last byte */

	 if (((byte != oldbyte) && cnt) || (cnt == 255))
	 {
	    if(((oldbyte == (UCHAR)0xED) && cnt > 1) || (cnt > 4))
	    {
	       put2(0xEDEDu, fp);
	       putbyte(cnt, fp);
	       putbyte(oldbyte, fp);
	       cnt = 1;
	    }
	    else
	    {
	       while(cnt--)
	       {
		  putbyte(oldbyte, fp);
	       }
	       if(address!=1)
	       {
		  if((oldbyte != (UCHAR)0xED))
		     cnt = 1;
		  else
		  {
		     putbyte(byte, fp);
		     cnt = 0;
		  }
	       }
	       else
		  cnt = 0;
	    }
	 }
	 else
	    cnt++;
	 oldbyte = byte;
      }
      while(address != 1);
	 putbyte(0, fp);
      putbyte(0xED, fp);
      putbyte(0xED, fp);
      putbyte(0, fp);
   }
   else
      save_raw(fp, (USHORT)0x4000u, (USHORT)0xFFFFu);
   return 0;
}


// save WSpecEm options
static int save_options(FILE * hfp)
{
   putbyte(Scale   ,     hfp);
   putbyte(bSoundOn,     hfp);
   putbyte(bFlashOn,     hfp);
   putbyte(bModel3 ,     hfp);
   putbyte(ScreenUpdate, hfp);
   //put2(DelayEmVal,      hfp);
   //putbyte((UCHAR)joystate, hfp);
   //putbyte(hwndKeyboard!=NULL, hfp);
   return 0;
}

/* save current state of a emulator as a TAP blocks instead of a snapshot
  --- inspiration taken from the much longed 'Multiface'
 */
static int tap_save(FILE * fp)
{
// must be screen address
// better after 0x4070
//#define START_AD 0x4E28
#define START_AD 0x4100

   short tmp;
   USHORT i;
   UCHAR save_mem[64];
   /* Basic loader block in .TAP binary format */
   static UCHAR loader[] = { 0x13, 0x00,      /* len   */
			     0x00,            /* A reg */
			     0x00,            /* first byte -- code block */
       /* 0x04 */            ' ',' ',' ',' ', /* name - 10 bytes */
			     ' ',' ',' ',' ',
			     ' ',' ',
	  /* 0x0E */         0x4D, 0x00,      /* size of block ? */
	  /* 0x10 */         0x14, 0x00,      /* line to execute by basic */
	  /* 0x12 */         0x4D, 0x00,      /* size of basic */
	  /* 0x14 */         0x00,            /* checksum --altered in prog*/

	  /* 0x15 */         0x4F, 0x00,      /* len */
	  /* 0x17 */         0xFF,            /* A reg */
	  /* 0x18 */         0x00, 0x0A,      /* 10 */
	  /* 0x1a */         0x2C, 0x00,      /* len of line 10 */
	  /* 0x1c */         0xEA,            /* REM */

	  /* 23760 */        0x31,            /* LD  SP,START_AD */
	  /* 0x1e */	     START_AD & 0xFF,
          /* 0x1f */	     START_AD >> 8,
          /* 0x20 */	     0x21, 0xE1, 0x5C,/* LD HL,5CE1 */
	  /* 0x23 */	     0x11, 0x00, 0x40,/* LD DE,4000h */
	  /* 0x26 */	     0x01, 0x1A, 0x00,/* LD BC, LEN  */

          /* 0x29 */	     0xED, 0xB0,      /* LDIR */
	  /* 0x2b */	     0xC3, 0x00, 0x40,/* JP 4000h */
	  /* 0x2e */         0x3E, 0xFF,      /* LD A,FFh */
	  /* 0x30 */         0x37,            /* SCF */
	  /* 0x31 */         0xDD, 0x21, 0x00, 0x5B, /* LD IX,5B00h */
	  /* 0x35 */         0x11, 0x00, 0xA5,/* LD DE, 0xA500 */
	  /* 0x38 */         0xCD, 0x56, 0x05,/* CALL 0556h */
	  /* 0x3b */         0x3E, 0xFF,      /* LD A,FFh */
	  /* 0x3e */         0x37,            /* SCF */
	  /* 0x3f */         0xDD, 0x21, 0x00, 0x40, /* LD IX,4000h */
	  /* 0x43 */         0x11, 0x00, 0x1B,/* LD DE, 0x1B00 */
	  /* 0x46 */         0xC3, 0x56, 0x05,/* JP 0556h */
	  /* 0x49 */         0x00, 0x14,      /* 20 */
	  /* 0x4b */         0x19, 0x00,      /* len of line 20 */
	  /* 0x4d */	     0xE7, 0xB0, '\"',/* BORDER VAL " */
	  /* 0x4f */	     '0', '\"', ':',  /* 0": */
	  /* 0x52 */	     0xDA, 0xB0, '\"',/* PAPER VAL " */
          /* 0x55 */         '0', '\"', ':',  /* 0": */
          /* 0x58 */	     0xFB, ':',
	  /* 0x5a */         0xF9, 0xC0, 0xB0,/* RANDOMIZE USR VAL */
	  /* 0x5d */         '\"','2','3','7',
	  /* 0x61 */         '6','0','\"',
	  /* 0x64 */         0x0D,            /* End of Basic */
	  /* 0x65 */         0x82             /* checksum */
   };

   /* save bytes that will be corrupted */
   for(i = 0 ; i <= 63 ; i++)
      save_mem[i] = readbyte(i+START_AD-2);

   /* write machine code to restore the state of machine when called
    */
   writebyte(START_AD+ 0, 0xF3);     /* DI     */
   writebyte(START_AD+ 1, 0x3E);     /* LD A,x */
   writebyte(START_AD+ 2, I);
   writebyte(START_AD+ 3, 0xED);     /* LD I,A */
   writebyte(START_AD+ 4, 0x47);

   writebyte(START_AD+ 5, 0xED);     /* IM x */
   switch(_IM)
   {
      case 0:
	 tmp = 0x46;
	break;

      case 1:
	 tmp = 0x56;
	break;

      case 2:
	 tmp = 0x5E;
	break;
   }
   writebyte(START_AD+ 6, tmp);

   writebyte(START_AD+ 7, 0x3E);     /* LD  A,x */

   tmp = R;
   /* modify R to compensate to instructions needed to reach
     the last state
    */
   R = R - 0x17;
   if(tmp & (UCHAR)0x80)
      R|= 0x80;
   else
      R&=0x7F;
   writebyte(START_AD+ 8, R);
   R = tmp;

   writebyte(START_AD+ 9, 0xED);     /* LD  R,A */
   writebyte(START_AD+10, 0x4F);


   writebyte(START_AD+11, 0x21);     /* MOV HL,x */
   writeword(START_AD+12, AF2);
   writebyte(START_AD+14, 0xE5);     /* PUSH HL  */
   writebyte(START_AD+15, 0xF1);     /* POP        AF  */
   writebyte(START_AD+16, 0x01);     /* LD BC,x */
   writeword(START_AD+17, BC2);
   writebyte(START_AD+19, 0x21);     /* LD HL,x */
   writeword(START_AD+20, HL2);
   writebyte(START_AD+22, 0x11);     /* LD DE,x */
   writeword(START_AD+23, DE2);
   writebyte(START_AD+25, 0xD9);     /* EXX */
   writebyte(START_AD+26, 0x08);     /* EX AF,AF' */

   writebyte(START_AD+27, 0x3E);     /* LD A,x */
   writebyte(START_AD+28, get_sbrdr() );

   writebyte(START_AD+29, 0xD3);     /*    OUT (254),A */
   writebyte(START_AD+30, 0xFE);

   writebyte(START_AD+31, 0x21);     /* MOV HL,x */
   writeword(START_AD+32, AF);
   writebyte(START_AD+34, 0xE5);     /* PUSH HL  */
   writebyte(START_AD+35, 0xF1);     /* POP        AF  */

   writebyte(START_AD+36, 0x21);     /* LD HL,x */
   writeword(START_AD+37, HL);
   writebyte(START_AD+39, 0x01);     /* LD BC,x */
   writeword(START_AD+40, BC);
   writebyte(START_AD+42, 0x11);     /* LD DE,x */
   writeword(START_AD+43, DE);

   writebyte(START_AD+45, 0xFD);     /* LD IY,x */
   writebyte(START_AD+46, 0x21);
   writeword(START_AD+47, IY);
   writebyte(START_AD+49, 0xDD);     /* LD IX,x */
   writebyte(START_AD+50, 0x21);
   writeword(START_AD+51, IX);

   writebyte(START_AD+53, 0x31);     /* LD SP,x */
   writeword(START_AD+54, SP);

   writebyte(START_AD+56,IFF1?0xFB:0xF3);  /* EI / DI */
   writebyte(START_AD+57, 0xC3);           /* JP x    */
   writeword(START_AD+58, (USHORT)PC);

   writeword(START_AD-2, START_AD);

   /* find name of program */
   for(i = 0 ; snapname[i] ; i++ )
      if((snapname[i] == '.') || (i == 10))
	 break;
      else
	 loader[i+4] = snapname[i];

   /* compute checksum of header of Basic loader */
   tmp = loader[0x04];
   for(i = 0x05 ; i < 0x14 ; i++)
      tmp ^= loader[i];
   loader[0x14] = tmp;

   /* compute checksum of Basic body */
   tmp = loader[0x17];
   for(i = 0x18 ; i < sizeof(loader)-1 ; i++)
      tmp ^= loader[i];
   loader[sizeof(loader)-1] = tmp;

   /* write Basic loader */
   for(i=0 ; i<sizeof(loader); i++)
      putbyte(loader[i], fp);

   /* save .TAP block */
   put2(0xA500u+2u, fp);/* len of block */
   putbyte(0xFF, fp);      /* type of block */
   /* save in file */
   save_raw(fp, (USHORT)0x5B00u, (USHORT)0xFFFFu);
   tmp = 0xFF;             /* valor inicial */
   i = 0x5B00-1;
   do
   {
      tmp^= readbyte(++i);
   }
   while(i != 0xFFFFu);
      putbyte(tmp, fp);

   /* save .TAP block */
   put2(0x1B00u+2u, fp);/* len of block */
   putbyte(0xFF, fp);      /* type of block */
   /* save in file */
   save_raw(fp, (USHORT)0x4000u, (USHORT)0x5AFFu);
   tmp = 0xFF;             /* valor inicial */
   i = 0x4000-1;
   do
   {
      tmp^= readbyte(++i);
   }
   while(i != 0x5AFFu);
   putbyte(tmp, fp);

   /* restore corrupted bytes */
   for(i = 0 ; i <= 63 ; i++)
      writebyte(i+START_AD-2, save_mem[i]);
   return 0;
}

/* EOF : Sna_save.c */
