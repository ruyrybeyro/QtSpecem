/* ExcTranf.c : exchange, block transfer and search block
 *             instructions emulation.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

#define ULONG unsigned long

/*=========================================================================*
 *                            ex_de_hl                                     *
 *=========================================================================*/
void ex_de_hl()
{
   LOCAL USHORT tmp;

   T(4);
   tmp = HL;
   HL  = DE;
   DE  = tmp;
   WZ=HL;
   Q = 0;
}

/*=========================================================================*
 *                            ex_de_ix                                     *
 *=========================================================================*/
void ex_de_ix()
{
   LOCAL USHORT tmp;

   T(8);
   /*tmp = IX; */ /* since in comp.sys.sinclair they disagree... */
   tmp = HL;
   /*IX  = DE; */
   HL  = DE;
   DE  = tmp;
   Q = 0;
}

/*=========================================================================*
 *                            ex_de_iy                                     *
 *=========================================================================*/
void ex_de_iy()
{
   LOCAL USHORT tmp;

   T(8);
   /* tmp = IY; */
   tmp = HL;
   /* IY  = DE; */
   HL  = DE;
   DE  = tmp;
   Q = 0;
}

/*=========================================================================*
 *                            ex_af_af2                                    *
 *=========================================================================*/
void ex_af_af2()
{
   LOCAL USHORT tmp;

   T(4);
   build_F();
   tmp = AF;
   AF  = AF2;
   AF2 = tmp;
   read_F();
   Q = 0;
   /* WZ OK */
}

/*=========================================================================*
 *                            exx                                          *
 *=========================================================================*/
void exx()
{
   LOCAL USHORT tmp;

   T(4);
   tmp = BC;
   BC  = BC2;
   BC2 = tmp;
   tmp = DE;
   DE  = DE2;
   DE2 = tmp;
   tmp = HL;
   HL  = HL2;
   HL2 = tmp;
   tmp = WZ; /* WZ OK */
   WZ  = WZ2;
   WZ2 = tmp;
   Q = 0;
}

/*=========================================================================*
 *                            ex_psp_hl                                    *
 *=========================================================================*/
void ex_psp_hl()
{
   LOCAL USHORT tmp;

   T(19);
   tmp = HL;
   HL  = pop();
   push(tmp);
   WZ=HL; /* WZ OK */
   Q = 0;
}

/*=========================================================================*
 *                            ex_psp_ix                                    *
 *=========================================================================*/
void ex_psp_ix()
{
   LOCAL USHORT tmp;

   T(23);
   tmp = IX;
   IX  = pop();
   push(tmp);
   WZ=IX;
   Q = 0;
}

/*=========================================================================*
 *                            ex_psp_iy                                    *
 *=========================================================================*/
void ex_psp_iy()
{
   LOCAL USHORT tmp;

   T(23);
   tmp = IY;
   IY  = pop();
   push(tmp);
   WZ = IY;
   Q = 0;
}


/*=========================================================================*
 *                            ldi                                          *
 *=========================================================================*/
void ldi()
{
#define ldi \
   LOCAL UCHAR n; \
   \
   T(16); \
   writebyte(DE++, n = readbyte(HL++) ); \
   flags._P = (--BC != 0); \
   flags._H = flags._N = 0; \
   n += A; \
   flags._X = n & (UCHAR)BIT_1; \
   flags._Y = n & (UCHAR)BIT_3; \
   Q = 1;


   ldi
}

/*=========================================================================*
 *                            ldir                                         *
 *=========================================================================*/
void ldir()
{
   /* This block is not really needed ---
   just here to speed up emulation
    */
   //if(BC > 3)
   //{
   //   LOCAL USHORT tmp;
   //
      /* calculate time for next interrupt
       */
    //  if((tmp=(clock_ticks/16))>BC)
	// tmp = BC;

      //while(tmp > 1)
      //{
	 //if((DE - PC) < 3)
	 //   break;
	 /* If LDIR would auto-modify its own opcode, will have to leave
	  */
	 //writebyte(DE++, readbyte(HL++));
	 //BC--;
	 //R += 2;
	 //tmp--;
     // }
   //}
   /* The instruction would work only with the following code
    */
   ldi
   if(BC)
   {
      T(5);
      PC -= 2;
      WZ = PC + 1;
   }
}

