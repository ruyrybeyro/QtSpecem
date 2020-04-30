
/* Rotate.c: Z80 rotate and displacement instructions.
 *
 * Copyright 1991-1996 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "../h/env.h"

/*=========================================================================*
 *                            rlca                                         *
 *=========================================================================*/
/*                        ----------------
			 |  ----------    |
		     CY <- | 7  <-  0 |<--
			    ----------
				A
 */
void rlca()
{
   T(4);
   Z80_H = Z80_N = 0;
   A =(A<<1)| (UCHAR)(Z80_C =(A>>7));
   Z80_X = A & (UCHAR)BIT_5;
   Z80_Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            rla                                          *
 *=========================================================================*/
/*               -------------------------
		|           ----------    |
		 --- CY <- | 7  <-  0 |<--
			    ----------
				A
 */
void rla()
{
   LOCAL UCHAR flagCcopy;

   T(4);
   flagCcopy = (Z80_C != 0);
   Z80_C = A & (UCHAR)BIT_7;
   A=((A<<1)|(flagCcopy));
   Z80_H = Z80_N = 0;
   Z80_X = A & (UCHAR)BIT_5;
   Z80_Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            rrca                                         *
 *=========================================================================*/
/*                     -----------------
		      |     ---------   |
		      --> | 7  ->  0 | ---> CY
			    ---------
				A
 */
void rrca()
{
   T(4);
   Z80_C = A & BIT_0;
   A = (A>>1) | (A<<7);
   Z80_H = Z80_N = 0;
   Z80_X = A & (UCHAR)BIT_5;
   Z80_Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            rra                                          *
 *=========================================================================*/
/*                     ---------------------
		      |    ---------       |
		      --> | 7  ->  0 | ---> CY
			   ---------
										  A
 */
void rra()
{
   LOCAL UCHAR flagCcopy;

   T(4);
   flagCcopy = Z80_C?(UCHAR)BIT_7:0;
   Z80_C = A & BIT_0;
   A=(A>>1)|flagCcopy;
   Z80_H = Z80_N = 0;
   Z80_X = A & (UCHAR)BIT_5;
   Z80_Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            rlc_r                                        *
 *=========================================================================*/
/*                         ----------------
			  |    ----------    |
			CY <- | 7  <-  0 |<--
			       ----------
										  r
 */
#define rlc_r(r, TS) { \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_Z = !((r)=((r)<<1)|(UCHAR)(Z80_C=((r)>>7))); \
   Z80_S = ((r) & (UCHAR)BIT_7); \
   Z80_P = parity(r); \
   Z80_X = (r) & (UCHAR)BIT_5; \
   Z80_Y = (r) & (UCHAR)BIT_3; \
   Q = 1; \
}

void rlc_b()  rlc_r(B,   8);
void rlc_c()  rlc_r(C,   8);
void rlc_d()  rlc_r(D,   8);
void rlc_e()  rlc_r(E,   8);
void rlc_h()  rlc_r(H,   8);
void rlc_Ix() rlc_r(HX, 15);
void rlc_Iy() rlc_r(HY, 15);
void rlc_l()  rlc_r(L,   8);
void rlc_iX() rlc_r(LX, 15);
void rlc_iY() rlc_r(LY, 15);
void rlc_a()  rlc_r(A,   8);

#undef rlc_r

/*=========================================================================*
 *                            rlc_phl                                      *
 *=========================================================================*/
/*                        ----------------
			 |  ----------    |
		     CY <- | 7  <-  0 |<--
			    ----------
				(HL),(IX+d),(IY+d)
 */
#define rlc_pss(ss, TS) \
{ \
   LOCAL USHORT tmp; \
   LOCAL UCHAR n; \
   \
   T(TS); \
   n = readbyte(tmp = ss); \
   Z80_H = Z80_N = 0; \
   Z80_Z = !((n)=((n)<<1)|(UCHAR)(Z80_C=((n)>>7))); \
   Z80_S = (n & (UCHAR)BIT_7); \
   Z80_P = parity(n); \
   writebyte(tmp, n); \
   Z80_X = n & (UCHAR)BIT_5; \
   Z80_Y = n & (UCHAR)BIT_3; \
   Q = 1; \
}

void rlc_phl() rlc_pss(HL,    15);
void rlc_pix() rlc_pss(pCBIX, 23);
void rlc_piy() rlc_pss(pCBIY, 23);

#undef rlc_pss

/*=========================================================================*
 *                            rl_r                                         *
 *=========================================================================*/
/*               -------------------------
		|           ----------    |
		 --- CY <- | 7  <-  0 |<--
			    ----------
				r
 */
#define rl_r(r, TS) { \
   LOCAL UCHAR flagCcopy; \
   \
   T(TS); \
   Z80_H = Z80_N = 0; \
   flagCcopy = (Z80_C != 0); \
   Z80_C = ((r) & (UCHAR)BIT_7); \
   Z80_S = (((r) = (((r) << (UCHAR)1) | flagCcopy)) & (UCHAR)BIT_7); \
   Z80_Z = !(r); \
   Z80_P = parity(r); \
   Z80_X = (r) & (UCHAR)BIT_5; \
   Z80_Y = (r) & (UCHAR)BIT_3; \
   Q = 1; \
}


void rl_b()  rl_r(B,   8);
void rl_c()  rl_r(C,   8);
void rl_d()  rl_r(D,   8);
void rl_e()  rl_r(E,   8);
void rl_h()  rl_r(H,   8);
void rl_Ix() rl_r(HX, 15);
void rl_Iy() rl_r(HY, 15);
void rl_l()  rl_r(L,   8);
void rl_iX() rl_r(LX, 15);
void rl_iY() rl_r(LY, 15);
void rl_a()  rl_r(A,   8);

#undef rl_r

/*=========================================================================*
 *                            rl_phl                                       *
 *=========================================================================*/
/*               -------------------------
		|           ----------    |
		 --- CY <- | 7  <-  0 |<--
			    ----------
				(HL),(IX+d),(IY+d)
 */

#define rl_pss(ss, TS) \
{ \
   LOCAL UCHAR flagCcopy; \
   LOCAL USHORT tmp; \
   LOCAL UCHAR n; \
   \
   T(TS); \
   Z80_H = Z80_N = 0; \
   flagCcopy = (Z80_C != 0); \
   Z80_C = ((n = readbyte(tmp = ss)) & (UCHAR)BIT_7); \
   Z80_S = ((n = (n << (UCHAR)1) | flagCcopy) & (UCHAR)BIT_7); \
   Z80_Z = !n; \
   Z80_P = parity(n); \
   writebyte(tmp, n); \
   Z80_X = n & (UCHAR)BIT_5; \
   Z80_Y = n & (UCHAR)BIT_3; \
   Q = 1; \
}

void rl_phl() rl_pss(HL   , 15);
void rl_pix() rl_pss(pCBIX, 23);
void rl_piy() rl_pss(pCBIY, 23);

#undef rl_pss

/*=========================================================================*
 *                            rrc_r                                        *
 *=========================================================================*/
/*                     -----------------
		      |     ---------   |
		      --> | 7  ->   0| ---> CY
			    ---------
				r
 */
#define rrc_r(r, TS) { \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_S = Z80_C = (r&1)?(USHORT)0x0080:0; \
   r = (r>>1) | Z80_C; \
   Z80_Z = !r; \
   Z80_P = parity(r); \
   Z80_X = (r) & (UCHAR)BIT_5; \
   Z80_Y = (r) & (UCHAR)BIT_3; \
   Q = 1; \
}


void rrc_b()  rrc_r(B,   8);
void rrc_c()  rrc_r(C,   8);
void rrc_d()  rrc_r(D,   8);
void rrc_e()  rrc_r(E,   8);
void rrc_h()  rrc_r(H,   8);
void rrc_Ix() rrc_r(HX, 15);
void rrc_Iy() rrc_r(HY, 15);
void rrc_l()  rrc_r(L,   8);
void rrc_iX() rrc_r(LX, 15);
void rrc_iY() rrc_r(LY, 15);
void rrc_a()  rrc_r(A,   8);

#undef rrc_r

/*=========================================================================*
 *                            rrc_phl                                      *
 *=========================================================================*/
/*                     -----------------
		      |     ---------   |
		      --> | 7  ->   0| ---> CY
			    ---------
			(HL),(IX+d),(IY+d)
 */
#define rrc_pss(ss, TS) \
{ \
   LOCAL USHORT tmp; \
   LOCAL UCHAR n; \
   \
   T(TS); \
   Z80_H = Z80_N = 0; \
   n = readbyte(tmp = ss); \
   Z80_S = Z80_C = (n&1)?(USHORT)0x0080:0; \
   n = (n>>1) | Z80_C; \
   Z80_Z = !n; \
   Z80_P = parity(n); \
   writebyte(tmp, n); \
   Z80_X = n & (UCHAR)BIT_5; \
   Z80_Y = n & (UCHAR)BIT_3; \
   Q = 1; \
}

void rrc_phl() rrc_pss(HL,    15);
void rrc_pix() rrc_pss(pCBIX, 23);
void rrc_piy() rrc_pss(pCBIY, 23);

#undef rrc_pss

/*=========================================================================*
 *                            rr_r                                         *
 *=========================================================================*/
/*                     ---------------------
		      |     ---------       |
		      --> | 7  ->  0 | ---> CY
			    ---------
				r
 */
#define rr_r(r, TS) { \
   LOCAL UCHAR flagCcopy; \
   \
   T(TS); \
   Z80_H = Z80_N = 0; \
   flagCcopy = Z80_C?(UCHAR)BIT_7:0; \
   Z80_C = ((r) & (UCHAR)1); \
   Z80_Z = !(r = (r >> 1) | flagCcopy); \
   Z80_S = ((r) & (UCHAR)BIT_7); \
   Z80_P = parity(r); \
   Z80_X = (r) & (UCHAR)BIT_5; \
   Z80_Y = (r) & (UCHAR)BIT_3; \
   Q = 1; \
}

