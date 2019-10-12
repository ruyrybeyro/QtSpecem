/* InOut.c: I/O instructions emulation.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */


#include "env.h"

/*=========================================================================*
 *                            in_a_pn                                      *
 *=========================================================================*/
void in_a_pn()
{
   USHORT port;

   T(11);
   A = readport( port = (USHORT)((Getnextbyte()) | ( A << 8) ) );
   WZ = port + 1; /* OK */
   Q = 0;
   /* Z80_H = Z80_N = 0;
   Z80_P = parity(A);
   Z80_S = ((A) & (UCHAR)BIT_7);
   Z80_Z = !(A);
   Z80_X = (A) & (UCHAR)BIT_5;
   Z80_Y = (A) & (UCHAR)BIT_3; */
}

/*=========================================================================*
 *                            in_r_pc                                      *
 *=========================================================================*/

#define in_r_pc(R) { \
   T(12); \
   Z80_H = Z80_N = 0; \
   Z80_P = parity((R) = readport(BC)); \
   Z80_S = ((R) & (UCHAR)BIT_7); \
   Z80_Z = !(R); \
   Z80_X = (R) & (UCHAR)BIT_5;  \
   Z80_Y = (R) & (UCHAR)BIT_3; \
   WZ = BC  + 1; /* OK */ \
   Q = 1; \
}

void in_b_pc() in_r_pc(B);
void in_c_pc() in_r_pc(C);
void in_d_pc() in_r_pc(D);
void in_e_pc() in_r_pc(E);
void in_h_pc() in_r_pc(H);
void in_l_pc() in_r_pc(L);
void in_a_pc() in_r_pc(A);

/* Is it that way? */
void in_pc()
{
   static UCHAR tmp;

   T(12);
   Z80_H = Z80_N = 0; \
   Z80_P = parity(tmp = readport(BC));
   Z80_S = (tmp & (UCHAR)BIT_7);
   Z80_Z = !tmp;
   Z80_X = tmp & (UCHAR)BIT_5;
   Z80_Y = tmp & (UCHAR)BIT_3;
   WZ = BC  + 1 /* OK */ ;
   Q = 1;
}

#undef in_r_pc

/*=========================================================================*
 *                            ini                                          *
 *=========================================================================*/
void ini()
{
#define ini \
   UCHAR n; \
   T(16); \
   writebyte(HL++, n = readport(C) ); \
   WZ = BC + 1; /* OK */ \
   dec_b(); \
   Z80_H = Z80_C = ((int)n + ((C+1) & 255)) > 255; \
   Z80_P = parity(((n + ((C+1)&255)) & 7) ^ B); \
   Z80_N = (n & 0x80) > 0; \
   Q = 1;
    
   ini
}

/*=========================================================================*
 *                            inir                                         *
 *=========================================================================*/
void inir()
{
   ini
   if(!Z80_Z)
   {
      T(5);
      PC -= 2;
   }
}

#undef ini

/*=========================================================================*
 *                            ind                                          *
 *=========================================================================*/
void ind()
{
#define ind \
   UCHAR n; \
   T(16); \
   writebyte(HL--, n = readport(C) ); \
   WZ = BC - 1; /* OK */ \
   dec_b();  \
   Z80_H = Z80_C = ((int)n + ((C-1) & 255)) > 255; \
   Z80_P = parity(((n + ((C-1)&255)) & 7) ^ B); \
   Z80_N = (n & 0x80) > 0; \
   Q = 1;
    
   ind
}

//   Z80_H = Z80_C = ((HL)+((C-1)&255))>255; \
//   Z80_P = parity((((HL)+((C-1)&255))&7)^B);

/*=========================================================================*
 *                            indr                                         *
 *=========================================================================*/
void indr()
{
   ind
   if(!Z80_Z)
   {
      T(5);
      PC -= 2;
   }
}

/*=========================================================================*
 *                            out_pn_a                                     *
 *=========================================================================*/
void out_pn_a()
{
   UCHAR port;
   T(11);
   writeport( (USHORT) ((port=Getnextbyte()) | (A << 8) ), A);
   W =  A;
   Z = port + 1; 
   Q = 0;
}

/*=========================================================================*
 *                            out_pc_r                                     *
 *=========================================================================*/

#define out_pc_r(R) { \
   T(12); \
   writeport(BC, (R)); \
   WZ = BC + 1; /* OK */ \
   Q = 0; \
}

void out_pc_b() out_pc_r(B);
void out_pc_c() out_pc_r(C);
void out_pc_d() out_pc_r(D);
void out_pc_e() out_pc_r(E);
void out_pc_h() out_pc_r(H);
void out_pc_l() out_pc_r(L);
void out_pc_a() out_pc_r(A);

#undef out_pc_r

/* What's this instruction? (ED71)
  In Spectrum appears to write 0 to port 0
  (or any other even port)
*/
void out_pc_0()
{
   T(12);
   writeport(BC, (UCHAR)0);
   WZ = BC + 1; /* OK */
   Q = 0;
}

/*=========================================================================*
 *                            outi                                         *
 *=========================================================================*/
void outi()
{
#define outi \
   UCHAR n; \
   T(12); \
   writeport(C, n = readbyte(HL++) ); \
   dec_b(); \
   WZ = BC + 1; /* OK */ \
   Z80_N = (n & 0x80) > 0; \
   Z80_H = Z80_C = (n + (int)L) > (int)255; \
   Z80_P = parity( ((n+L) & 7) ^ B ); \
   Q = 1;

   outi
}

/*=========================================================================*
 *                            otir                                         *
 *=========================================================================*/
void otir()
{
   outi
   if(!Z80_Z)
   {
      T(5);
      PC -= 2;
   }
}

#undef outi

/*=========================================================================*
 *                            outd                                         *
 *=========================================================================*/
void outd()
{
#define outd \
   T(16); \
   UCHAR n; \
   writeport(C, n = readbyte(HL--) ); \
   dec_b(); \
   WZ = BC - 1; /* OK */ \
   Z80_N = (n & 0x80) > 0; \
   Z80_H = Z80_C = (n + (int)L) > (int)255; \
   Z80_P = parity( ((n+L) & 7) ^ B ); \
   Q = 1;

   outd
}

/*=========================================================================*
 *                            otdr                                         *
 *=========================================================================*/
void otdr()
{
   outd
   if(!Z80_Z)
   {
      T(5);
      PC -= 2;
   }
}

#undef outd

/* EOF: InOut.C */
