/* Math16Bi.c: Z80 16 bit arithmetic instructions.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "../h/env.h"

/*=========================================================================*
 *                            add_hl_ss                                    *
 *=========================================================================*/

#define add_hl_ss(hl,ss,hh,TS) { \
   LOCAL USHORT tmp; \
   \
   T(TS); \
   Z80_N = 0; \
   Z80_C = ( (tmp = (USHORT)(hl + (ss))) < hl ) || ((tmp==hl) && (ss)); \
   Z80_H = (((hl & (USHORT)0xFFF) + ((ss) & (USHORT)0xFFF) ) & (USHORT)0x1000 ) > 0; \
   WZ=hl+1; /* OK */ \
   hl = tmp; \
   Z80_X = hh & (UCHAR)BIT_5; \
   Z80_Y = hh & (UCHAR)BIT_3; \
   Q = 1; \
}

void add_hl_bc() add_hl_ss(HL,BC,H,11);
void add_hl_de() add_hl_ss(HL,DE,H,11);
void add_hl_hl() add_hl_ss(HL,HL,H,11);
/*void add_hl_hl()
{
   T(11);
   Z80_H = ((HL & (USHORT)0xFFF) > (USHORT)0x7FF) > 0;
   Z80_N = 0;
   Z80_C = (HL > (USHORT)0x7FFF);
   HL += HL;
   Z80_X = H & (UCHAR)BIT_5;
   Z80_Y = H & (UCHAR)BIT_3;
} */

void add_hl_sp() add_hl_ss(HL,SP,H,11);

void add_ix_bc() add_hl_ss(IX,BC,HX,15);
void add_ix_de() add_hl_ss(IX,DE,HX,15);
void add_ix_ix() add_hl_ss(IX,IX,HX,15);
void add_ix_sp() add_hl_ss(IX,SP,HX,15);

void add_iy_bc() add_hl_ss(IY,BC,HY,15);
void add_iy_de() add_hl_ss(IY,DE,HY,15);
void add_iy_iy() add_hl_ss(IY,IY,HY,15);
void add_iy_sp() add_hl_ss(IY,SP,HY,15);

#undef add_hl_ss

/*=========================================================================*
 *                            adc_hl_ss                                    *
 *=========================================================================*/

#define adc_hl_ss(ss,r) { \
	LOCAL USHORT tmp; \
	\
	T(15); \
	Z80_N = 0; \
	Z80_C = ( (tmp = (USHORT)(HL + (ss) + (USHORT) \
		 (Z80_C != 0 ))) < HL ) || ((tmp==HL) && (ss)); \
	Z80_P = ( ((H & BIT_7) == ((r) & BIT_7)) && ((HL ^ tmp) \
				& (USHORT)0x8000) ); \
    Z80_H = (((HL & (USHORT)0xFFF) + ((ss) & (USHORT)0xFFF)  + (Z80_C != 0)  ) & (USHORT)0x1000 ) > 0; \
    WZ = HL+1; /* OK */ \
	Z80_Z = !(HL = tmp); \
	Z80_S = (H & BIT_7); \
	Z80_X = H & (UCHAR)BIT_5; \
	Z80_Y = H & (UCHAR)BIT_3; \
    Q = 1; \
}


/**** ADC overflow: -- = + ou ++ = - */

void adc_hl_bc() adc_hl_ss(BC, B);
void adc_hl_de() adc_hl_ss(DE, D);
void adc_hl_hl() adc_hl_ss(HL, H);
/*
void adc_hl_hl()
{
	LOCAL USHORT tmp;

	T(15);
	Z80_H = ((HL & (USHORT)0xFFF) > (USHORT)0x7FF) > 0;
	Z80_N = 0;
	Z80_C = (HL > (USHORT)0x7FFF);
	Z80_P = ( (HL ^ tmp) & (USHORT)0x8000);
    WZ = HL +1;
	Z80_Z = !(HL += HL + (Z80_C != 0));
	Z80_S = (H & BIT_7);
	Z80_X = H & (UCHAR)BIT_5;
	Z80_Y = H & (UCHAR)BIT_3;
}
*/
void adc_hl_sp() adc_hl_ss(SP, SP >> 8);

#undef adc_hl_ss

/*=========================================================================*
 *                            sbc_hl_ss                                    *
 *=========================================================================*/

#define sbc_hl_ss(ss, r) { \
	 LOCAL USHORT tmp; \
	 \
	 T(15); \
     Z80_H = ((((ss)+(Z80_C!=0)) & (USHORT)0xFFF) > (HL & (USHORT)0xFFF) ); \
	 Z80_N = 1; \
	 Z80_Z = !(tmp = (USHORT)(HL - (ss) - \
			(USHORT)(Z80_C != 0)) ); \
	 Z80_C = ((ss) > HL)||(tmp>HL); \
	 Z80_P = ( ((H & BIT_7) != ((r) & BIT_7)) && ((HL ^ tmp) \
				& (USHORT)0x8000) ); \
     WZ = HL +1; /* OK */ \
	 Z80_S = ((HL = tmp) & (USHORT)0x8000); \
     Z80_X = H & (UCHAR)BIT_5; \
	 Z80_Y = H & (UCHAR)BIT_3; \
     Q = 1; \
}

// Z80_H = (((HL & (USHORT)0xFFF) - ((ss) & (USHORT)0xFFF)  - (Z80_C != 0)  ) & (USHORT)0x1000 ) > 0; 

/**** SBC overflow: -+ = + ou +- = - */

void sbc_hl_bc() sbc_hl_ss(BC, B);
void sbc_hl_de() sbc_hl_ss(DE, D);
void sbc_hl_hl() sbc_hl_ss(HL, H);
/*
void sbc_hl_hl()
{
	 LOCAL USHORT tmp;

	 T(15);
	 Z80_N = 1;
	 Z80_Z = !(Z80_S = Z80_X = Z80_Y =
			 HL = (Z80_C ? (USHORT)0xffff:(USHORT)0));
	 Z80_P = Z80_C = Z80_H = 0;
	 HL = tmp;
}
*/
void sbc_hl_sp() sbc_hl_ss(SP, SP >> 8);

#undef sbc_hl_ss

/*=========================================================================*
 *                            inc_ss                                       *
 *=========================================================================*/

#define inc_ss(r,TS) { T(TS); (r)++; Q=0; }

void inc_bc() inc_ss(BC,6);
void inc_de() inc_ss(DE,6);
void inc_hl() inc_ss(HL,6);
void inc_ix() inc_ss(IX,10);
void inc_iy() inc_ss(IY,10);
void inc_sp() inc_ss(SP,6);

#undef inc_ss

/*=========================================================================*
 *                            dec_ss                                       *
 *=========================================================================*/

#define dec_ss(r,TS) { T(TS); (r)--; Q=0; }

void dec_bc() dec_ss(BC,6);
void dec_de() dec_ss(DE,6);
void dec_hl() dec_ss(HL,6);
void dec_ix() dec_ss(IX,10);   /* oK */
void dec_iy() dec_ss(IY,10);   /* OK */
void dec_sp() dec_ss(SP,6);

#undef dec_ss

/* EOF: Math16Bi.c */