void rr_b()  rr_r(B,   8);
void rr_c()  rr_r(C,   8);
void rr_d()  rr_r(D,   8);
void rr_e()  rr_r(E,   8);
void rr_h()  rr_r(H,   8);
void rr_Ix() rr_r(HX, 15);
void rr_Iy() rr_r(HY, 15);
void rr_l()  rr_r(L,   8);
void rr_iX() rr_r(LX, 15);
void rr_iY() rr_r(LY, 15);
void rr_a()  rr_r(A,   8);

#undef rr_r

/*=========================================================================*
 *                            rr_phl                                       *
 *=========================================================================*/
/*                     ---------------------
		      |     ----------      |
		      --> | 7  ->   0 | ---> CY
			    ----------
				(HL),(IX+d),(IY+d)
 */
#define rr_pss(ss, TS) \
{ \
   LOCAL UCHAR flagCcopy; \
   LOCAL USHORT tmp; \
   LOCAL UCHAR n; \
   \
   T(TS); \
   Z80_H = Z80_N = 0; \
   flagCcopy = Z80_C?(UCHAR)BIT_7:0; \
   Z80_C = ((n = readbyte(tmp = ss)) & (UCHAR)1); \
   Z80_Z = !(n = (n >> (UCHAR)1) | flagCcopy); \
   Z80_S = (n & (UCHAR)BIT_7); \
   Z80_P = parity(n); \
   writebyte(tmp, n); \
   Z80_X = n & (UCHAR)BIT_5; \
   Z80_Y = n & (UCHAR)BIT_3; \
   Q = 1; \
}

