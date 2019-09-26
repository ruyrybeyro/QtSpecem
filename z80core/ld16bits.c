/* Ld16Bits.c: Z80 16 bit load instructions.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

/*=========================================================================*
 *                            ld_dd_nn                                     *
 *=========================================================================*/

#define ld_dd_nn(dd,TS) { T(TS); dd=Getnextword();Q=0; }

void ld_bc_nn() ld_dd_nn(BC, 10);
void ld_de_nn() ld_dd_nn(DE, 10);
void ld_hl_nn() ld_dd_nn(HL, 10);
void ld_ix_nn() ld_dd_nn(IX, 14);
void ld_iy_nn() ld_dd_nn(IY, 14);
void ld_sp_nn() ld_dd_nn(SP, 10);

#undef ld_dd_nn

/*=========================================================================*
 *                            ld_dd_pnn                                    *
 *=========================================================================*/

#define ld_dd_pnn(dd, TS) { T(TS); dd=readword(WZ=Getnextword() ); WZ++; Q=0; }

void ld_hl_pnn() ld_dd_pnn(HL, 16);
void ld_ix_pnn() ld_dd_pnn(IX, 20);
void ld_iy_pnn() ld_dd_pnn(IY, 20);
void ld_bc_pnn() ld_dd_pnn(BC, 20);
void ld_de_pnn() ld_dd_pnn(DE, 20);
void ld_sp_pnn() ld_dd_pnn(SP, 20);
void ld_hl_pnnED() ld_dd_pnn(HL, 20);
#undef ld_dd_pnn

/*=========================================================================*
 *                            ld_pnn_dd                                    *
 *=========================================================================*/

#define ld_pnn_dd(dd, TS) { T(TS); writeword(WZ=Getnextword(), dd); Q=0; }
// WZ OK

void ld_pnn_hl() ld_pnn_dd(HL, 16);
void ld_pnn_ix() ld_pnn_dd(IX, 20);
void ld_pnn_iy() ld_pnn_dd(IY, 20);
void ld_pnn_bc() ld_pnn_dd(BC, 20);
void ld_pnn_de() ld_pnn_dd(DE, 20);
void ld_pnn_sp() ld_pnn_dd(SP, 20);
void ld_pnn_hlED() ld_pnn_dd(HL, 20);

#undef ld_pnn_dd

#define ld_sp_dd(dd, TS) { T(TS); SP = dd ; Q=0; } 
/*=========================================================================*
 *                            ld_sp_hl                                     *
 *=========================================================================*/
void ld_sp_hl() ld_sp_dd(HL, 6);

/*=========================================================================*
 *                            ld_sp_ix                                     *
 *=========================================================================*/
void ld_sp_ix() ld_sp_dd(IX, 10);

/*=========================================================================*
 *                            ld_sp_iy                                     *
 *=========================================================================*/
void ld_sp_iy() ld_sp_dd(IY, 10);

#undef ld_sp_dd

/*=========================================================================*
 *                            push_qq                                      *
 *=========================================================================*/

#define push_qq(R, TS) { T(TS); push(R); Q=0; }
/* ZW ok */

void push_bc() push_qq(BC, 11);
void push_de() push_qq(DE, 11);
void push_hl() push_qq(HL, 11);
void push_ix() push_qq(IX, 15);
void push_iy() push_qq(IY, 15);

void push_af()
{
	T(11);
	build_F();
	push(AF);
        Q=0;
}

#undef push_qq

/*=========================================================================*
 *                            pop_qq                                       *
 *=========================================================================*/

#define pop_qq(r, TS) { T(TS); (r) = pop(); Q=0; }

/* WZ OK */

void pop_bc() pop_qq(BC, 10);
void pop_de() pop_qq(DE, 10);
void pop_hl() pop_qq(HL, 10);
void pop_ix() pop_qq(IX, 14);
void pop_iy() pop_qq(IY, 14);

void pop_af()
{
	T(10);
	AF = pop();
	read_F();
        Q = 0;
}

#undef pop_qq

/* EOF : Ld16Bits.c */
