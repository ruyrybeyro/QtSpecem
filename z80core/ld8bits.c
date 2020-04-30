/* Ld8Bits.c: Z80 8 bit load instructions.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "../h/env.h"

/*=========================================================================*
 *                            ld_r_s                                       *
 *=========================================================================*/
#define ld_r_s(r,s,TS) { T(TS); (r)=(s); Q=0; }

void ld_b_b()  { T(4); }
void ld_b_c()  ld_r_s(B, C, 4);
void ld_b_d()  ld_r_s(B, D, 4);
void ld_b_e()  ld_r_s(B, E, 4);
void ld_b_h()  ld_r_s(B, H, 4);
void ld_b_Ix() ld_r_s(B, HX, 8);
void ld_b_Iy() ld_r_s(B, HY, 8);
void ld_b_l()  ld_r_s(B, L, 4);
void ld_b_iX() ld_r_s(B, LX, 8);
void ld_b_iY() ld_r_s(B, LY, 8);
void ld_b_a()  ld_r_s(B, A, 4);
void ld_c_b()  ld_r_s(C, B, 4);
void ld_c_c()  { T(4); }
void ld_c_d()  ld_r_s(C, D, 4);
void ld_c_e()  ld_r_s(C, E, 4);
void ld_c_h()  ld_r_s(C, H, 4);
void ld_c_Ix() ld_r_s(C, HX, 8);
void ld_c_Iy() ld_r_s(C, HY, 8);
void ld_c_l()  ld_r_s(C, L, 4);
void ld_c_iX() ld_r_s(C, LX, 8);
void ld_c_iY() ld_r_s(C, LY, 8);
void ld_c_a()  ld_r_s(C, A, 4);
void ld_d_b()  ld_r_s(D, B, 4);
void ld_d_c()  ld_r_s(D, C, 4);
void ld_d_d()  { T(4); }
void ld_d_e()  ld_r_s(D, E, 4);
void ld_d_h()  ld_r_s(D, H, 4);
void ld_d_Ix() ld_r_s(D, HX, 8);
void ld_d_Iy() ld_r_s(D, HY, 8);
void ld_d_l()  ld_r_s(D, L, 4);
void ld_d_iX() ld_r_s(D, LX, 8);
void ld_d_iY() ld_r_s(D, LY, 8);
void ld_d_a()  ld_r_s(D, A, 4);
void ld_e_b()  ld_r_s(E, B, 4);
void ld_e_c()  ld_r_s(E, C, 4);
void ld_e_d()  ld_r_s(E, D, 4);
void ld_e_e()  { T(4); }
void ld_e_h()  ld_r_s(E, H, 4);
void ld_e_Ix() ld_r_s(E, HX, 8);
void ld_e_Iy() ld_r_s(E, HY, 8);
void ld_e_l()  ld_r_s(E, L, 4);
void ld_e_iX() ld_r_s(E, LX, 8);
void ld_e_iY() ld_r_s(E, LY, 8);
void ld_e_a()  ld_r_s(E, A, 4);
void ld_h_b()  ld_r_s(H, B, 4);
void ld_h_c()  ld_r_s(H, C, 4);
void ld_h_d()  ld_r_s(H, D, 4);
void ld_h_e()  ld_r_s(H, E, 4);
void ld_h_h()  { T(4); }
void ld_h_l()  ld_r_s(H, L, 4);
void ld_h_a()  ld_r_s(H, A, 4);
void ld_Ix_b() ld_r_s(HX, B, 8);
void ld_Ix_c() ld_r_s(HX, C, 8);
void ld_Ix_d() ld_r_s(HX, D, 8);
void ld_Ix_e() ld_r_s(HX, E, 8);
void ld_Ix_Ix(){ T(8); }
void ld_Ix_iX()ld_r_s(HX, LX, 8);
void ld_Ix_a() ld_r_s(HX, A, 8);
void ld_Iy_b() ld_r_s(HY, B, 8);
void ld_Iy_c() ld_r_s(HY, C, 8);
void ld_Iy_d() ld_r_s(HY, D, 8);
void ld_Iy_e() ld_r_s(HY, E, 8);
void ld_Iy_Iy(){ T(8); }
void ld_Iy_iY()ld_r_s(HY, LY, 8);
void ld_Iy_a() ld_r_s(HY, A, 8);
void ld_l_b()  ld_r_s(L, B, 4);
void ld_l_c()  ld_r_s(L, C, 4);
void ld_l_d()  ld_r_s(L, D, 4);
void ld_l_e()  ld_r_s(L, E, 4);
void ld_l_h()  ld_r_s(L, H, 4);
void ld_l_l()  { T(4); }
void ld_l_a()  ld_r_s(L, A, 4);
void ld_iX_b() ld_r_s(LX, B, 8);
void ld_iX_c() ld_r_s(LX, C, 8);
void ld_iX_d() ld_r_s(LX, D, 8);
void ld_iX_e() ld_r_s(LX, E, 8);
void ld_iX_iX(){ T(8); }
void ld_iX_Ix()ld_r_s(LX, HX, 8);
void ld_iX_a() ld_r_s(LX, A, 8);
void ld_iY_b() ld_r_s(LY, B, 8);
void ld_iY_c() ld_r_s(LY, C, 8);
void ld_iY_d() ld_r_s(LY, D, 8);
void ld_iY_e() ld_r_s(LY, E, 8);
void ld_iY_iY(){ T(8); }
void ld_iY_Iy()ld_r_s(LY, HY, 8);
void ld_iY_a() ld_r_s(LY, A, 8);
void ld_a_b()  ld_r_s(A, B, 4);
void ld_a_c()  ld_r_s(A, C, 4);
void ld_a_d()  ld_r_s(A, D, 4);
void ld_a_e()  ld_r_s(A, E, 4);
void ld_a_h()  ld_r_s(A, H, 4);
void ld_a_Ix() ld_r_s(A, HX, 8);
void ld_a_Iy() ld_r_s(A, HY, 8);
void ld_a_l()  ld_r_s(A, L, 4);
void ld_a_iX() ld_r_s(A, LX, 8);
void ld_a_iY() ld_r_s(A, LY, 8);
void ld_a_a()  { T(4); }

