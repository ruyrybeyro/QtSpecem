/* Math8Bit.c: Z80's 8 bit arithmetic and logic instructions.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

/*=========================================================================*
 *                            add_a_r                                      *
 *=========================================================================*/

#define add_a_r(r, TS) { \
   LOCAL UCHAR tmp; \
   \
   T(TS); \
   flags._N = 0; \
   tmp = (UCHAR)(A + (r)); \
   flags._C = (A + (unsigned int)(r)) >= 0x100; \
   flags._H = ( ((A & (UCHAR)0xF) + ((r) & (UCHAR)0xF) ) > (UCHAR)0xF ); \
   flags._P = (~A^(r)) & (UCHAR)BIT_7 & (tmp^A); \
   flags._S = ((A = tmp) & (UCHAR)BIT_7); \
   flags._Z = !A; \
   flags._X = A & (UCHAR)BIT_5; \
   flags._Y = A & (UCHAR)BIT_3; \
   Q=1; \
}

// flags._H = ((tmp & (UCHAR)0x0f) < (A & (UCHAR)0x0f));
// flags._P = (((A ^ ~(r)) & (A ^ tmp)) > (UCHAR)0x7f);

void add_a_b()  add_a_r(B, 4);
void add_a_c()  add_a_r(C, 4);
void add_a_d()  add_a_r(D, 4);
void add_a_e()  add_a_r(E, 4);
void add_a_h()  add_a_r(H, 4);
void add_a_Ix() add_a_r(HX, 8);
void add_a_Iy() add_a_r(HY, 8);
void add_a_l()  add_a_r(L, 4);
void add_a_iX() add_a_r(LX, 8);
void add_a_iY() add_a_r(LY, 8);
void add_a_a()  add_a_r(A, 4);

#undef add_a_r

/*=========================================================================*
 *                            add_a_r                                      *
 *=========================================================================*/
#define add_a_pss(ss, TS) \
{ \
   LOCAL UCHAR tmp, n; \
   \
   T(TS); \
   n = ss; \
   flags._N = 0; \
   tmp = (UCHAR)(A + (UCHAR)n); \
   flags._C = (A + (unsigned int)n) >= 0x100; \
   flags._H = ( ((A & (UCHAR)0xF) + (n & (UCHAR)0xF) ) > (UCHAR)0xF ); \
   flags._P = (~A^n) & (UCHAR)BIT_7 & (tmp^A); \
   flags._S = ((A = tmp) & (UCHAR)BIT_7); \
   flags._Z = !A; \
   flags._X = A & (UCHAR)BIT_5; \
   flags._Y = A & (UCHAR)BIT_3; \
   Q = 1; \
}

// flags._P = ((A ^ ~(n) & (A ^ tmp)) > (UCHAR)0x7f);
// flags._H = ((tmp & (UCHAR)0x0f) < (A & (UCHAR)0x0f));

void add_a_n() add_a_pss(Getnextbyte(), 7);

/*=========================================================================*
 *                            add_a_phl                                    *
 *=========================================================================*/
void add_a_phl() add_a_pss(readbyte(HL),  7);
void add_a_pix() add_a_pss(readbyte(pIX), 19);
void add_a_piy() add_a_pss(readbyte(pIY), 19);

#undef add_a_pss

/*=========================================================================*
 *                            adc_a_r                                      *
 *=========================================================================*/

#define adc_a_r(r, TS) { \
   LOCAL UCHAR tmp; \
   \
   T(TS); \
   flags._N = 0; \
   tmp = (UCHAR)(A + (r) + (UCHAR)(flags._C != 0)); \
   flags._C = ((unsigned int)A + (unsigned int)(r) + (unsigned int)(flags._C != 0) ) >= 0x100; \
   flags._H = (((A ^ r ^ tmp) & 0x10) != 0); \
   flags._P = (((A ^ ~(r)) & (A ^ tmp)) > (UCHAR)0x7f); \
   flags._S = ((A = tmp) & (UCHAR)BIT_7); \
   flags._Z = !A; \
   flags._X = A & (UCHAR)BIT_5; \
   flags._Y = A & (UCHAR)BIT_3; \
   Q = 1; \
}

