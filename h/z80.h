
/* Z80.h : Registers and primitives.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "../h/quirks.h"

#include <stdint.h>

/* Do not change the order!
*/
struct Z80word_reg
{
   USHORT bc, de, hl, af, wz;
};

struct Z80byte_reg
{
#if !defined (M68000)
   UCHAR c, b, e, d, l, h, f, a, z, w;
#else
   UCHAR b, c, d, e, h, l, a, f, w, z;
#endif
};

union Z80Regs
{
   struct Z80word_reg x;
   struct Z80byte_reg k;
   USHORT wregs[5];
   UCHAR  bregs[10];
};


/*
 * Flags (F) :   7   6   5   4   3   2   1   0
 *             ---------------------------------
 *             | S | Z | X | H | Y | P | N | C |
 *             ---------------------------------
 */
struct CPU_flags
{
   USHORT  _S;   /* Sign                    */
   USHORT  _Z;   /* Zero                    */
   USHORT  _X;   /* Auxiliary Overflow  ?   */ /* Undocumented */
   USHORT  _H;   /* Half carry              */
   USHORT  _Y;   /* Auxliary Half carry ?   */ /* Undocumented */
   USHORT  _P;   /* Parity - overflow - P/V */
   USHORT  _N;   /* Subtraction             */
   USHORT  _C;   /* Carry                   */
};

struct Z80vars
{
   UCHAR TraceOn;
   UCHAR TraceOnCopy;
   USHORT PC;
   USHORT SP;
   uint32_t BreakPoint;
   // define if last operation affected flags
   USHORT Q;
   /* 'ticks' counter ; SPECTRUM Z80A - 3,5469MHz -
  70938 'ticks' between each int (50 per second)
   */
   unsigned long  clock_ticks;

   UCHAR   R, R_BIT7,  I;

   /* Interrupt mode - 0, 1 or 2 */
   UCHAR _IM;

   /* interrupt 'switch' interrupts and copy
    *
    *            IFF1  IFF2
    *  ----------------------
    *  | RESET  |  0  |  0  |
    *  |--------+-----+-----|
    *  | DI     |  0  |  0  |
    *  |--------+-----+-----|
    *  | EI     |  1  |  1  |
    *  |--------+-----+-----|
    *  | LD A,I |  .  |  .  | - P/V = IFF2
    *  |--------+-----+-----|
    *  | LD A,R |  .  |  .  | - P/V = IFF2
    *  |--------+-----+-----|
    *  | NMI    |  0  |  .  |
    *  |--------+-----+-----|
    *  | RETN   | IFF2|  .  |
    *  |--------+-----+-----|
    *  | INT    |  0  |  0  |
    *  |--------+-----+-----|
    *  | RETI   |  .  |  .  | - same as RET. (This instruction only exists
    *  ----------------------              to be recognized by external
    *                                      devices, so they know int is over)
    */
   UCHAR IFF1, IFF2;

};

#define PC Z80vars->PC
#define SP Z80vars->SP
#define BreakPoint Z80vars->BreakPoint
#define Q Z80vars->Q
#define clock_ticks Z80vars->clock_ticks
#define R Z80vars->R
#define R_BIT7 Z80vars->R_BIT7
#define I Z80vars->I
#define _IM Z80vars->_IM
#define IFF1 Z80vars->IFF1
#define IFF2 Z80vars->IFF2
#define TraceOn Z80vars->TraceOn
#define TraceOnCopy Z80vars->TraceOnCopy;

/* Shortcuts */
#define BC   Z80Regs->x.bc     /* Z80Wreg(0) -- BC   */
#define DE   Z80Regs->x.de     /* Z80Wreg(1) -- DE   */
#define HL   Z80Regs->x.hl     /* Z80Wreg(2) -- HL   */
#define AF   Z80Regs->x.af     /* Z80Wreg(3) -- AF   */
#define WZ   Z80Regs->x.wz     /* Z80Wreg(4) -- WZ   */
#define B    Z80Regs->k.b      /* Z80Breg(0) -- B    */
#define C    Z80Regs->k.c      /* Z80Breg(1) -- C    */
#define D    Z80Regs->k.d      /* Z80Breg(2) -- D    */
#define E    Z80Regs->k.e      /* Z80Breg(3) -- E    */
#define H    Z80Regs->k.h      /* Z80Breg(4) -- H    */
#define L    Z80Regs->k.l      /* Z80Breg(5) -- L    */
#define A    Z80Regs->k.a      /* Z80Breg(6) -- A    */
#define F    Z80Regs->k.f      /* Z80Breg(7) -- F    */
#define W    Z80Regs->k.w      /* Z80Breg(8) -- F    */
#define Z    Z80Regs->k.z      /* Z80Breg(9) -- F    */