void rr_phl() rr_pss(HL,    15);
void rr_pix() rr_pss(pCBIX, 23);
void rr_piy() rr_pss(pCBIY, 23);

#undef rr_pss

/*=========================================================================*
 *                            sla_r                                        *
 *=========================================================================*/
/*
			    ----------
		     CY <- | 7  <-  0 |<-- 0
			    ----------
				A
 */

#define sla_r(r, TS) { \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_C = ((r) & (UCHAR)BIT_7); \
   Z80_Z = !((r) = ((r) << (UCHAR)1) ); \
   Z80_S = ((r) & (UCHAR)BIT_7); \
   Z80_P = parity(r); \
   Z80_X = (r) & (UCHAR)BIT_5; \
   Z80_Y = (r) & (UCHAR)BIT_3; \
   Q = 1; \
}

void sla_b()  sla_r(B,   8);
void sla_c()  sla_r(C,   8);
void sla_d()  sla_r(D,   8);
void sla_e()  sla_r(E,   8);
void sla_h()  sla_r(H,   8);
void sla_Ix() sla_r(HX, 15);
void sla_Iy() sla_r(HY, 15);
void sla_l()  sla_r(L,   8);
void sla_iX() sla_r(LX, 15);
void sla_iY() sla_r(LY, 15);
void sla_a()  sla_r(A,   8);

