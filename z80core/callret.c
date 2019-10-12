/* CallRet.c: call & return logic emulation.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

/*=========================================================================*
 *                            call_nn                                      *
 *=========================================================================*/
void call_nn()
{
#define call_nn() T(17);push((USHORT)((USHORT)PC+(USHORT)2));PutPC(WZ=Getnextword()) ; Q=0
/* WZ OK */

   call_nn();
}

/*=========================================================================*
 *                            call_cc_nn                                   *
 *=========================================================================*/

#define call_cc_nn(flag) { \
   if(flag) \
   { \
      call_nn(); \
   } \
   else \
   { \
     T(10); \
     WZ=Getnextword(); /* WZ OK */ \
   } \
}

//     PC += 2; 

void call_nz_nn() call_cc_nn(!Z80_Z);
void call_z_nn() call_cc_nn(Z80_Z);
void call_nc_nn() call_cc_nn(!Z80_C);
void call_c_nn() call_cc_nn(Z80_C);
void call_po_nn() call_cc_nn(!Z80_P);
void call_pe_nn() call_cc_nn(Z80_P);
void call_p_nn() call_cc_nn(!Z80_S);
void call_m_nn() call_cc_nn(Z80_S);

#undef call_cc_nn
#undef call_nn

/*=========================================================================*
 *                            ret                                          *
 *=========================================================================*/
void ret()
{
#define ret() PutPC(WZ = pop());Q=0;

 // WZ OK
    
   T(10);
   ret();
}

/*=========================================================================*
 *                            ret_cc                                       *
 *=========================================================================*/

#define ret_cc(flag) { \
   if(flag) \
   { \
      T(11); \
      ret(); \
   } \
   else \
      T(5); \
}


void ret_nz() ret_cc(!Z80_Z);
void ret_z() ret_cc(Z80_Z);
void ret_nc() ret_cc(!Z80_C);
void ret_c() ret_cc(Z80_C);
void ret_po() ret_cc(!Z80_P);
void ret_pe() ret_cc(Z80_P);
void ret_p() ret_cc(!Z80_S);
void ret_m() ret_cc(Z80_S);

#undef ret_cc

/*=========================================================================*
 *                            reti                                         *
 *=========================================================================*/
void reti()
{
   T(14);
   ret();
}

/*=========================================================================*
 *                            retn                                         *
 *=========================================================================*/
void retn()
{
   T(14);

   /* Restore interrupt state */
   IFF1 = IFF2;
   ret();
}

#undef ret

/*=========================================================================*
 *                            rst_p                                        *
 *=========================================================================*/

#define rst_p(p) { \
   T(11); \
   push((USHORT)PC); \
   PutPC(WZ=p); \
   Q=0; \
}

void rst_0()  rst_p(0);
void rst_8()  rst_p(8);
void rst_10() rst_p(0x10);
void rst_18() rst_p(0x18);
void rst_20() rst_p(0x20);
void rst_28() rst_p(0x28);
void rst_30() rst_p(0x30);
void rst_38() rst_p(0x38);

#undef rst_p

/* #include <stdio.h>

void rst_10()
{
    FILE * out;
    
    T(11);
    push((USHORT)PC);
    PutPC(0x10);
    out = fopen("/tmp/out", "a");
    fputc(A, out);
    fclose(out);
}
 
*/

/* EOF: CallRet.c */
