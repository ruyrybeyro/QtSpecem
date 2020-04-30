/* Bits.c : Z80 bit manipulation instructions.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "../h/env.h"

/*=========================================================================*
 *                            bit_b_r                                      *
 *=========================================================================*/

#define bit_b_r(R, BIT,TS)  {\
   LOCAL UCHAR tmp; \
   T(TS); \
   Z80_Z = Z80_P = !(tmp =((R) & (UCHAR)(BIT))); \
   Z80_H = 1; \
   Z80_N = 0; \
   Z80_S = BIT_7 & tmp; \
   Z80_X = BIT_5 & R; \
   Z80_Y = BIT_3 & R; \
   Q = 1; \
}

void bit_7_b() bit_b_r(B, BIT_7, 8);
void bit_6_b() bit_b_r(B, BIT_6, 8);
void bit_5_b() bit_b_r(B, BIT_5, 8);
void bit_4_b() bit_b_r(B, BIT_4, 8);
void bit_3_b() bit_b_r(B, BIT_3, 8);
void bit_2_b() bit_b_r(B, BIT_2, 8);
void bit_1_b() bit_b_r(B, BIT_1, 8);
void bit_0_b() bit_b_r(B, BIT_0, 8);
void bit_7_c() bit_b_r(C, BIT_7, 8);
void bit_6_c() bit_b_r(C, BIT_6, 8);
void bit_5_c() bit_b_r(C, BIT_5, 8);
void bit_4_c() bit_b_r(C, BIT_4, 8);
void bit_3_c() bit_b_r(C, BIT_3, 8);
void bit_2_c() bit_b_r(C, BIT_2, 8);
void bit_1_c() bit_b_r(C, BIT_1, 8);
void bit_0_c() bit_b_r(C, BIT_0, 8);
void bit_7_d() bit_b_r(D, BIT_7, 8);
void bit_6_d() bit_b_r(D, BIT_6, 8);
void bit_5_d() bit_b_r(D, BIT_5, 8);
void bit_4_d() bit_b_r(D, BIT_4, 8);
void bit_3_d() bit_b_r(D, BIT_3, 8);
void bit_2_d() bit_b_r(D, BIT_2, 8);
void bit_1_d() bit_b_r(D, BIT_1, 8);
void bit_0_d() bit_b_r(D, BIT_0, 8);
void bit_7_e() bit_b_r(E, BIT_7, 8);
void bit_6_e() bit_b_r(E, BIT_6, 8);
void bit_5_e() bit_b_r(E, BIT_5, 8);
void bit_4_e() bit_b_r(E, BIT_4, 8);
void bit_3_e() bit_b_r(E, BIT_3, 8);
void bit_2_e() bit_b_r(E, BIT_2, 8);
void bit_1_e() bit_b_r(E, BIT_1, 8);
void bit_0_e() bit_b_r(E, BIT_0, 8);
void bit_7_h() bit_b_r(H, BIT_7, 8);
void bit_6_h() bit_b_r(H, BIT_6, 8);
void bit_5_h() bit_b_r(H, BIT_5, 8);
void bit_4_h() bit_b_r(H, BIT_4, 8);
void bit_3_h() bit_b_r(H, BIT_3, 8);
void bit_2_h() bit_b_r(H, BIT_2, 8);
void bit_1_h() bit_b_r(H, BIT_1, 8);
void bit_0_h() bit_b_r(H, BIT_0, 8);
void bit_7_Ix() bit_b_r(HX, BIT_7, 15);
void bit_6_Ix() bit_b_r(HX, BIT_6, 15);
void bit_5_Ix() bit_b_r(HX, BIT_5, 15);
void bit_4_Ix() bit_b_r(HX, BIT_4, 15);
void bit_3_Ix() bit_b_r(HX, BIT_3, 15);
void bit_2_Ix() bit_b_r(HX, BIT_2, 15);
void bit_1_Ix() bit_b_r(HX, BIT_1, 15);
void bit_0_Ix() bit_b_r(HX, BIT_0, 15);
void bit_7_Iy() bit_b_r(HY, BIT_7, 15);
void bit_6_Iy() bit_b_r(HY, BIT_6, 15);
void bit_5_Iy() bit_b_r(HY, BIT_5, 15);
void bit_4_Iy() bit_b_r(HY, BIT_4, 15);
void bit_3_Iy() bit_b_r(HY, BIT_3, 15);
void bit_2_Iy() bit_b_r(HY, BIT_2, 15);
void bit_1_Iy() bit_b_r(HY, BIT_1, 15);
void bit_0_Iy() bit_b_r(HY, BIT_0, 15);
void bit_7_l() bit_b_r(L, BIT_7, 8);
void bit_6_l() bit_b_r(L, BIT_6, 8);
void bit_5_l() bit_b_r(L, BIT_5, 8);
void bit_4_l() bit_b_r(L, BIT_4, 8);
void bit_3_l() bit_b_r(L, BIT_3, 8);
void bit_2_l() bit_b_r(L, BIT_2, 8);
void bit_1_l() bit_b_r(L, BIT_1, 8);
void bit_0_l() bit_b_r(L, BIT_0, 8);
void bit_7_iX() bit_b_r(LX, BIT_7, 15);
void bit_6_iX() bit_b_r(LX, BIT_6, 15);
void bit_5_iX() bit_b_r(LX, BIT_5, 15);
void bit_4_iX() bit_b_r(LX, BIT_4, 15);
void bit_3_iX() bit_b_r(LX, BIT_3, 15);
void bit_2_iX() bit_b_r(LX, BIT_2, 15);
void bit_1_iX() bit_b_r(LX, BIT_1, 15);
void bit_0_iX() bit_b_r(LX, BIT_0, 15);
void bit_7_iY() bit_b_r(LY, BIT_7, 15);
void bit_6_iY() bit_b_r(LY, BIT_6, 15);
void bit_5_iY() bit_b_r(LY, BIT_5, 15);
void bit_4_iY() bit_b_r(LY, BIT_4, 15);
void bit_3_iY() bit_b_r(LY, BIT_3, 15);
void bit_2_iY() bit_b_r(LY, BIT_2, 15);
void bit_1_iY() bit_b_r(LY, BIT_1, 15);
void bit_0_iY() bit_b_r(LY, BIT_0, 15);
void bit_7_a() bit_b_r(A, BIT_7, 8);
void bit_6_a() bit_b_r(A, BIT_6, 8);
void bit_5_a() bit_b_r(A, BIT_5, 8);
void bit_4_a() bit_b_r(A, BIT_4, 8);
void bit_3_a() bit_b_r(A, BIT_3, 8);
void bit_2_a() bit_b_r(A, BIT_2, 8);
void bit_1_a() bit_b_r(A, BIT_1, 8);
void bit_0_a() bit_b_r(A, BIT_0, 8);

