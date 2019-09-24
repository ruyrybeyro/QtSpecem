/* InOut.c: I/O instructions emulation.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
   /* flags._H = flags._N = 0;
   flags._P = parity(A);
   flags._S = ((A) & (UCHAR)BIT_7);
   flags._Z = !(A);
   flags._X = (A) & (UCHAR)BIT_5;
   flags._Y = (A) & (UCHAR)BIT_3; */
}

/*=========================================================================*
 *                            in_r_pc                                      *
 *=========================================================================*/

#define in_r_pc(R) { \
   T(12); \
   flags._H = flags._N = 0; \
   flags._P = parity((R) = readport(BC)); \
   flags._S = ((R) & (UCHAR)BIT_7); \
   flags._Z = !(R); \
   flags._X = (R) & (UCHAR)BIT_5;  \
   flags._Y = (R) & (UCHAR)BIT_3; \
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
   flags._H = flags._N = 0; \
   flags._P = parity(tmp = readport(BC));
   flags._S = (tmp & (UCHAR)BIT_7);
   flags._Z = !tmp;
   flags._X = tmp & (UCHAR)BIT_5;
   flags._Y = tmp & (UCHAR)BIT_3;
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
   flags._H = flags._C = ((int)n + ((C+1) & 255)) > 255; \
   flags._P = parity(((n + ((C+1)&255)) & 7) ^ B); \
   flags._N = (n & 0x80) > 0; \
   Q = 1;
    
   ini
}

/*=========================================================================*
 *                            inir                                         *
 *=========================================================================*/
void inir()
{
   ini
   if(!flags._Z)
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
   flags._H = flags._C = ((int)n + ((C-1) & 255)) > 255; \
   flags._P = parity(((n + ((C-1)&255)) & 7) ^ B); \
   flags._N = (n & 0x80) > 0; \
   Q = 1;
    
   ind
}

//   flags._H = flags._C = ((HL)+((C-1)&255))>255; \
//   flags._P = parity((((HL)+((C-1)&255))&7)^B);

/*=========================================================================*
 *                            indr                                         *
 *=========================================================================*/
void indr()
{
   ind
   if(!flags._Z)
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
   flags._N = (n & 0x80) > 0; \
   flags._H = flags._C = (n + (int)L) > (int)255; \
   flags._P = parity( ((n+L) & 7) ^ B ); \
   Q = 1;

   outi
}

/*=========================================================================*
 *                            otir                                         *
 *=========================================================================*/
void otir()
{
   outi
   if(!flags._Z)
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
   flags._N = (n & 0x80) > 0; \
   flags._H = flags._C = (n + (int)L) > (int)255; \
   flags._P = parity( ((n+L) & 7) ^ B ); \
   Q = 1;

   outd
}

/*=========================================================================*
 *                            otdr                                         *
 *=========================================================================*/
void otdr()
{
   outd
   if(!flags._Z)
   {
      T(5);
      PC -= 2;
   }
}

#undef outd

/* EOF: InOut.C */