#undef sla_r

/*=========================================================================*
 *                            sla_phl                                      *
 *=========================================================================*/
/*
			    ----------
		     CY <- | 7  <-  0 |<-- 0
			    ----------
			(HL),(IX+d),(IY+d)
 */
#define sla_pss(ss, TS) \
{ \
   LOCAL USHORT tmp; \
   LOCAL UCHAR n; \
   \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_C = ((n = readbyte(tmp = ss)) & (UCHAR)BIT_7); \
   Z80_Z = !(n <<= (UCHAR)1); \
   Z80_S = (n & (UCHAR)BIT_7); \
   Z80_P = parity(n); \
   writebyte(tmp, n); \
   Z80_X = n & (UCHAR)BIT_5; \
   Z80_Y = n & (UCHAR)BIT_3; \
   Q = 1; \
}

void sla_phl() sla_pss(HL,    15);
void sla_pix() sla_pss(pCBIX, 23);
void sla_piy() sla_pss(pCBIY, 23);

#undef sla_pss

/*=========================================================================*
 *                            sll_r  (sls_r)                               *
 *=========================================================================*/
/*
			    ----------
		     CY <- | 7  <-  0 |<-- 1
			    ----------
				A
 */

#define sll_r(r, TS) { \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_C = ((r) & (UCHAR)BIT_7); \
   Z80_Z = !((r) = (((r) << (UCHAR)1) | (UCHAR)1) ); \
   Z80_S = ((r) & (UCHAR)BIT_7); \
   Z80_P = parity(r); \
   Z80_X = (r) & (UCHAR)BIT_5; \
   Z80_Y = (r) & (UCHAR)BIT_3; \
   Q = 1; \
}

void sll_b()  sll_r(B,   8);
void sll_c()  sll_r(C,   8);
void sll_d()  sll_r(D,   8);
void sll_e()  sll_r(E,   8);
void sll_h()  sll_r(H,   8);
void sll_Ix() sll_r(HX, 15);
void sll_Iy() sll_r(HY, 15);
void sll_l()  sll_r(L,   8);
void sll_iX() sll_r(LX, 15);
void sll_iY() sll_r(LY, 15);
void sll_a()  sll_r(A,   8);

#undef sll_r

/*=========================================================================*
 *                            sll_phl (sls_phl)                            *
 *=========================================================================*/
/*
			    ----------
		     CY <- | 7  <-  0 |<-- 1
			    ----------
			(HL),(IX+d),(IY+d)
 */
#define sll_pss(ss, TS) \
{ \
   LOCAL USHORT tmp; \
   LOCAL UCHAR n; \
   \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_C = ((n = readbyte(tmp = ss)) & (UCHAR)BIT_7); \
   Z80_Z = !(n = ((n << (UCHAR)1) | 1)); \
   Z80_S = (n & (UCHAR)BIT_7); \
   Z80_P = parity(n); \
   writebyte(tmp, n); \
   Z80_X = n & (UCHAR)BIT_5; \
   Z80_Y = n & (UCHAR)BIT_3; \
   Q = 1; \
}

void sll_phl() sll_pss(HL,    15);
void sll_pix() sll_pss(pCBIX, 23);
void sll_piy() sll_pss(pCBIY, 23);

#undef sll_pss

/*=========================================================================*
 *                            sra_r                                        *
 *=========================================================================*/