#undef bit_b_r

/*=========================================================================*
 *                            bit_b_phl                                    *
 *=========================================================================*/

#define bit_b_phl(BIT,expr,TS) { \
	LOCAL UCHAR tmp; \
	\
	T(TS); \
	Z80_Z = Z80_P = !(tmp = ((readbyte(expr)) \
	    & (UCHAR)(BIT))); \
	Z80_H = 1; \
	Z80_N = 0; \
    Z80_S = BIT_7 & tmp; \
	Z80_X = (BIT_5 & W) > 0; \
	Z80_Y = (BIT_3 & W) > 0; \
    Q = 1; \
}

void bit_7_phl() bit_b_phl(BIT_7, HL, 12);
void bit_6_phl() bit_b_phl(BIT_6, HL, 12);
void bit_5_phl() bit_b_phl(BIT_5, HL, 12);
void bit_4_phl() bit_b_phl(BIT_4, HL, 12);
void bit_3_phl() bit_b_phl(BIT_3, HL, 12);
void bit_2_phl() bit_b_phl(BIT_2, HL, 12);
void bit_1_phl() bit_b_phl(BIT_1, HL, 12);
void bit_0_phl() bit_b_phl(BIT_0, HL, 12);
#undef bit_b_phl

#define bit_b_pix(BIT,expr,TS) { \
    LOCAL UCHAR tmp; \
    LOCAL USHORT tmp2; \
    \
    T(TS); \
    Z80_Z = Z80_P = !(tmp = ((readbyte(tmp2 = expr)) \
        & (UCHAR)(BIT))); \
    Z80_H = 1; \
    Z80_N = 0; \
    Z80_S = BIT_7 & tmp; \
    Z80_X = BIT_13 & tmp2; \
    Z80_Y = BIT_11 & tmp2; \
    Q = 1; \
}

void bit_7_pix() bit_b_pix(BIT_7, pCBIX, 20);
void bit_6_pix() bit_b_pix(BIT_6, pCBIX, 20);
void bit_5_pix() bit_b_pix(BIT_5, pCBIX, 20);
void bit_4_pix() bit_b_pix(BIT_4, pCBIX, 20);
void bit_3_pix() bit_b_pix(BIT_3, pCBIX, 20);
void bit_2_pix() bit_b_pix(BIT_2, pCBIX, 20);
void bit_1_pix() bit_b_pix(BIT_1, pCBIX, 20);
void bit_0_pix() bit_b_pix(BIT_0, pCBIX, 20);
void bit_7_piy() bit_b_pix(BIT_7, pCBIY, 20);
void bit_6_piy() bit_b_pix(BIT_6, pCBIY, 20);
void bit_5_piy() bit_b_pix(BIT_5, pCBIY, 20);
void bit_4_piy() bit_b_pix(BIT_4, pCBIY, 20);
void bit_3_piy() bit_b_pix(BIT_3, pCBIY, 20);
void bit_2_piy() bit_b_pix(BIT_2, pCBIY, 20);
void bit_1_piy() bit_b_pix(BIT_1, pCBIY, 20);
void bit_0_piy() bit_b_pix(BIT_0, pCBIY, 20);
#undef bit_b_pix

/*=========================================================================*
 *                            set_b_r                                      *
 *=========================================================================*/