// flags._H = (((A ^ (r) ^ tmp) & 0x10) != 0);
// flags._P = (((A ^ ~(r)) & (A ^ tmp)) > 0x7f);
//// flags._H = ((tmp & (UCHAR)0x0f) <= (A & (UCHAR)0x0f));


void adc_a_b()  adc_a_r(B,  4);
void adc_a_c()  adc_a_r(C,  4);
void adc_a_d()  adc_a_r(D,  4);
void adc_a_e()  adc_a_r(E,  4);
void adc_a_h()  adc_a_r(H,  4);
void adc_a_Ix() adc_a_r(HX, 8);
void adc_a_Iy() adc_a_r(HY, 8);
void adc_a_l()  adc_a_r(L,  4);
void adc_a_iX() adc_a_r(LX, 8);
void adc_a_iY() adc_a_r(LY, 8);
void adc_a_a()  adc_a_r(A,  4);

#undef adc_a_r

/*=========================================================================*
 *                            adc_a_n                                      *
 *=========================================================================*/
#define adc_a_pss(ss, TS) \
{ \
   LOCAL UCHAR tmp, n; \
   \
   T(TS); \
   flags._N = 0; \
   n = ss; \
   tmp = (UCHAR)(A + n + (UCHAR)(flags._C != 0)); \
   flags._C = ((unsigned int)A + (unsigned int) n + (unsigned int)(flags._C != 0) ) >= 0x100; \
   flags._H = (((A ^ n ^ tmp) & 0x10) != 0); \
   flags._P = (((A ^ ~(n)) & (A ^ tmp)) > 0x7f); \
   flags._S = ((A = tmp) & (UCHAR)BIT_7); \
   flags._Z = !A; \
   flags._X = A & (UCHAR)BIT_5; \
   flags._Y = A & (UCHAR)BIT_3; \
   Q = 1; \
}

void adc_a_n() adc_a_pss(Getnextbyte(), 7);

/*=========================================================================*
 *                            adc_a_phl                                    *
 *=========================================================================*/
void adc_a_phl() adc_a_pss(readbyte(HL),   7);
void adc_a_pix() adc_a_pss(readbyte(pIX), 19);
void adc_a_piy() adc_a_pss(readbyte(pIY), 19);

#undef add_a_pss

/*=========================================================================*
 *                            sub_r                                        *
 *=========================================================================*/

#define sub_r(r, TS) { \
   LOCAL UCHAR tmp; \
   \
   T(TS); \
   flags._N = 1; \
   flags._Z = !(tmp = (UCHAR)(A - (r))); \
   flags._H = ((tmp & 0x0f) > (A & 0x0f)); \
   flags._C = ((signed int)A - (signed int)(r) ) < 0; \
   flags._P = (A^(r)) & (UCHAR)BIT_7 & (tmp^A); \
   flags._S = ((A = tmp) & (UCHAR)BIT_7); \
   flags._X = A & (UCHAR)BIT_5; \
   flags._Y = A & (UCHAR)BIT_3; \
   Q = 1; \
}

// flags._C = ((r) > A);

void sub_b()  sub_r(B,  4);
void sub_c()  sub_r(C,  4);
void sub_d()  sub_r(D,  4);
void sub_e()  sub_r(E,  4);
void sub_h()  sub_r(H,  4);
void sub_Ix() sub_r(HX, 8);
void sub_Iy() sub_r(HY, 8);
void sub_l()  sub_r(L,  4);
void sub_iX() sub_r(LX, 8);
void sub_iY() sub_r(LY, 8);
void sub_a()  sub_r(A,  4);

/* void sub_a()
{
   T(8);
   flags._Z = flags._N = 1;
   A = flags._X = flags._Y = flags._H = flags._C = flags._P = flags._S = 0;
} */

#undef sub_r

/*=========================================================================*
 *                            sub_n                                        *
 *=========================================================================*/