#undef ldi

/*=========================================================================*
 *                            ldd                                          *
 *=========================================================================*/
void ldd()
{
#define ldd \
   LOCAL UCHAR n; \
   \
   T(16); \
   writebyte(DE--, n = readbyte(HL--) ); \
   flags._P = (--BC != 0); \
   flags._H = flags._N = 0; \
   n += A; \
   flags._X = n & (UCHAR)BIT_1; \
   flags._Y = n & (UCHAR)BIT_3; \
   Q = 1;

   ldd
}

/*=========================================================================*
 *                            lddr                                         *
 *=========================================================================*/
void lddr()
{
   /* This block is not really needed ---
   just here to speed up emulation
    */
   //if(BC > 3)
   //{
   //   LOCAL USHORT tmp;

      /* calculate time for next interrupt
       */
   //   if((tmp=clock_ticks/16)>BC)
	// tmp = BC;

      //while(tmp > 1)
      //{
	 /* If LDDR would auto-modify its own opcode, will have to leave
	  */
	 //if((PC - DE) < 3)
	   // break;
	 //writebyte(DE--, readbyte(HL--));
	 //BC--;
	 //R += 2;
	 //tmp--;
     // }
   //}
   /* The instruction would work only with the following code
    */
   ldd
   if(BC)
   {
      T(5);
      PC -= 2;
      WZ = PC + 1;
   }
}

#undef ldd

/*=========================================================================*
 *                            cpi                                          *
 *=========================================================================*/
void cpi()
{

#define cpi \
   LOCAL UCHAR tmp, n; \
   \
   T(16); \
   flags._N = 1; \
   \
   /* cp_phl ïinlineï */ \
   flags._H = (((n = readbyte(HL)) & 0xF) > (A & 0xF)); \
   flags._Z = !(tmp = (UCHAR)((UCHAR)A - (UCHAR)n)); \
   flags._S = (tmp & BIT_7); \
   /* -------------------------- */ \
   flags._P = --BC != 0; \
   HL++; \
   flags._X = (tmp - flags._H) & (UCHAR)BIT_1; \
   flags._Y = (tmp - flags._H) & (UCHAR)BIT_3; \
   WZ++; \
   Q = 1;

   cpi
}

// flags._C = (n > A);

/*=========================================================================*
 *                            cpir                                         *
 *=========================================================================*/
void cpir()
{
   cpi
   if(BC && !flags._Z)
   {
      T(5);
      WZ = PC + 1;
      PC -= 2;
   }
}

#undef cpi

/*=========================================================================*
 *                            cpd                                          *
 *=========================================================================*/
void cpd()
{
#define cpd \
   LOCAL UCHAR tmp, n; \
   \
   T(16); \
   flags._N = 1; \
   flags._H = (((n  = readbyte(HL)) & 0xF) > (A & 0xF)); \
   flags._Z = !(tmp = (UCHAR)((UCHAR)A - (UCHAR)n)); \
   flags._S = (tmp & BIT_7); \
   flags._P = --BC != 0; \
   HL--; \
   flags._X = (tmp - flags._H)  & (UCHAR)BIT_1; \
   flags._Y = (tmp - flags._H)  & (UCHAR)BIT_3; \
   WZ--; \
   Q=1;

   cpd
}

// flags._C = (n > A);

/*=========================================================================*
 *                            cpdr                                         *
 *=========================================================================*/
void cpdr()
{
   cpd
   if(BC && !flags._Z)
   {
      T(5);
      WZ = PC + 1;
      PC -= 2;
   }
}

#undef cpd

/* EOF: ExcTranf.c */