#define set_b_r(R, BIT,TS) { \
	T(TS); \
	(R) |= (UCHAR)(BIT); \
    Z |= BIT; \
    Q = 0; \
	}

void set_7_b() set_b_r(B, BIT_7, 8);
void set_6_b() set_b_r(B, BIT_6, 8);
void set_5_b() set_b_r(B, BIT_5, 8);
void set_4_b() set_b_r(B, BIT_4, 8);
void set_3_b() set_b_r(B, BIT_3, 8);
void set_2_b() set_b_r(B, BIT_2, 8);
void set_1_b() set_b_r(B, BIT_1, 8);
void set_0_b() set_b_r(B, BIT_0, 8);
void set_7_c() set_b_r(C, BIT_7, 8);
void set_6_c() set_b_r(C, BIT_6, 8);
void set_5_c() set_b_r(C, BIT_5, 8);
void set_4_c() set_b_r(C, BIT_4, 8);
void set_3_c() set_b_r(C, BIT_3, 8);
void set_2_c() set_b_r(C, BIT_2, 8);
void set_1_c() set_b_r(C, BIT_1, 8);
void set_0_c() set_b_r(C, BIT_0, 8);
void set_7_d() set_b_r(D, BIT_7, 8);
void set_6_d() set_b_r(D, BIT_6, 8);
void set_5_d() set_b_r(D, BIT_5, 8);
void set_4_d() set_b_r(D, BIT_4, 8);
void set_3_d() set_b_r(D, BIT_3, 8);
void set_2_d() set_b_r(D, BIT_2, 8);
void set_1_d() set_b_r(D, BIT_1, 8);
void set_0_d() set_b_r(D, BIT_0, 8);
void set_7_e() set_b_r(E, BIT_7, 8);
void set_6_e() set_b_r(E, BIT_6, 8);
void set_5_e() set_b_r(E, BIT_5, 8);
void set_4_e() set_b_r(E, BIT_4, 8);
void set_3_e() set_b_r(E, BIT_3, 8);
void set_2_e() set_b_r(E, BIT_2, 8);
void set_1_e() set_b_r(E, BIT_1, 8);
void set_0_e() set_b_r(E, BIT_0, 8);
void set_7_h() set_b_r(H, BIT_7, 8);
void set_6_h() set_b_r(H, BIT_6, 8);
void set_5_h() set_b_r(H, BIT_5, 8);
void set_4_h() set_b_r(H, BIT_4, 8);
void set_3_h() set_b_r(H, BIT_3, 8);
void set_2_h() set_b_r(H, BIT_2, 8);
void set_1_h() set_b_r(H, BIT_1, 8);
void set_0_h() set_b_r(H, BIT_0, 8);
void set_7_Ix() set_b_r(HX, BIT_7, 15);
void set_6_Ix() set_b_r(HX, BIT_6, 15);
void set_5_Ix() set_b_r(HX, BIT_5, 15);
void set_4_Ix() set_b_r(HX, BIT_4, 15);
void set_3_Ix() set_b_r(HX, BIT_3, 15);
void set_2_Ix() set_b_r(HX, BIT_2, 15);
void set_1_Ix() set_b_r(HX, BIT_1, 15);
void set_0_Ix() set_b_r(HX, BIT_0, 15);
void set_7_Iy() set_b_r(HY, BIT_7, 15);
void set_6_Iy() set_b_r(HY, BIT_6, 15);
void set_5_Iy() set_b_r(HY, BIT_5, 15);
void set_4_Iy() set_b_r(HY, BIT_4, 15);
void set_3_Iy() set_b_r(HY, BIT_3, 15);
void set_2_Iy() set_b_r(HY, BIT_2, 15);
void set_1_Iy() set_b_r(HY, BIT_1, 15);
void set_0_Iy() set_b_r(HY, BIT_0, 15);
void set_7_l() set_b_r(L, BIT_7, 8);
void set_6_l() set_b_r(L, BIT_6, 8);
void set_5_l() set_b_r(L, BIT_5, 8);
void set_4_l() set_b_r(L, BIT_4, 8);
void set_3_l() set_b_r(L, BIT_3, 8);
void set_2_l() set_b_r(L, BIT_2, 8);
void set_1_l() set_b_r(L, BIT_1, 8);
void set_0_l() set_b_r(L, BIT_0, 8);
void set_7_iX() set_b_r(LX, BIT_7, 15);
void set_6_iX() set_b_r(LX, BIT_6, 15);
void set_5_iX() set_b_r(LX, BIT_5, 15);
void set_4_iX() set_b_r(LX, BIT_4, 15);
void set_3_iX() set_b_r(LX, BIT_3, 15);
void set_2_iX() set_b_r(LX, BIT_2, 15);
void set_1_iX() set_b_r(LX, BIT_1, 15);
void set_0_iX() set_b_r(LX, BIT_0, 15);
void set_7_iY() set_b_r(LY, BIT_7, 15);
void set_6_iY() set_b_r(LY, BIT_6, 15);
void set_5_iY() set_b_r(LY, BIT_5, 15);
void set_4_iY() set_b_r(LY, BIT_4, 15);
void set_3_iY() set_b_r(LY, BIT_3, 15);
void set_2_iY() set_b_r(LY, BIT_2, 15);
void set_1_iY() set_b_r(LY, BIT_1, 15);
void set_0_iY() set_b_r(LY, BIT_0, 15);
void set_7_a() set_b_r(A, BIT_7, 8);
void set_6_a() set_b_r(A, BIT_6, 8);
void set_5_a() set_b_r(A, BIT_5, 8);
void set_4_a() set_b_r(A, BIT_4, 8);
void set_3_a() set_b_r(A, BIT_3, 8);
void set_2_a() set_b_r(A, BIT_2, 8);
void set_1_a() set_b_r(A, BIT_1, 8);
void set_0_a() set_b_r(A, BIT_0, 8);