#define sub_pss(ss, TS) \
{ \
   LOCAL UCHAR tmp, n; \
   \
   T(7); \
   flags._N = 1; \
   n = ss; \
   flags._Z = !(tmp = (UCHAR)(A - n)); \
   flags._H = ((tmp & 0x0f) > (A & 0x0f)); \
   flags._C = ((signed int)A - (signed int)(n) ) < 0; \
   flags._P = (A^n) & (UCHAR)BIT_7 & (tmp^A); \
   flags._S = ((A = tmp) & (UCHAR)BIT_7); \
   flags._X = A & (UCHAR)BIT_5; \
   flags._Y = A & (UCHAR)BIT_3; \
   Q = 1; \
}

// flags._C = (n > A);

void sub_n() sub_pss(Getnextbyte(), 7);

/*=========================================================================*
 *                            sub_phl                                      *
 *=========================================================================*/
void sub_phl() sub_pss(readbyte(HL),   7);
void sub_pix() sub_pss(readbyte(pIX), 19);
void sub_piy() sub_pss(readbyte(pIY), 19);

#undef sub_pss

/*=========================================================================*
 *                            sbc_a_r                                      *
 *=========================================================================*/

#define sbc_a_r(r, TS) { \
   LOCAL UCHAR tmp; \
   \
   T(TS); \
   flags._N = 1; \
   flags._Z = !(tmp = (UCHAR)(A - (r) - (UCHAR)(flags._C != 0))); \
   flags._H = (((A ^ (r) ^ tmp) & 0x10) != 0); \
   flags._C = ((signed int)A - (signed int)(r) -  (flags._C != 0) ) < 0; \
   flags._P = (((A ^ (r)) & (A ^ tmp)) > (UCHAR)0x7f); \
   flags._S = ((A = tmp) & (UCHAR)BIT_7); \
   flags._X = A & (UCHAR)BIT_5; \
   flags._Y = A & (UCHAR)BIT_3; \
   Q = 1; \
}

// flags._C = ((r - (UCHAR)(flags._C != 0)) > A) || (tmp > A);

void sbc_a_b()  sbc_a_r(B,  4);
void sbc_a_c()  sbc_a_r(C,  4);
void sbc_a_d()  sbc_a_r(D,  4);
void sbc_a_e()  sbc_a_r(E,  4);
void sbc_a_h()  sbc_a_r(H,  4);
void sbc_a_Ix() sbc_a_r(HX, 8);
void sbc_a_Iy() sbc_a_r(HY, 8);
void sbc_a_l()  sbc_a_r(L,  4);
void sbc_a_iX() sbc_a_r(LX, 8);
void sbc_a_iY() sbc_a_r(LY, 8);
void sbc_a_a()  sbc_a_r(A,  4);

/* void sbc_a_a()
{
   T(4);
   flags._N = 1;
   flags._P = 0;
   A = (flags._Z = !(flags._X = flags._Y = flags._H = flags._C) ) ?
       0 : (UCHAR)0xFF;
} */

#undef sbc_a_r
/*=========================================================================*
 *                            sbc_a_n                                      *
 *=========================================================================*/
#define sbc_a_pss(ss, TS) \
{ \
   LOCAL UCHAR tmp, n; \
   \
   T(TS); \
   flags._N = 1; \
   n = ss; \
   flags._Z = !(tmp = (UCHAR)(A - n - (flags._C != 0))); \
   flags._H = (((A ^ (n) ^ tmp) & 0x10) != 0); \
   flags._C = ((signed int)A - (signed int)n -  (flags._C != 0) ) < 0; \
   flags._P = (((A ^ (n)) & (A ^ tmp)) > (UCHAR)0x7f); \
   flags._S = ((A = tmp) & (UCHAR)BIT_7); \
   flags._X = A & (UCHAR)BIT_5; \
   flags._Y = A & (UCHAR)BIT_3; \
   Q = 1; \
}

// flags._C = (n > A)||(tmp>A);

void sbc_a_n() sbc_a_pss(Getnextbyte(), 7);

/*=========================================================================*
 *                            sbc_a_phl                                    *
 *=========================================================================*/
