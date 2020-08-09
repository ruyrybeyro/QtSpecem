
/* Snap.h : Definitions for loading and saving snapshots.
 * Copyright 1996-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

enum { SNA_FMT, SNX_FMT, SNP_FMT, Z80_FMT, SIT_FMT, SP_FMT, RAW_FMT, ZX_FMT,
       PRG_FMT, ACH_FMT, TAP_FMT, BLK_FMT, ROM_FMT, DAT_FMT, SLT_FMT, SCR_FMT,
       SEM_FMT, PCX_FMT, INI_FMT, BMP_FMT, POK_FMT };

#ifndef _WIN32
//  Activate long file names on file dialog boxes under
// Windows 95
#define OFN_LONGNAMES   0x00200000;
#endif

/* EOF : Snap.h */