#undef set_b_r

/*=========================================================================*
 *                            set_b_phl                                    *
 *=========================================================================*/

#define set_b_phl(BIT,expr,TS) { \
	LOCAL USHORT tmp; \
	\
	T(TS); \
	tmp = expr; \
	writebyte(tmp, readbyte(tmp) | (UCHAR)(BIT) ); \
    Z |= BIT; \
    Q = 0; \
}

void set_7_phl() set_b_phl(BIT_7,HL,15);
void set_6_phl() set_b_phl(BIT_6,HL,15);
void set_5_phl() set_b_phl(BIT_5,HL,15);
void set_4_phl() set_b_phl(BIT_4,HL,15);
void set_3_phl() set_b_phl(BIT_3,HL,15);
void set_2_phl() set_b_phl(BIT_2,HL,15);
void set_1_phl() set_b_phl(BIT_1,HL,15);
void set_0_phl() set_b_phl(BIT_0,HL,15);
void set_7_pix() set_b_phl(BIT_7, pCBIX, 23);
void set_6_pix() set_b_phl(BIT_6, pCBIX, 23);
void set_5_pix() set_b_phl(BIT_5, pCBIX, 23);
void set_4_pix() set_b_phl(BIT_4, pCBIX, 23);
void set_3_pix() set_b_phl(BIT_3, pCBIX, 23);
void set_2_pix() set_b_phl(BIT_2, pCBIX, 23);
void set_1_pix() set_b_phl(BIT_1, pCBIX, 23);
void set_0_pix() set_b_phl(BIT_0, pCBIX, 23);
void set_7_piy() set_b_phl(BIT_7, pCBIY, 23);
void set_6_piy() set_b_phl(BIT_6, pCBIY, 23);
void set_5_piy() set_b_phl(BIT_5, pCBIY, 23);
void set_4_piy() set_b_phl(BIT_4, pCBIY, 23);
void set_3_piy() set_b_phl(BIT_3, pCBIY, 23);
void set_2_piy() set_b_phl(BIT_2, pCBIY, 23);
void set_1_piy() set_b_phl(BIT_1, pCBIY, 23);
void set_0_piy() set_b_phl(BIT_0, pCBIY, 23);

#undef set_b_phl

/*=========================================================================*
 *                            res_b_r                                      *
 *=========================================================================*/

#define res_b_r(R, BIT, TS) { \
	T(TS); \
	(R) &= ~(UCHAR)(BIT); \
    Z &= ~BIT; \
    Q = 0; \
}