void sbc_a_phl() sbc_a_pss(readbyte(HL),  7);
void sbc_a_pix() sbc_a_pss(readbyte(pIX), 19);
void sbc_a_piy() sbc_a_pss(readbyte(pIY), 19);

#undef sbc_a_pss

/*=========================================================================*
 *                            and_r                                        *
 *=========================================================================*/

#define and_r(r, TS) { \
   T(TS); \
   flags._H = 1; \
   flags._N = flags._C = 0; \
   flags._Z = !(A=A & (r)); \
   flags._P = parity(A); \
   flags._S = A & (UCHAR)BIT_7; \
   flags._X = A & (UCHAR)BIT_5; \
   flags._Y = A & (UCHAR)BIT_3; \
   Q = 1; \
}

void and_b()  and_r(B,  4);
void and_c()  and_r(C,  4);
void and_d()  and_r(D,  4);
void and_e()  and_r(E,  4);
void and_h()  and_r(H,  4);
void and_Ix() and_r(HX, 8);
void and_Iy() and_r(HY, 8);
void and_l()  and_r(L,  4);
void and_iX() and_r(LX, 8);
void and_iY() and_r(LY, 8);
void and_a()  and_r(A,  4);

/* void and_a()
{
	T(4);
	flags._H = 1;
	flags._N = flags._C = 0;
	flags._S = A & (UCHAR)BIT_7;
	flags._Z = !A;
	flags._P = parity(A);
	flags._X = A & (UCHAR)BIT_5;
	flags._Y = A & (UCHAR)BIT_3;
} */

/*=========================================================================*
 *                            and_n                                        *
 *=========================================================================*/
void and_n() and_r(Getnextbyte(), 7);

/*=========================================================================*
 *                            and_phl                                      *
 *=========================================================================*/
void and_phl() and_r(readbyte(HL), 7);
void and_pix() and_r(readbyte(pIX), 19);
void and_piy() and_r(readbyte(pIY), 19);

#undef and_r

/*=========================================================================*
 *                            or_r                                         *
 *=========================================================================*/

#define or_r(r,TS) { \
	T(TS); \
	flags._N = flags._C = flags._H = 0; \
	flags._Z = !(A = A | (r)); \
	flags._P = parity(A); \
	flags._S = A & (UCHAR)BIT_7; \
	flags._X = A & (UCHAR)BIT_5; \
	flags._Y = A & (UCHAR)BIT_3; \
        Q = 1; \
}


void or_b() or_r(B, 4);
void or_c() or_r(C, 4);
void or_d() or_r(D, 4);
void or_e() or_r(E, 4);
void or_h() or_r(H, 4);
void or_Ix() or_r(HX, 8);
void or_Iy() or_r(HY, 8);
void or_l() or_r(L, 4);
void or_iX() or_r(LX, 8);
void or_iY() or_r(LY, 8);
void or_a() or_r(A, 4);

/* void or_a()
{
	T(4);
	flags._N = flags._C = flags._H = 0;
	flags._S = (A & (UCHAR)BIT_7);
	flags._Z = !A;
	flags._P = parity(A);
	flags._X = A & (UCHAR)BIT_5;
	flags._Y = (UCHAR)BIT_3;
} */

/*=========================================================================*
 *                            or_n                                         *
 *=========================================================================*/
void or_n() or_r(Getnextbyte(), 7);

/*=========================================================================*
 *                            or_phl                                       *
 *=========================================================================*/
void or_phl() or_r(readbyte(HL), 7);
void or_pix() or_r(readbyte(pIX), 19);
void or_piy() or_r(readbyte(pIY), 19);

#undef or_r

/*=========================================================================*
 *                            xor_r                                        *
 *=========================================================================*/

#define xor_r(r, TS) { \
	T(TS); \
	flags._N = flags._C = flags._H = 0; \
	flags._Z = !(A = A ^ (r)); \
	flags._P = parity(A); \
	flags._S = A & (UCHAR)BIT_7; \
	flags._X = A & (UCHAR)BIT_5; \
	flags._Y = A & (UCHAR)BIT_3; \
        Q = 1; \
}