/* Note: to use F or AF, first build_F() must be called
*/

#define Z80Wreg(elem) Z80Regs->wregs[elem]
#if !defined (M68000)
#define Z80Breg(elem) Z80Regs->bregs[(elem)^1]
#else
#define Z80Breg(elem) Z80Regs->bregs[elem]
#endif

/* Alternate register set */
/* In the 'real' Z80 there is no acess to the alternate register set as a bytes
  , but they might be needed to debugging purposes
*/
#define BC2  Z80Regs2->x.bc    /* Z80Wreg2(0) -- BC' */
#define DE2  Z80Regs2->x.de    /* Z80Wreg2(1) -- DE' */
#define HL2  Z80Regs2->x.hl    /* Z80Wreg2(2) -- HL' */
#define AF2  Z80Regs2->x.af    /* Z80Wreg2(3) -- AF' */
#define WZ2  Z80Regs2->x.wz    /* Z80Wreg2(4) -- AF' */
#define B2   Z80Regs2->k.b     /* Z80Breg2(0) -- B'  */
#define C2   Z80Regs2->k.c     /* Z80Breg2(1) -- C'  */
#define D2   Z80Regs2->k.d     /* Z80Breg2(2) -- D'  */
#define E2   Z80Regs2->k.e     /* Z80Breg2(3) -- E'  */
#define H2   Z80Regs2->k.h     /* Z80Breg2(4) -- H'  */
#define L2   Z80Regs2->k.l     /* Z80Breg2(5) -- L'  */
#define A2   Z80Regs2->k.a     /* Z80Breg2(6) -- A'  */
#define F2   Z80Regs2->k.f     /* Z80Breg2(7) -- F'  */

#define Z80Wreg2(elem) Z80Regs2->wregs[elem]
#if !defined (M68000)
#define Z80Breg2(elem) Z80Regs2->bregs[(elem)^1]
#else
#define Z80Breg2(elem) Z80Regs2->bregs[elem]
#endif

union Z80IX
{
   USHORT IX;
   UCHAR  bregs[2];
};

union Z80IY
{
   USHORT IY;
   UCHAR  bregs[2];
};

#define IX Z80IX->IX
#define IY Z80IY->IY

#if !defined (M68000)
#define HX Z80IX->bregs[1]
#define LX Z80IX->bregs[0]
#define HY Z80IY->bregs[1]
#define LY Z80IY->bregs[0]
#else
#define HX Z80IX->bregs[0]
#define LX Z80IX->bregs[1]
#define HY Z80IY->bregs[0]
#define LY Z80IY->bregs[1]
#endif

#define Z80_S flags->_S
#define Z80_Z flags->_Z
#define Z80_X flags->_X
#define Z80_H flags->_H
#define Z80_Y flags->_Y
#define Z80_P flags->_P
#define Z80_N flags->_N
#define Z80_C flags->_C


/* Clock ticks between each interrupt */
#define INT_TIME 69888L         /* Timing provided by Ian Collier */

/*
    Z80 Timings:

	Condition test       -->  1T
	Inc/Dec 8  bits reg  -->  1T
	Inc/Dec 16 bits reg  -->  2T
	PC+n                 -->  5T
	sum IX+n             -->  5T
	read a memory byte   -->  3T
	read a port   byte   -->  4T (3T + 1T in Wait)

        http://www.z80.info/interrup.htm
	Nmi int              --> 11T 
	Int IM 0             --> 17T 
	Int IM 1             --> 13T 
	Int IM 2             --> 19T
*/

/* EOF: Z80.h */