void res_7_b() res_b_r(B, BIT_7, 8);
void res_6_b() res_b_r(B, BIT_6, 8);
void res_5_b() res_b_r(B, BIT_5, 8);
void res_4_b() res_b_r(B, BIT_4, 8);
void res_3_b() res_b_r(B, BIT_3, 8);
void res_2_b() res_b_r(B, BIT_2, 8);
void res_1_b() res_b_r(B, BIT_1, 8);
void res_0_b() res_b_r(B, BIT_0, 8);
void res_7_c() res_b_r(C, BIT_7, 8);
void res_6_c() res_b_r(C, BIT_6, 8);
void res_5_c() res_b_r(C, BIT_5, 8);
void res_4_c() res_b_r(C, BIT_4, 8);
void res_3_c() res_b_r(C, BIT_3, 8);
void res_2_c() res_b_r(C, BIT_2, 8);
void res_1_c() res_b_r(C, BIT_1, 8);
void res_0_c() res_b_r(C, BIT_0, 8);
void res_7_d() res_b_r(D, BIT_7, 8);
void res_6_d() res_b_r(D, BIT_6, 8);
void res_5_d() res_b_r(D, BIT_5, 8);
void res_4_d() res_b_r(D, BIT_4, 8);
void res_3_d() res_b_r(D, BIT_3, 8);
void res_2_d() res_b_r(D, BIT_2, 8);
void res_1_d() res_b_r(D, BIT_1, 8);
void res_0_d() res_b_r(D, BIT_0, 8);
void res_7_e() res_b_r(E, BIT_7, 8);
void res_6_e() res_b_r(E, BIT_6, 8);
void res_5_e() res_b_r(E, BIT_5, 8);
void res_4_e() res_b_r(E, BIT_4, 8);
void res_3_e() res_b_r(E, BIT_3, 8);
void res_2_e() res_b_r(E, BIT_2, 8);
void res_1_e() res_b_r(E, BIT_1, 8);
void res_0_e() res_b_r(E, BIT_0, 8);
void res_7_h() res_b_r(H, BIT_7, 8);
void res_6_h() res_b_r(H, BIT_6, 8);
void res_5_h() res_b_r(H, BIT_5, 8);
void res_4_h() res_b_r(H, BIT_4, 8);
void res_3_h() res_b_r(H, BIT_3, 8);
void res_2_h() res_b_r(H, BIT_2, 8);
void res_1_h() res_b_r(H, BIT_1, 8);
void res_0_h() res_b_r(H, BIT_0, 8);
void res_7_Ix() res_b_r(HX, BIT_7, 15);
void res_6_Ix() res_b_r(HX, BIT_6, 15);
void res_5_Ix() res_b_r(HX, BIT_5, 15);
void res_4_Ix() res_b_r(HX, BIT_4, 15);
void res_3_Ix() res_b_r(HX, BIT_3, 15);
void res_2_Ix() res_b_r(HX, BIT_2, 15);
void res_1_Ix() res_b_r(HX, BIT_1, 15);
void res_0_Ix() res_b_r(HX, BIT_0, 15);
void res_7_Iy() res_b_r(HY, BIT_7, 15);
void res_6_Iy() res_b_r(HY, BIT_6, 15);
void res_5_Iy() res_b_r(HY, BIT_5, 15);
void res_4_Iy() res_b_r(HY, BIT_4, 15);
void res_3_Iy() res_b_r(HY, BIT_3, 15);
void res_2_Iy() res_b_r(HY, BIT_2, 15);
void res_1_Iy() res_b_r(HY, BIT_1, 15);
void res_0_Iy() res_b_r(HY, BIT_0, 15);
void res_7_l() res_b_r(L, BIT_7, 8);
void res_6_l() res_b_r(L, BIT_6, 8);
void res_5_l() res_b_r(L, BIT_5, 8);
void res_4_l() res_b_r(L, BIT_4, 8);
void res_3_l() res_b_r(L, BIT_3, 8);
void res_2_l() res_b_r(L, BIT_2, 8);
void res_1_l() res_b_r(L, BIT_1, 8);
void res_0_l() res_b_r(L, BIT_0, 8);
void res_7_iX() res_b_r(LX, BIT_7, 15);
void res_6_iX() res_b_r(LX, BIT_6, 15);
void res_5_iX() res_b_r(LX, BIT_5, 15);
void res_4_iX() res_b_r(LX, BIT_4, 15);
void res_3_iX() res_b_r(LX, BIT_3, 15);
void res_2_iX() res_b_r(LX, BIT_2, 15);
void res_1_iX() res_b_r(LX, BIT_1, 15);
void res_0_iX() res_b_r(LX, BIT_0, 15);
void res_7_iY() res_b_r(LY, BIT_7, 15);
void res_6_iY() res_b_r(LY, BIT_6, 15);
void res_5_iY() res_b_r(LY, BIT_5, 15);
void res_4_iY() res_b_r(LY, BIT_4, 15);
void res_3_iY() res_b_r(LY, BIT_3, 15);
void res_2_iY() res_b_r(LY, BIT_2, 15);
void res_1_iY() res_b_r(LY, BIT_1, 15);
void res_0_iY() res_b_r(LY, BIT_0, 15);
void res_7_a() res_b_r(A, BIT_7, 8);
void res_6_a() res_b_r(A, BIT_6, 8);
void res_5_a() res_b_r(A, BIT_5, 8);
void res_4_a() res_b_r(A, BIT_4, 8);
void res_3_a() res_b_r(A, BIT_3, 8);
void res_2_a() res_b_r(A, BIT_2, 8);
void res_1_a() res_b_r(A, BIT_1, 8);
void res_0_a() res_b_r(A, BIT_0, 8);

#undef res_b_r

/*=========================================================================*
 *                            res_b_phl                                    *
 *=========================================================================*/

#define res_b_phl(BIT,expr,TS) { \
	LOCAL USHORT tmp; \
	\
	T(TS); \
	tmp = expr; \
	writebyte(tmp, readbyte(tmp) & ~(UCHAR)BIT ); \
    Z &= ~BIT; \
    Q = 0; \
}

