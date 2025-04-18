/* Jump.c: Jump logic emulation.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

/*=========================================================================*
 *                            jp_nn                                        *
 *=========================================================================*/
void jp_nn()
{
#define jp_nn() PutPC(WZ = Getnextword()); Q=0;
// WZ - OK
    
   T(10);
   jp_nn();
}

/*=========================================================================*
 *                            jp_cc_nn                                     *
 *=========================================================================*/

#define jp_cc_nn(flag) { \
   T(10); \
   if(flag) \
   { \
      jp_nn(); \
   } \
   else \
      WZ = Getnextword(); /* OK */ \
}


void jp_nz_nn() jp_cc_nn(!Z80_Z);
void jp_z_nn()  jp_cc_nn(Z80_Z);
void jp_nc_nn() jp_cc_nn(!Z80_C);
void jp_c_nn()  jp_cc_nn(Z80_C);
void jp_po_nn() jp_cc_nn(!Z80_P);
void jp_pe_nn() jp_cc_nn(Z80_P);
void jp_p_nn()  jp_cc_nn(!Z80_S);
void jp_m_nn()  jp_cc_nn(Z80_S);

#undef jp_cc_nn
#undef jp_nn

/*=========================================================================*
 *                            jr_e                                         *
 *=========================================================================*/
void jr_e()
{
/* OK */
#define jr_e() (PC += ucharToUshort(readbyte(PC))+1);WZ=PC;Q=0;

   T(12);
   jr_e();
}

/*=========================================================================*
 *                            jr_cc_e                                      *
 *=========================================================================*/

#define jr_cc_e(flag) { \
   if(flag) \
   { \
      T(12); \
      jr_e(); \
   } \
   else \
   { \
      T(7); \
      ++PC; \
   } \
}


void jr_c_e()  jr_cc_e(Z80_C);
void jr_nc_e() jr_cc_e(!Z80_C);
void jr_z_e()  jr_cc_e(Z80_Z);
void jr_nz_e() jr_cc_e(!Z80_Z);

#undef jr_cc_e

/*=========================================================================*
 *                            jp_hl                                        *
 *=========================================================================*/
void jp_hl()
{
   T(4);
   PutPC(HL); /* WZ not used */
   Q = 0;
}

/*=========================================================================*
 *                            jp_ix                                        *
 *=========================================================================*/
void jp_ix()
{
   T(8);
   PutPC(IX); /* WZ not used */
   Q = 0;
}

/*=========================================================================*
 *                            jp_iy                                        *
 *=========================================================================*/
void jp_iy()
{
   T(8);
   PutPC(IY);
   Q = 0;
}

/*=========================================================================*
 *                            djnz_e                                       *
 *=========================================================================*/
void djnz_e()
{
   if(--B)
   {
      T(13);
      jr_e();
   }
   else
   {
      T(8);
      PC++;
   }
}

#undef jr_e

/* EOF : Jump.c */