#undef ld_r_s

/*=========================================================================*
 *                            ld_r_n                                       *
 *=========================================================================*/

#define ld_r_n(r, TS) { T(TS); r = Getnextbyte(); Q=0; }

void ld_b_n()  ld_r_n(B, 7);
void ld_c_n()  ld_r_n(C, 7);
void ld_d_n()  ld_r_n(D, 7);
void ld_e_n()  ld_r_n(E, 7);
void ld_h_n()  ld_r_n(H, 7);
void ld_Ix_n() ld_r_n(HX, 11);
void ld_Iy_n() ld_r_n(HY, 11);
void ld_l_n()  ld_r_n(L, 7);
void ld_iX_n() ld_r_n(LX, 7);
void ld_iY_n() ld_r_n(LY, 7);
void ld_a_n()  ld_r_n(A, 7);

#undef ld_r_n

/*=========================================================================*
 *                            ld_r_phl                                     *
 *=========================================================================*/

#define ld_r_phl(r,expr,TS) { T(TS); (r) = readbyte(expr); Q=0; }

void ld_b_phl() ld_r_phl(B, HL , 7);
void ld_c_phl() ld_r_phl(C, HL , 7);
void ld_d_phl() ld_r_phl(D, HL , 7);
void ld_e_phl() ld_r_phl(E, HL , 7);
void ld_h_phl() ld_r_phl(H, HL , 7);
void ld_l_phl() ld_r_phl(L, HL , 7);
void ld_a_phl() ld_r_phl(A, HL , 7);
void ld_b_pix() ld_r_phl(B, pIX, 19);
void ld_c_pix() ld_r_phl(C, pIX, 19);
void ld_d_pix() ld_r_phl(D, pIX, 19);
void ld_e_pix() ld_r_phl(E, pIX, 19);
void ld_h_pix() ld_r_phl(H, pIX, 19);
void ld_l_pix() ld_r_phl(L, pIX, 19);
void ld_a_pix() ld_r_phl(A, pIX, 19);
void ld_b_piy() ld_r_phl(B, pIY, 19);
void ld_c_piy() ld_r_phl(C, pIY, 19);
void ld_d_piy() ld_r_phl(D, pIY, 19);
void ld_e_piy() ld_r_phl(E, pIY, 19);
void ld_h_piy() ld_r_phl(H, pIY, 19);
void ld_l_piy() ld_r_phl(L, pIY, 19);
void ld_a_piy() ld_r_phl(A, pIY, 19);

#undef ld_r_phl

/*=========================================================================*
 *                            ld_phl_r                                     *
 *=========================================================================*/

#define ld_phl_r(r,expr,TS) { T(TS); writebyte(expr, r); Q=0; }