void xor_b() xor_r(B, 4);
void xor_c() xor_r(C, 4);
void xor_d() xor_r(D, 4);
void xor_e() xor_r(E, 4);
void xor_h() xor_r(H, 4);
void xor_Ix() xor_r(HX, 8);
void xor_Iy() xor_r(HY, 8);
void xor_l() xor_r(L, 4);
void xor_iX() xor_r(LX, 8);
void xor_iY() xor_r(LY, 8);
void xor_a() xor_r(A, 4);

/* void xor_a()
{
	T(4);
	A = flags._X = flags._Y = flags._N = flags._C = flags._H = flags._S = 0;
	flags._P = flags._Z = 1;
} */

/*=========================================================================*
 *                            xor_n                                        *
 *=========================================================================*/
void xor_n() xor_r(Getnextbyte(), 7);

/*=========================================================================*
 *                            xor_phl                                      *
 *=========================================================================*/
void xor_phl() xor_r(readbyte(HL), 7);
void xor_pix() xor_r(readbyte(pIX), 19);
void xor_piy() xor_r(readbyte(pIY), 19);

#undef xor_r

/*=========================================================================*
 *                            cp_r                                         *
 *=========================================================================*/

#define cp_r(r, TS) { \
	LOCAL UCHAR tmp; \
	\
	T(TS); \
	flags._N = 1; \
	flags._Z = !(tmp = (UCHAR)(A - (r)) ); \
    flags._H = ((tmp & 0x0f) > (A & 0x0f)); \
	flags._C = ((r) > A); \
	flags._P = (A^(r)) & (UCHAR)BIT_7 & (tmp^A); \
	flags._S = (tmp & (UCHAR)BIT_7); \
	flags._X = (r) & (UCHAR)BIT_5; \
	flags._Y = (r) & (UCHAR)BIT_3; \
        Q = 1; \
}

// flags._Z = !(tmp = (UCHAR)(A - (r)) - (flags._H != 0));
// flags._H = ((r) & (UCHAR)0xF) > (A & (UCHAR)0xF);

void cp_b()  cp_r(B, 4);
void cp_c()  cp_r(C, 4);
void cp_d()  cp_r(D, 4);
void cp_e()  cp_r(E, 4);
void cp_h()  cp_r(H, 4);
void cp_Ix() cp_r(HX, 8);
void cp_Iy() cp_r(HY, 8);
void cp_l()  cp_r(L, 4);
void cp_iX() cp_r(LX, 8);
void cp_iY() cp_r(LY, 8);
void cp_a()  cp_r(A, 4);

/* void cp_a()
{
	T(4);
	flags._Z = flags._N = 1;
	flags._X = flags._Y = flags._H = flags._C = flags._P = flags._S = 0;
} */

#undef cp_r

/*=========================================================================*
 *                            cp_n                                         *
 *=========================================================================*/
#define cp_pss(ss, TS) \
{ \
	LOCAL UCHAR tmp, n; \
	\
	T(TS); \
	flags._N = 1; \
    flags._H = ((n = (ss)) & (UCHAR)0xF) > (A & (UCHAR)0xF); \
	flags._Z = !(tmp = (UCHAR)(A - n )); \
	flags._C = (n > A); \
	flags._P = (A^(n)) & (UCHAR)BIT_7 & (tmp^A); \
	flags._S = (tmp & (UCHAR)BIT_7); \
	flags._X = n & (UCHAR)BIT_5; \
	flags._Y = n & (UCHAR)BIT_3; \
        Q = 1; \
}

// flags._Z = !(tmp = (UCHAR)(A - n - (flags._H != 0)));

// flags._H = (((n = (ss)) & (UCHAR)0xF) > (A & (UCHAR)0xF) );

void cp_n() cp_pss(Getnextbyte(), 7);

/*=========================================================================*
 *                            cp_phl                                       *
 *=========================================================================*/
void cp_phl() cp_pss(readbyte(HL), 7);
void cp_pix() cp_pss(readbyte(pIX), 19);
void cp_piy() cp_pss(readbyte(pIY), 19);

#undef cp_pss