/*
			    ----------
		       --  | 7  ->  0 | ---> CY
		      |     ----------
		      |     |  r
		       ->---
*/
#define sra_r(r, TS) { \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_C = (r) & (UCHAR)1; \
   Z80_Z = !((r) = ((r) & (UCHAR)BIT_7) | ((r) >> (UCHAR)1)); \
   Z80_S = ((r) & (UCHAR)BIT_7); \
   Z80_P = parity(r); \
   Z80_X = (r) & (UCHAR)BIT_5; \
   Z80_Y = (r) & (UCHAR)BIT_3; \
   Q = 1; \
}


void sra_b()  sra_r(B,   8);
void sra_c()  sra_r(C,   8);
void sra_d()  sra_r(D,   8);
void sra_e()  sra_r(E,   8);
void sra_h()  sra_r(H,   8);
void sra_Ix() sra_r(HX, 15);
void sra_Iy() sra_r(HY, 15);
void sra_l()  sra_r(L,   8);
void sra_iX() sra_r(LX, 15);
void sra_iY() sra_r(LY, 15);
void sra_a()  sra_r(A,   8);

#undef sra_r

/*=========================================================================*
 *                            sra_phl                                      *
 *=========================================================================*/
/*
			    ----------
		       --  | 7  ->  0 | ---> CY
		      |     ----------
		      |     ^  (HL),(IX+d),(IY+d)
		      |     |
		       ->---
*/
#define sra_pss(ss, TS) \
{ \
   LOCAL USHORT tmp; \
   LOCAL UCHAR n; \
   \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_C = (n = readbyte(tmp = ss)) & (UCHAR)1; \
   Z80_Z = !(n = (n & (UCHAR)BIT_7) | (n >> (UCHAR)1)); \
   Z80_S = (n & (UCHAR)BIT_7); \
   Z80_P = parity(n); \
   writebyte(tmp, n); \
   Z80_X = n & (UCHAR)BIT_5; \
   Z80_Y = n & (UCHAR)BIT_3; \
   Q = 1; \
}

void sra_phl() sra_pss(HL,    15);
void sra_pix() sra_pss(pCBIX, 23);
void sra_piy() sra_pss(pCBIY, 23);

#undef sra_pss

/*=========================================================================*
 *                            srl_r                                        *
 *=========================================================================*/
/*
			    ----------
		     0 --> | 7  ->  0 | ---> CY
			    ----------
				r

*/
#define srl_r(r, TS) { \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_C = (r) & (UCHAR)1; \
   Z80_Z = !((r) >>= (UCHAR)1); \
   Z80_S = ((r) & (UCHAR)BIT_7); \
   Z80_P = parity(r); \
   Z80_X = (r) & (UCHAR)BIT_5; \
   Z80_Y = (r) & (UCHAR)BIT_3; \
   Q = 1; \
}

void srl_b()  srl_r(B, 8);
void srl_c()  srl_r(C, 8);
void srl_d()  srl_r(D, 8);
void srl_e()  srl_r(E, 8);
void srl_h()  srl_r(H, 8);
void srl_Ix() srl_r(HX, 15);
void srl_Iy() srl_r(HY, 15);
void srl_l()  srl_r(L, 8);
void srl_iX() srl_r(LX, 15);
void srl_iY() srl_r(LY, 15);
void srl_a()  srl_r(A, 8);

#undef srl_r

/*=========================================================================*
 *                            srl_phl                                      *
 *=========================================================================*/
/*
			    ----------
		     0 --> | 7  ->  0 | ---> CY
			    ----------
			(HL),(IX+d),(IY+d)

*/
#define srl_pss(ss, TS) \
{ \
   LOCAL USHORT tmp; \
   LOCAL UCHAR n; \
   \
   T(TS); \
   Z80_H = Z80_N = 0; \
   Z80_C = (n = readbyte(tmp = ss)) & (UCHAR)1; \
   Z80_Z = !(n >>= (UCHAR)1); \
   Z80_S = (n & (UCHAR)BIT_7); \
   Z80_P = parity(n); \
   writebyte(tmp, n); \
   Z80_X = n & (UCHAR)BIT_5; \
   Z80_Y = n & (UCHAR)BIT_3; \
   Q = 1; \
}

void srl_phl() srl_pss(HL,    15);
void srl_pix() srl_pss(pCBIX, 23);
void srl_piy() srl_pss(pCBIY, 23);

#undef srl_pss