void ld_phl_b() ld_phl_r(B, HL, 7);
void ld_phl_c() ld_phl_r(C, HL, 7);
void ld_phl_d() ld_phl_r(D, HL, 7);
void ld_phl_e() ld_phl_r(E, HL, 7);
void ld_phl_h() ld_phl_r(H, HL, 7);
void ld_phl_l() ld_phl_r(L, HL, 7);
void ld_phl_a() ld_phl_r(A, HL, 7);
void ld_pix_b() ld_phl_r(B, pIX, 19);
void ld_pix_c() ld_phl_r(C, pIX, 19);
void ld_pix_d() ld_phl_r(D, pIX, 19);
void ld_pix_e() ld_phl_r(E, pIX, 19);
void ld_pix_h() ld_phl_r(H, pIX, 19);
void ld_pix_l() ld_phl_r(L, pIX, 19);
void ld_pix_a() ld_phl_r(A, pIX, 19);
void ld_piy_b() ld_phl_r(B, pIY, 19);
void ld_piy_c() ld_phl_r(C, pIY, 19);
void ld_piy_d() ld_phl_r(D, pIY, 19);
void ld_piy_e() ld_phl_r(E, pIY, 19);
void ld_piy_h() ld_phl_r(H, pIY, 19);
void ld_piy_l() ld_phl_r(L, pIY, 19);
void ld_piy_a() ld_phl_r(A, pIY, 19);

#undef ld_phl_r

/*=========================================================================*
 *                            ld_phl_n                                     *
 *=========================================================================*/
void ld_phl_n() { T(10); writebyte(HL, Getnextbyte() ); Q=0; }
/* The next two instructions will be a possible source of problems with a
	optimizing compiler -- if tmp was cutted we will have a problem in
	writebyte()
 */
static USHORT tmp;
void ld_pix_n() { /*USHORT tmp;*/ T(19); tmp=pIX;
			 writebyte(tmp,	Getnextbyte() ); Q=0; }
void ld_piy_n() { /*USHORT tmp;*/ T(19); tmp=pIY;
			 writebyte(tmp,	Getnextbyte() ); Q=0; }

/*=========================================================================*
 *                            ld_a_pbc                                     *
 *=========================================================================*/
void ld_a_pbc()
{
	T(7);
	A = readbyte(BC);
	WZ = BC + 1; /* OK */
    Q=0;
}

/*=========================================================================*
 *                            ld_a_pde                                     *
 *=========================================================================*/
void ld_a_pde()
{
	T(7);
	A = readbyte(DE);
    WZ = DE + 1; /* OK */
    Q=0;
}

/*=========================================================================*
 *                            ld_a_pnn                                     *
 *=========================================================================*/
void ld_a_pnn()
{
        USHORT addr;
	T(13);
	A = readbyte(addr = Getnextword());
        WZ = addr + 1; /* OK */
        Q=0;
}

/*=========================================================================*
 *                            ld_pbc_a                                     *
 *=========================================================================*/
void ld_pbc_a()
{
	T(7);
	writebyte(BC, A);
    W = A;
    Z = C+1;
    Q=0;
}

/*=========================================================================*
 *                            ld_pde_a                                     *
 *=========================================================================*/
void ld_pde_a()
{
	T(7);
	writebyte(DE, A);
    W = A;
    Z = E + 1;
    Q=0;
}

/*=========================================================================*
 *                            ld_pnn_a                                     *
 *=========================================================================*/
void ld_pnn_a()
{
	USHORT addr;

	T(13);
	writebyte(addr = Getnextword(), A);
    W = A;
    Z = (UCHAR)(addr +1);
    Q=0;
}

/*=========================================================================*
 *                            ld_a_i                                       *
 *=========================================================================*/
void ld_a_i()
{
	T(9);
	A = I;
	Z80_P = IFF2;
	/* IFF2 got IFF1 copy, so this instruction can be used in Z80 programming
	  to know the former state of interrupts in the middle of a NMI interrupt
	*/
	Z80_S = (A & BIT_7);
	Z80_Z = !A;
	Z80_H = Z80_N = 0;
	Z80_X = A & (UCHAR)BIT_5;
	Z80_Y = A & (UCHAR)BIT_3;
        Q = 1;
}

/*=========================================================================*
 *                            ld_a_r                                       *
 *=========================================================================*/
void ld_a_r()
{
	T(9);
	A = ((R-1) & ~BIT_7) | R_BIT7;
	Z80_P = IFF2;
	/* IFF2 got IFF1 copy, so this instruction can be used in Z80 programming
	  to know the former state of interrupts in the middle of a NMI interrupt
	*/
	Z80_S = (A & BIT_7);
	Z80_Z = !A;
	//Z80_S = Z80_X = Z80_Y =
	Z80_H = Z80_N = 0;
	Z80_X = A & (UCHAR)BIT_5;
	Z80_Y = A & (UCHAR)BIT_3;
        Q = 1;
}

/*=========================================================================*
 *                            ld_i_a                                       *
 *=========================================================================*/
void ld_i_a()
{
	T(9);
	I = A;
        Q = 0;
}

/*=========================================================================*
 *                            ld_r_a                                       *
 *=========================================================================*/
void ld_r_a()
{
	T(9);
	R = A + 1;
	R_BIT7 = (A & BIT_7);
        Q = 0;
}

/* EOF : Ld8Bits.c */