void res_7_phl() res_b_phl(BIT_7,HL,15);
void res_6_phl() res_b_phl(BIT_6,HL,15);
void res_5_phl() res_b_phl(BIT_5,HL,15);
void res_4_phl() res_b_phl(BIT_4,HL,15);
void res_3_phl() res_b_phl(BIT_3,HL,15);
void res_2_phl() res_b_phl(BIT_2,HL,15);
void res_1_phl() res_b_phl(BIT_1,HL,15);
void res_0_phl() res_b_phl(BIT_0,HL,15);
void res_7_pix() res_b_phl(BIT_7, pCBIX, 23);
void res_6_pix() res_b_phl(BIT_6, pCBIX, 23);
void res_5_pix() res_b_phl(BIT_5, pCBIX, 23);
void res_4_pix() res_b_phl(BIT_4, pCBIX, 23);
void res_3_pix() res_b_phl(BIT_3, pCBIX, 23);
void res_2_pix() res_b_phl(BIT_2, pCBIX, 23);
void res_1_pix() res_b_phl(BIT_1, pCBIX, 23);
void res_0_pix() res_b_phl(BIT_0, pCBIX, 23);
void res_7_piy() res_b_phl(BIT_7, pCBIY, 23);
void res_6_piy() res_b_phl(BIT_6, pCBIY, 23);
void res_5_piy() res_b_phl(BIT_5, pCBIY, 23);
void res_4_piy() res_b_phl(BIT_4, pCBIY, 23);
void res_3_piy() res_b_phl(BIT_3, pCBIY, 23);
void res_2_piy() res_b_phl(BIT_2, pCBIY, 23);
void res_1_piy() res_b_phl(BIT_1, pCBIY, 23);
void res_0_piy() res_b_phl(BIT_0, pCBIY, 23);

#undef res_b_phl

#define res_CB(r, expr) { expr(); r=readbyte(pCBIX);Q=0; }
void res_0_pixb() res_CB(B, res_0_pix);
void res_0_pixc() res_CB(C, res_0_pix);
void res_0_pixd() res_CB(D, res_0_pix);
void res_0_pixe() res_CB(E, res_0_pix);
void res_0_pixa() res_CB(A, res_0_pix);
void res_0_pixh() res_CB(H, res_0_pix);
void res_0_pixl() res_CB(L, res_0_pix);
void res_1_pixb() res_CB(B, res_1_pix);
void res_1_pixc() res_CB(C, res_1_pix);
void res_1_pixd() res_CB(D, res_1_pix);
void res_1_pixe() res_CB(E, res_1_pix);
void res_1_pixa() res_CB(A, res_1_pix);
void res_1_pixh() res_CB(H, res_1_pix);
void res_1_pixl() res_CB(L, res_1_pix);
void res_2_pixb() res_CB(B, res_2_pix);
void res_2_pixc() res_CB(C, res_2_pix);
void res_2_pixd() res_CB(D, res_2_pix);
void res_2_pixe() res_CB(E, res_2_pix);
void res_2_pixa() res_CB(A, res_2_pix);
void res_2_pixh() res_CB(H, res_2_pix);
void res_2_pixl() res_CB(L, res_2_pix);
void res_3_pixb() res_CB(B, res_3_pix);
void res_3_pixc() res_CB(C, res_3_pix);
void res_3_pixd() res_CB(D, res_3_pix);
void res_3_pixe() res_CB(E, res_3_pix);
void res_3_pixa() res_CB(A, res_3_pix);
void res_3_pixh() res_CB(H, res_3_pix);
void res_3_pixl() res_CB(L, res_3_pix);
void res_4_pixb() res_CB(B, res_4_pix);
void res_4_pixc() res_CB(C, res_4_pix);
void res_4_pixd() res_CB(D, res_4_pix);
void res_4_pixe() res_CB(E, res_4_pix);
void res_4_pixa() res_CB(A, res_4_pix);
void res_4_pixh() res_CB(H, res_4_pix);
void res_4_pixl() res_CB(L, res_4_pix);
void res_5_pixb() res_CB(B, res_5_pix);
void res_5_pixc() res_CB(C, res_5_pix);
void res_5_pixd() res_CB(D, res_5_pix);
void res_5_pixe() res_CB(E, res_5_pix);
void res_5_pixa() res_CB(A, res_5_pix);
void res_5_pixh() res_CB(H, res_5_pix);
void res_5_pixl() res_CB(L, res_5_pix);
void res_6_pixb() res_CB(B, res_6_pix);
void res_6_pixc() res_CB(C, res_6_pix);
void res_6_pixd() res_CB(D, res_6_pix);
void res_6_pixe() res_CB(E, res_6_pix);
void res_6_pixa() res_CB(A, res_6_pix);
void res_6_pixh() res_CB(H, res_6_pix);
void res_6_pixl() res_CB(L, res_6_pix);
void res_7_pixb() res_CB(B, res_7_pix);
void res_7_pixc() res_CB(C, res_7_pix);
void res_7_pixd() res_CB(D, res_7_pix);
void res_7_pixe() res_CB(E, res_7_pix);
void res_7_pixa() res_CB(A, res_7_pix);
void res_7_pixh() res_CB(H, res_7_pix);
void res_7_pixl() res_CB(L, res_7_pix);
#undef res_CB
#define res_CB(r, expr) { expr(); r=readbyte(pCBIY);Q=0; }
void res_0_piyb() res_CB(B, res_0_piy);
void res_0_piyc() res_CB(C, res_0_piy);
void res_0_piyd() res_CB(D, res_0_piy);
void res_0_piye() res_CB(E, res_0_piy);
void res_0_piya() res_CB(A, res_0_piy);
void res_0_piyh() res_CB(H, res_0_piy);
void res_0_piyl() res_CB(L, res_0_piy);
void res_1_piyb() res_CB(B, res_1_piy);
void res_1_piyc() res_CB(C, res_1_piy);
void res_1_piyd() res_CB(D, res_1_piy);
void res_1_piye() res_CB(E, res_1_piy);
void res_1_piya() res_CB(A, res_1_piy);
void res_1_piyh() res_CB(H, res_1_piy);
void res_1_piyl() res_CB(L, res_1_piy);
void res_2_piyb() res_CB(B, res_2_piy);
void res_2_piyc() res_CB(C, res_2_piy);
void res_2_piyd() res_CB(D, res_2_piy);
void res_2_piye() res_CB(E, res_2_piy);
void res_2_piya() res_CB(A, res_2_piy);
void res_2_piyh() res_CB(H, res_2_piy);
void res_2_piyl() res_CB(L, res_2_piy);
void res_3_piyb() res_CB(B, res_3_piy);
void res_3_piyc() res_CB(C, res_3_piy);
void res_3_piyd() res_CB(D, res_3_piy);
void res_3_piye() res_CB(E, res_3_piy);
void res_3_piya() res_CB(A, res_3_piy);
void res_3_piyh() res_CB(H, res_3_piy);
void res_3_piyl() res_CB(L, res_3_piy);
void res_4_piyb() res_CB(B, res_4_piy);
void res_4_piyc() res_CB(C, res_4_piy);
void res_4_piyd() res_CB(D, res_4_piy);
void res_4_piye() res_CB(E, res_4_piy);
void res_4_piya() res_CB(A, res_4_piy);
void res_4_piyh() res_CB(H, res_4_piy);
void res_4_piyl() res_CB(L, res_4_piy);
void res_5_piyb() res_CB(B, res_5_piy);
void res_5_piyc() res_CB(C, res_5_piy);
void res_5_piyd() res_CB(D, res_5_piy);
void res_5_piye() res_CB(E, res_5_piy);
void res_5_piya() res_CB(A, res_5_piy);
void res_5_piyh() res_CB(H, res_5_piy);
void res_5_piyl() res_CB(L, res_5_piy);
void res_6_piyb() res_CB(B, res_6_piy);
void res_6_piyc() res_CB(C, res_6_piy);
void res_6_piyd() res_CB(D, res_6_piy);
void res_6_piye() res_CB(E, res_6_piy);
void res_6_piya() res_CB(A, res_6_piy);
void res_6_piyh() res_CB(H, res_6_piy);
void res_6_piyl() res_CB(L, res_6_piy);
void res_7_piyb() res_CB(B, res_7_piy);
void res_7_piyc() res_CB(C, res_7_piy);
void res_7_piyd() res_CB(D, res_7_piy);
void res_7_piye() res_CB(E, res_7_piy);
void res_7_piya() res_CB(A, res_7_piy);
void res_7_piyh() res_CB(H, res_7_piy);
void res_7_piyl() res_CB(L, res_7_piy);
#undef res_CB