/*=========================================================================*
 *                            rld                                          *
 *=========================================================================*/
/******  RLD:            ____________________
			^                    |
	    [A]  1111 1111     [(HL)]  1111 1111
			^              |  ^    |
			----------------  ------
**/
void rld()
{
   LOCAL UCHAR n;

   T(18);
   writebyte(HL, (A & (UCHAR)0x0F) | ((n=readbyte(HL))<<4) );
   A = (A & (UCHAR)0xF0) | (n>>4);
   Z80_H = Z80_N = 0;
   Z80_Z = !A;
   Z80_S = (A & (UCHAR)BIT_7);
   Z80_P = parity(A);
   Z80_X = A & (UCHAR)BIT_5;
   Z80_Y = A & (UCHAR)BIT_3;
   WZ = HL + 1;                   /* OK */
   Q = 1;
};

/*=========================================================================*
 *                            rrd                                          *
 *=========================================================================*/
/******  RRD:            ______________   ____
			^              | ^    |
	    [A]  1111 1111     [(HL)]  1111 1111
			^                     |
			-----------------------
**/
void rrd()
{
   LOCAL UCHAR n;

   T(18);
   writebyte(HL, (A<<4) | ((n = readbyte(HL))>>4));
   A = (A & (UCHAR)0xF0) | (n & (UCHAR)0x0F);
   Z80_Z = !A;
   Z80_H = Z80_N = 0;
   Z80_S = (A & (UCHAR)BIT_7);
   Z80_P = parity(A);
   Z80_X = A & (UCHAR)BIT_5;
   Z80_Y = A & (UCHAR)BIT_3;
   WZ = HL + 1;                  /* OK */
   Q = 1;
}

