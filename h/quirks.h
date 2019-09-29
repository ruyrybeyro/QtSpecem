
/* Quirks.h : definitions for portability of Z80 kernel.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

/* #define WINDOWS_SOUND 1 */

/* in 68000 put: #define M68000 1 */
#define i8086

#undef __MSDOS__

#define P_TYPE      long

/* in Borland C */
#define LOCAL static

/* for parameter passage - void or char */
#if defined(__NPAR_ANSI)
#define VOID char
#else
#define VOID void
#endif

/* Some compilers define the macros below
*/
#undef USHORT
#undef UCHAR
#undef UINT

#define USHORT unsigned short
#define UCHAR  unsigned char
#define UINT   unsigned int

#define BIT_13 ((USHORT)0x2000)
#define BIT_11 ((USHORT)0x800)
#define BIT_7 ((USHORT)0x80)
#define BIT_6 ((USHORT)0x40)
#define BIT_5 ((USHORT)0x20)
#define BIT_4 ((USHORT)0x10)
#define BIT_3 ((USHORT)0x08)
#define BIT_2 ((USHORT)0x04)
#define BIT_1 ((USHORT)0x02)
#define BIT_0 ((USHORT)0x01)

#define NDEBUG 1

#define BOOLEAN UCHAR

/* EOF: Quirks.h */