/* ++++++++ */
#define set_CB(r, expr) { expr(); r=readbyte(pCBIX);Q=0; }
void set_0_pixb() set_CB(B, set_0_pix);
void set_0_pixc() set_CB(C, set_0_pix);
void set_0_pixd() set_CB(D, set_0_pix);
void set_0_pixe() set_CB(E, set_0_pix);
void set_0_pixa() set_CB(A, set_0_pix);
void set_0_pixh() set_CB(H, set_0_pix);
void set_0_pixl() set_CB(L, set_0_pix);
void set_1_pixb() set_CB(B, set_1_pix);
void set_1_pixc() set_CB(C, set_1_pix);
void set_1_pixd() set_CB(D, set_1_pix);
void set_1_pixe() set_CB(E, set_1_pix);
void set_1_pixa() set_CB(A, set_1_pix);
void set_1_pixh() set_CB(H, set_1_pix);
void set_1_pixl() set_CB(L, set_1_pix);
void set_2_pixb() set_CB(B, set_2_pix);
void set_2_pixc() set_CB(C, set_2_pix);
void set_2_pixd() set_CB(D, set_2_pix);
void set_2_pixe() set_CB(E, set_2_pix);
void set_2_pixa() set_CB(A, set_2_pix);
void set_2_pixh() set_CB(H, set_2_pix);
void set_2_pixl() set_CB(L, set_2_pix);
void set_3_pixb() set_CB(B, set_3_pix);
void set_3_pixc() set_CB(C, set_3_pix);
void set_3_pixd() set_CB(D, set_3_pix);
void set_3_pixe() set_CB(E, set_3_pix);
void set_3_pixa() set_CB(A, set_3_pix);
void set_3_pixh() set_CB(H, set_3_pix);
void set_3_pixl() set_CB(L, set_3_pix);
void set_4_pixb() set_CB(B, set_4_pix);
void set_4_pixc() set_CB(C, set_4_pix);
void set_4_pixd() set_CB(D, set_4_pix);
void set_4_pixe() set_CB(E, set_4_pix);
void set_4_pixa() set_CB(A, set_4_pix);
void set_4_pixh() set_CB(H, set_4_pix);
void set_4_pixl() set_CB(L, set_4_pix);
void set_5_pixb() set_CB(B, set_5_pix);
void set_5_pixc() set_CB(C, set_5_pix);
void set_5_pixd() set_CB(D, set_5_pix);
void set_5_pixe() set_CB(E, set_5_pix);
void set_5_pixa() set_CB(A, set_5_pix);
void set_5_pixh() set_CB(H, set_5_pix);
void set_5_pixl() set_CB(L, set_5_pix);
void set_6_pixb() set_CB(B, set_6_pix);
void set_6_pixc() set_CB(C, set_6_pix);
void set_6_pixd() set_CB(D, set_6_pix);
void set_6_pixe() set_CB(E, set_6_pix);
void set_6_pixa() set_CB(A, set_6_pix);
void set_6_pixh() set_CB(H, set_6_pix);
void set_6_pixl() set_CB(L, set_6_pix);
void set_7_pixb() set_CB(B, set_7_pix);
void set_7_pixc() set_CB(C, set_7_pix);
void set_7_pixd() set_CB(D, set_7_pix);
void set_7_pixe() set_CB(E, set_7_pix);
void set_7_pixa() set_CB(A, set_7_pix);
void set_7_pixh() set_CB(H, set_7_pix);
void set_7_pixl() set_CB(L, set_7_pix);
#undef set_CB
#define set_CB(r, expr) { expr(); r=readbyte(pCBIY);Q=0; }
void set_0_piyb() set_CB(B, set_0_piy);
void set_0_piyc() set_CB(C, set_0_piy);
void set_0_piyd() set_CB(D, set_0_piy);
void set_0_piye() set_CB(E, set_0_piy);
void set_0_piya() set_CB(A, set_0_piy);
void set_0_piyh() set_CB(H, set_0_piy);
void set_0_piyl() set_CB(L, set_0_piy);
void set_1_piyb() set_CB(B, set_1_piy);
void set_1_piyc() set_CB(C, set_1_piy);
void set_1_piyd() set_CB(D, set_1_piy);
void set_1_piye() set_CB(E, set_1_piy);
void set_1_piya() set_CB(A, set_1_piy);
void set_1_piyh() set_CB(H, set_1_piy);
void set_1_piyl() set_CB(L, set_1_piy);
void set_2_piyb() set_CB(B, set_2_piy);
void set_2_piyc() set_CB(C, set_2_piy);
void set_2_piyd() set_CB(D, set_2_piy);
void set_2_piye() set_CB(E, set_2_piy);
void set_2_piya() set_CB(A, set_2_piy);
void set_2_piyh() set_CB(H, set_2_piy);
void set_2_piyl() set_CB(L, set_2_piy);
void set_3_piyb() set_CB(B, set_3_piy);
void set_3_piyc() set_CB(C, set_3_piy);
void set_3_piyd() set_CB(D, set_3_piy);
void set_3_piye() set_CB(E, set_3_piy);
void set_3_piya() set_CB(A, set_3_piy);
void set_3_piyh() set_CB(H, set_3_piy);
void set_3_piyl() set_CB(L, set_3_piy);
void set_4_piyb() set_CB(B, set_4_piy);
void set_4_piyc() set_CB(C, set_4_piy);
void set_4_piyd() set_CB(D, set_4_piy);
void set_4_piye() set_CB(E, set_4_piy);
void set_4_piya() set_CB(A, set_4_piy);
void set_4_piyh() set_CB(H, set_4_piy);
void set_4_piyl() set_CB(L, set_4_piy);
void set_5_piyb() set_CB(B, set_5_piy);
void set_5_piyc() set_CB(C, set_5_piy);
void set_5_piyd() set_CB(D, set_5_piy);
void set_5_piye() set_CB(E, set_5_piy);
void set_5_piya() set_CB(A, set_5_piy);
void set_5_piyh() set_CB(H, set_5_piy);
void set_5_piyl() set_CB(L, set_5_piy);
void set_6_piyb() set_CB(B, set_6_piy);
void set_6_piyc() set_CB(C, set_6_piy);
void set_6_piyd() set_CB(D, set_6_piy);
void set_6_piye() set_CB(E, set_6_piy);
void set_6_piya() set_CB(A, set_6_piy);
void set_6_piyh() set_CB(H, set_6_piy);
void set_6_piyl() set_CB(L, set_6_piy);
void set_7_piyb() set_CB(B, set_7_piy);
void set_7_piyc() set_CB(C, set_7_piy);
void set_7_piyd() set_CB(D, set_7_piy);
void set_7_piye() set_CB(E, set_7_piy);
void set_7_piya() set_CB(A, set_7_piy);
void set_7_piyh() set_CB(H, set_7_piy);
void set_7_piyl() set_CB(L, set_7_piy);
#undef set_CB

/* EOF: Bits.c */