/*=========================================================================*
 *                            inc_r                                        *
 *=========================================================================*/

#define inc_r(r, TS) { \
	T(TS); \
	flags._P = ((r) == 0x7F); \
	flags._N = 0; \
    flags._H = ((r) & (UCHAR)0x0F) == 0x0F; \
	flags._S = (++(r) & (UCHAR)BIT_7); \
	flags._Z = !(r); \
    flags._X = (r) & (UCHAR)BIT_5; \
    flags._Y = (r) & (UCHAR)BIT_3; \
    Q = 1; \
}

// flags._P = ((r) == 0xFF);

void inc_b() inc_r(B, 4);
void inc_c() inc_r(C, 4);
void inc_d() inc_r(D, 4);
void inc_e() inc_r(E, 4);
void inc_h() inc_r(H, 4);
void inc_Ix() inc_r(HX, 8);
void inc_Iy() inc_r(HY, 8);
void inc_l() inc_r(L, 4);
void inc_iX() inc_r(LX, 8);
void inc_iY() inc_r(LY, 8);
void inc_a() inc_r(A, 4);

#undef inc_r

/*=========================================================================*
 *                            inc_phl                                      *
 *=========================================================================*/
#define inc_pss(ss, TS) \
{ \
	LOCAL UCHAR n; \
	LOCAL USHORT tmp; \
	\
	T(TS); \
	flags._P = ((n = readbyte(tmp = (ss))) == 0x7F); \
	flags._N = 0; \
        flags._H = ((n) & (UCHAR)0x0F) == 0x0F; \
	flags._S = (++n & (UCHAR)BIT_7); \
	writebyte(tmp, n); \
	flags._Z = !n; \
	flags._X = n & (UCHAR)BIT_5; \
	flags._Y = n & (UCHAR)BIT_3; \
        Q = 1; \
}

void inc_phl() inc_pss(HL, 11);
void inc_pix() inc_pss(pIX, 23);
void inc_piy() inc_pss(pIY, 23);

#undef inc_pss

/*=========================================================================*
 *                            dec_r                                        *
 *=========================================================================*/

#define dec_r(r,TS) { \
	T(TS); \
	flags._P = ((r) == (UCHAR)BIT_7); \
	flags._N = 1; \
    flags._H = ((r) & (UCHAR)0x0F) == 0x00; \
	flags._S = ((--(r)) & (UCHAR)BIT_7); \
	flags._Z = !(r); \
	flags._X = (r) & (UCHAR)BIT_5; \
	flags._Y = (r) & (UCHAR)BIT_3; \
        Q = 1; \
}

// flags._P = ((r) == (UCHAR)0);


void dec_b() dec_r(B,4);
void dec_c() dec_r(C,4);
void dec_d() dec_r(D,4);
void dec_e() dec_r(E,4);
void dec_h() dec_r(H,4);
void dec_Ix() dec_r(HX,8);
void dec_Iy() dec_r(HY,8);
void dec_l() dec_r(L,4);
void dec_iX() dec_r(LX,8);
void dec_iY() dec_r(LY,8);
void dec_a() dec_r(A,4);

#undef dec_r

/*=========================================================================*
 *                            dec_phl                                      *
 *=========================================================================*/
#define dec_pss(ss,TS) \
{ \
	LOCAL UCHAR n; \
	LOCAL USHORT tmp; \
		  \
	T(TS);    \
    n = readbyte(tmp = (ss)); \
	flags._P = (n == (UCHAR)BIT_7); \
	flags._N = 1; \
    flags._H = ((n) & (UCHAR)0x0F) == 0x00; \
	flags._S = ((--n) & (UCHAR)BIT_7); \
	writebyte(tmp, n); \
	flags._Z = !n;     \
	flags._X = n & (UCHAR)BIT_5; \
	flags._Y = n & (UCHAR)BIT_3;  \
        Q = 1; \
}

void dec_phl() dec_pss(HL, 11);
void dec_pix() dec_pss(pIX, 23);
void dec_piy() dec_pss(pIY, 23);

#undef dec_pss

/* EOF: Math8Bit.c */