/* =================================== */
#define rlc_CB(r) { rlc_pix(); r=readbyte(pCBIX); }
void rlc_pixb() rlc_CB(B);
void rlc_pixc() rlc_CB(C);
void rlc_pixd() rlc_CB(D);
void rlc_pixe() rlc_CB(E);
void rlc_pixa() rlc_CB(A);
void rlc_pixh() rlc_CB(H);
void rlc_pixl() rlc_CB(L);
#undef rlc_CB
#define rlc_CB(r) { rlc_piy(); r=readbyte(pCBIY); }
void rlc_piyb() rlc_CB(B);
void rlc_piyc() rlc_CB(C);
void rlc_piyd() rlc_CB(D);
void rlc_piye() rlc_CB(E);
void rlc_piya() rlc_CB(A);
void rlc_piyh() rlc_CB(H);
void rlc_piyl() rlc_CB(L);
#undef rlc_CB
#define rrc_CB(r) { rrc_pix(); r=readbyte(pCBIX); }
void rrc_pixb() rrc_CB(B);
void rrc_pixc() rrc_CB(C);
void rrc_pixd() rrc_CB(D);
void rrc_pixe() rrc_CB(E);
void rrc_pixa() rrc_CB(A);
void rrc_pixh() rrc_CB(H);
void rrc_pixl() rrc_CB(L);
#undef rrc_CB
#define rrc_CB(r) { rrc_piy(); r=readbyte(pCBIY); }
void rrc_piyb() rrc_CB(B);
void rrc_piyc() rrc_CB(C);
void rrc_piyd() rrc_CB(D);
void rrc_piye() rrc_CB(E);
void rrc_piya() rrc_CB(A);
void rrc_piyh() rrc_CB(H);
void rrc_piyl() rrc_CB(L);
#undef rrc_CB
#define rl_CB(r) { rl_pix(); r=readbyte(pCBIX); }
void rl_pixb() rl_CB(B);
void rl_pixc() rl_CB(C);
void rl_pixd() rl_CB(D);
void rl_pixe() rl_CB(E);
void rl_pixa() rl_CB(A);
void rl_pixh() rl_CB(H);
void rl_pixl() rl_CB(L);
#undef rl_CB
#define rl_CB(r) { rl_piy(); r=readbyte(pCBIY); }
void rl_piyb() rl_CB(B);
void rl_piyc() rl_CB(C);
void rl_piyd() rl_CB(D);
void rl_piye() rl_CB(E);
void rl_piya() rl_CB(A);
void rl_piyh() rl_CB(H);
void rl_piyl() rl_CB(L);
#undef rl_CB
#define rr_CB(r) { rr_pix(); r=readbyte(pCBIX); }
void rr_pixb() rr_CB(B);
void rr_pixc() rr_CB(C);
void rr_pixd() rr_CB(D);
void rr_pixe() rr_CB(E);
void rr_pixa() rr_CB(A);
void rr_pixh() rr_CB(H);
void rr_pixl() rr_CB(L);
#undef rr_CB
#define rr_CB(r) { rr_piy(); r=readbyte(pCBIY); }
void rr_piyb() rr_CB(B);
void rr_piyc() rr_CB(C);
void rr_piyd() rr_CB(D);
void rr_piye() rr_CB(E);
void rr_piya() rr_CB(A);
void rr_piyh() rr_CB(H);
void rr_piyl() rr_CB(L);
#undef rr_CB
#define sla_CB(r) { sla_pix(); r=readbyte(pCBIX); }
void sla_pixb() sla_CB(B);
void sla_pixc() sla_CB(C);
void sla_pixd() sla_CB(D);
void sla_pixe() sla_CB(E);
void sla_pixa() sla_CB(A);
void sla_pixh() sla_CB(H);
void sla_pixl() sla_CB(L);
#undef sla_CB
#define sla_CB(r) { sla_piy(); r=readbyte(pCBIY); }
void sla_piyb() sla_CB(B);
void sla_piyc() sla_CB(C);
void sla_piyd() sla_CB(D);
void sla_piye() sla_CB(E);
void sla_piya() sla_CB(A);
void sla_piyh() sla_CB(H);
void sla_piyl() sla_CB(L);
#undef sla_CB
#define sra_CB(r) { sra_pix(); r=readbyte(pCBIX); }
void sra_pixb() sra_CB(B);
void sra_pixc() sra_CB(C);
void sra_pixd() sra_CB(D);
void sra_pixe() sra_CB(E);
void sra_pixa() sra_CB(A);
void sra_pixh() sra_CB(H);
void sra_pixl() sra_CB(L);
#undef sra_CB
#define sra_CB(r) { sra_piy(); r=readbyte(pCBIY); }
void sra_piyb() sra_CB(B);
void sra_piyc() sra_CB(C);
void sra_piyd() sra_CB(D);
void sra_piye() sra_CB(E);
void sra_piya() sra_CB(A);
void sra_piyh() sra_CB(H);
void sra_piyl() sra_CB(L);
#undef sra_CB
#define sll_CB(r) { sll_pix(); r=readbyte(pCBIX); }
void sll_pixb() sll_CB(B);
void sll_pixc() sll_CB(C);
void sll_pixd() sll_CB(D);
void sll_pixe() sll_CB(E);
void sll_pixa() sll_CB(A);
void sll_pixh() sll_CB(H);
void sll_pixl() sll_CB(L);
#undef sll_CB
#define sll_CB(r) { sll_piy(); r=readbyte(pCBIY); }
void sll_piyb() sll_CB(B);
void sll_piyc() sll_CB(C);
void sll_piyd() sll_CB(D);
void sll_piye() sll_CB(E);
void sll_piya() sll_CB(A);
void sll_piyh() sll_CB(H);
void sll_piyl() sll_CB(L);
#undef sll_CB
#define srl_CB(r) { srl_pix(); r=readbyte(pCBIX); }
void srl_pixb() srl_CB(B);
void srl_pixc() srl_CB(C);
void srl_pixd() srl_CB(D);
void srl_pixe() srl_CB(E);
void srl_pixa() srl_CB(A);
void srl_pixh() srl_CB(H);
void srl_pixl() srl_CB(L);
#undef srl_CB
#define srl_CB(r) { srl_piy(); r=readbyte(pCBIY); }
void srl_piyb() srl_CB(B);
void srl_piyc() srl_CB(C);
void srl_piyd() srl_CB(D);
void srl_piye() srl_CB(E);
void srl_piya() srl_CB(A);
void srl_piyh() srl_CB(H);
void srl_piyl() srl_CB(L);
#undef srl_CB

/* EOF: Rotate.c */
