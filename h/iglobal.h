
/* IGlobal.h : Functions prototypes.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

/* LD8BITS.c  */

void ld_b_b(void);
void ld_b_c(void);
void ld_b_d(void);
void ld_b_e(void);
void ld_b_h(void);
void ld_b_Ix(void);
void ld_b_Iy(void);
void ld_b_l(void);
void ld_b_iX(void);
void ld_b_iY(void);
void ld_b_a(void);
void ld_c_b(void);
void ld_c_c(void);
void ld_c_d(void);
void ld_c_e(void);
void ld_c_h(void);
void ld_c_Ix(void);
void ld_c_Iy(void);
void ld_c_l(void);
void ld_c_iX(void);
void ld_c_iY(void);
void ld_c_a(void);
void ld_d_b(void);
void ld_d_c(void);
void ld_d_d(void);
void ld_d_e(void);
void ld_d_h(void);
void ld_d_Ix(void);
void ld_d_Iy(void);
void ld_d_l(void);
void ld_d_iX(void);
void ld_d_iY(void);
void ld_d_a(void);
void ld_e_b(void);
void ld_e_c(void);
void ld_e_d(void);
void ld_e_e(void);
void ld_e_h(void);
void ld_e_Ix(void);
void ld_e_Iy(void);
void ld_e_l(void);
void ld_e_iX(void);
void ld_e_iY(void);
void ld_e_a(void);
void ld_h_b(void);
void ld_h_c(void);
void ld_h_d(void);
void ld_h_e(void);
void ld_h_h(void);
void ld_h_l(void);
void ld_h_a(void);
void ld_Ix_b(void);
void ld_Ix_c(void);
void ld_Ix_d(void);
void ld_Ix_e(void);
void ld_Ix_Ix(void);
void ld_Ix_iX(void);
void ld_Ix_a(void);
void ld_Iy_b(void);
void ld_Iy_c(void);
void ld_Iy_d(void);
void ld_Iy_e(void);
void ld_Iy_Iy(void);
void ld_Iy_iY(void);
void ld_Iy_a(void);
void ld_l_b(void);
void ld_l_c(void);
void ld_l_d(void);
void ld_l_e(void);
void ld_l_h(void);
void ld_l_l(void);
void ld_l_a(void);
void ld_iX_b(void);
void ld_iX_c(void);
void ld_iX_d(void);
void ld_iX_e(void);
void ld_iX_iX(void);
void ld_iX_Ix(void);
void ld_iX_a(void);
void ld_iY_b(void);
void ld_iY_c(void);
void ld_iY_d(void);
void ld_iY_e(void);
void ld_iY_iY(void);
void ld_iY_Iy(void);
void ld_iY_a(void);
void ld_a_b(void);
void ld_a_c(void);
void ld_a_d(void);
void ld_a_e(void);
void ld_a_h(void);
void ld_a_Ix(void);
void ld_a_Iy(void);
void ld_a_l(void);
void ld_a_iX(void);
void ld_a_iY(void);
void ld_a_a(void);
void ld_b_n(void);
void ld_c_n(void);
void ld_d_n(void);
void ld_e_n(void);
void ld_h_n(void);
void ld_Ix_n(void);
void ld_Iy_n(void);
void ld_l_n(void);
void ld_iX_n(void);
void ld_iY_n(void);
void ld_a_n(void);
void ld_b_phl(void);
void ld_c_phl(void);
void ld_d_phl(void);
void ld_e_phl(void);
void ld_h_phl(void);
void ld_l_phl(void);
void ld_a_phl(void);
void ld_b_pix(void);
void ld_c_pix(void);
void ld_d_pix(void);
void ld_e_pix(void);
void ld_h_pix(void);
void ld_l_pix(void);
void ld_a_pix(void);
void ld_b_piy(void);
void ld_c_piy(void);
void ld_d_piy(void);
void ld_e_piy(void);
void ld_h_piy(void);
void ld_l_piy(void);
void ld_a_piy(void);
void ld_phl_b(void);
void ld_phl_c(void);
void ld_phl_d(void);
void ld_phl_e(void);
void ld_phl_h(void);
void ld_phl_l(void);
void ld_phl_a(void);
void ld_pix_b(void);
void ld_pix_c(void);
void ld_pix_d(void);
void ld_pix_e(void);
void ld_pix_h(void);
void ld_pix_l(void);
void ld_pix_a(void);
void ld_piy_b(void);
void ld_piy_c(void);
void ld_piy_d(void);
void ld_piy_e(void);
void ld_piy_h(void);
void ld_piy_l(void);
void ld_piy_a(void);
void ld_phl_n(void);
void ld_pix_n(void);
void ld_piy_n(void);
void ld_a_pbc(void);
void ld_a_pde(void);
void ld_a_pnn(void);
void ld_pbc_a(void);
void ld_pde_a(void);
void ld_pnn_a(void);
void ld_a_i(void);
void ld_a_r(void);
void ld_i_a(void);
void ld_r_a(void);

/* LD16BITS.c */
void ld_bc_nn(void);
void ld_de_nn(void);
void ld_hl_nn(void);
void ld_ix_nn(void);
void ld_iy_nn(void);
void ld_sp_nn(void);
void ld_hl_pnn(void);
void ld_ix_pnn(void);
void ld_iy_pnn(void);
void ld_bc_pnn(void);
void ld_de_pnn(void);
void ld_sp_pnn(void);
void ld_hl_pnnED(void);
void ld_pnn_hl(void);
void ld_pnn_ix(void);
void ld_pnn_iy(void);
void ld_pnn_bc(void);
void ld_pnn_de(void);
void ld_pnn_sp(void);
void ld_pnn_hlED(void);
void ld_sp_hl(void);
void ld_sp_ix(void);
void ld_sp_iy(void);
void push_bc(void);
void push_de(void);
void push_hl(void);
void push_ix(void);
void push_iy(void);
void push_af(void);
void pop_bc(void);
void pop_de(void);
void pop_hl(void);
void pop_ix(void);
void pop_iy(void);
void pop_af(void);

/* EXCTRANF.c */

void ex_de_hl(void);
void ex_de_ix(void);
void ex_de_iy(void);
void ex_af_af2(void);
void exx(void);
void ex_psp_hl(void);
void ex_psp_ix(void);
void ex_psp_iy(void);
void ldi(void);
void ldir(void);
void ldd(void);
void lddr(void);
void cpi(void);
void cpir(void);
void cpd(void);
void cpdr(void);

/* MATH8BIT.c */

void add_a_b(void);
void add_a_c(void);
void add_a_d(void);
void add_a_e(void);
void add_a_h(void);
void add_a_Ix(void);
void add_a_Iy(void);
void add_a_l(void);
void add_a_iX(void);
void add_a_iY(void);
void add_a_a(void);
void add_a_n(void);
void add_a_phl(void);
void add_a_pix(void);
void add_a_piy(void);
void adc_a_b(void);
void adc_a_c(void);
void adc_a_d(void);
void adc_a_e(void);
void adc_a_h(void);
void adc_a_Ix(void);
void adc_a_Iy(void);
void adc_a_l(void);
void adc_a_iX(void);
void adc_a_iY(void);
void adc_a_a(void);
void adc_a_n(void);
void adc_a_phl(void);
void adc_a_pix(void);
void adc_a_piy(void);
void sub_b(void);
void sub_c(void);
void sub_d(void);
void sub_e(void);
void sub_h(void);
void sub_Ix(void);
void sub_Iy(void);
void sub_l(void);
void sub_iX(void);
void sub_iY(void);
void sub_a(void);
void sub_n(void);
void sub_phl(void);
void sub_pix(void);
void sub_piy(void);
void sbc_a_b(void);
void sbc_a_c(void);
void sbc_a_d(void);
void sbc_a_e(void);
void sbc_a_h(void);
void sbc_a_Ix(void);
void sbc_a_Iy(void);
void sbc_a_l(void);
void sbc_a_iX(void);
void sbc_a_iY(void);
void sbc_a_a(void);
void sbc_a_n(void);
void sbc_a_phl(void);
void sbc_a_pix(void);
void sbc_a_piy(void);
void and_b(void);
void and_c(void);
void and_d(void);
void and_e(void);
void and_h(void);
void and_Ix(void);
void and_Iy(void);
void and_l(void);
void and_iX(void);
void and_iY(void);
void and_a(void);
void and_n(void);
void and_phl(void);
void and_pix(void);
void and_piy(void);
void or_b(void);
void or_c(void);
void or_d(void);
void or_e(void);
void or_h(void);
void or_Ix(void);
void or_Iy(void);
void or_l(void);
void or_iX(void);
void or_iY(void);
void or_a(void);
void or_n(void);
void or_phl(void);
void or_pix(void);
void or_piy(void);
void xor_b(void);
void xor_c(void);
void xor_d(void);
void xor_e(void);
void xor_h(void);
void xor_Ix(void);
void xor_Iy(void);
void xor_l(void);
void xor_iX(void);
void xor_iY(void);
void xor_a(void);
void xor_n(void);
void xor_phl(void);
void xor_pix(void);
void xor_piy(void);
void cp_b(void);
void cp_c(void);
void cp_d(void);
void cp_e(void);
void cp_h(void);
void cp_Ix(void);
void cp_Iy(void);
void cp_l(void);
void cp_iX(void);
void cp_iY(void);
void cp_a(void);
void cp_n(void);
void cp_phl(void);
void cp_pix(void);
void cp_piy(void);
void inc_b(void);
void inc_c(void);
void inc_d(void);
void inc_e(void);
void inc_h(void);
void inc_Ix(void);
void inc_Iy(void);
void inc_l(void);
void inc_iX(void);
void inc_iY(void);
void inc_a(void);
void inc_phl(void);
void inc_pix(void);
void inc_piy(void);
void dec_b(void);
void dec_c(void);
void dec_d(void);
void dec_e(void);
void dec_h(void);
void dec_Ix(void);
void dec_Iy(void);
void dec_l(void);
void dec_iX(void);
void dec_iY(void);
void dec_a(void);
void dec_phl(void);
void dec_pix(void);
void dec_piy(void);

/* MATH16BI.C */

void add_hl_bc(void);
void add_hl_de(void);
void add_hl_hl(void);
void add_hl_sp(void);
void add_ix_bc(void);
void add_ix_de(void);
void add_ix_ix(void);
void add_ix_sp(void);
void add_iy_bc(void);
void add_iy_de(void);
void add_iy_iy(void);
void add_iy_sp(void);
void adc_hl_bc(void);
void adc_hl_de(void);
void adc_hl_hl(void);
void adc_hl_sp(void);
void sbc_hl_bc(void);
void sbc_hl_de(void);
void sbc_hl_hl(void);
void sbc_hl_sp(void);
void inc_bc(void);
void inc_de(void);
void inc_hl(void);
void inc_ix(void);
void inc_iy(void);
void inc_sp(void);
void dec_bc(void);
void dec_de(void);
void dec_hl(void);
void dec_ix(void);
void dec_iy(void);
void dec_sp(void);

/* MISC.c     */

void daa(void);
void cpl(void);
void neg(void);
void ccf(void);
void scf(void);
void nop(void);
void halt(void);
void di(void);
void ei(void);
void im_0(void);
void im__(void);
void im_1(void);
void im_2(void);

/*** auxiliary routines ***/
void dummyED(void);
void dummyIX(void);
void dummyIY(void);
USHORT parity(unsigned char);

/* ROTATE.c   */

void rlca(void);
void rla(void);
void rrca(void);
void rra(void);
void rlc_b(void);
void rlc_c(void);
void rlc_d(void);
void rlc_e(void);
void rlc_h(void);
void rlc_Ix(void);
void rlc_Iy(void);
void rlc_l(void);
void rlc_iX(void);
void rlc_iY(void);
void rlc_a(void);
void rlc_phl(void);
void rlc_pix(void);
void rlc_piy(void);
void rl_b(void);
void rl_c(void);
void rl_d(void);
void rl_e(void);
void rl_h(void);
void rl_Ix(void);
void rl_Iy(void);
void rl_l(void);
void rl_iX(void);
void rl_iY(void);
void rl_a(void);
void rl_phl(void);
void rl_pix(void);
void rl_piy(void);
void rrc_b(void);
void rrc_c(void);
void rrc_d(void);
void rrc_e(void);
void rrc_h(void);
void rrc_Ix(void);
void rrc_Iy(void);
void rrc_l(void);
void rrc_iX(void);
void rrc_iY(void);
void rrc_a(void);
void rrc_phl(void);
void rrc_pix(void);
void rrc_piy(void);
void rr_b(void);
void rr_c(void);
void rr_d(void);
void rr_e(void);
void rr_h(void);
void rr_Ix(void);
void rr_Iy(void);
void rr_l(void);
void rr_iX(void);
void rr_iY(void);
void rr_a(void);
void rr_phl(void);
void rr_pix(void);
void rr_piy(void);
void sla_b(void);
void sla_c(void);
void sla_d(void);
void sla_e(void);
void sla_h(void);
void sla_Ix(void);
void sla_Iy(void);
void sla_l(void);
void sla_iX(void);
void sla_iY(void);
void sla_a(void);
void sla_phl(void);
void sla_pix(void);
void sla_piy(void);
void sll_b(void);
void sll_c(void);
void sll_d(void);
void sll_e(void);
void sll_h(void);
void sll_Ix(void);
void sll_Iy(void);
void sll_l(void);
void sll_iX(void);
void sll_iY(void);
void sll_a(void);
void sll_phl(void);
void sll_pix(void);
void sll_piy(void);
void sra_b(void);
void sra_c(void);
void sra_d(void);
void sra_e(void);
void sra_h(void);
void sra_Ix(void);
void sra_Iy(void);
void sra_l(void);
void sra_iX(void);
void sra_iY(void);
void sra_a(void);
void sra_phl(void);
void sra_pix(void);
void sra_piy(void);
void sra_iX(void);
void sra_iY(void);
void srl_b(void);
void srl_c(void);
void srl_d(void);
void srl_e(void);
void srl_h(void);
void srl_Ix(void);
void srl_Iy(void);
void srl_l(void);
void srl_iX(void);
void srl_iY(void);
void srl_a(void);
void srl_phl(void);
void srl_pix(void);
void srl_piy(void);
void rld(void);
void rrd(void);
/* -------------*/
void rlc_pixb(void);
void rlc_pixc(void);
void rlc_pixd(void);
void rlc_pixe(void);
void rlc_pixa(void);
void rlc_pixh(void);
void rlc_pixl(void);
void rlc_piyb(void);
void rlc_piyc(void);
void rlc_piyd(void);
void rlc_piye(void);
void rlc_piya(void);
void rlc_piyh(void);
void rlc_piyl(void);
void rrc_pixb(void);
void rrc_pixc(void);
void rrc_pixd(void);
void rrc_pixe(void);
void rrc_pixa(void);
void rrc_pixh(void);
void rrc_pixl(void);
void rrc_piyb(void);
void rrc_piyc(void);
void rrc_piyd(void);
void rrc_piye(void);
void rrc_piya(void);
void rrc_piyh(void);
void rrc_piyl(void);
void rl_pixb(void);
void rl_pixc(void);
void rl_pixd(void);
void rl_pixe(void);
void rl_pixa(void);
void rl_pixh(void);
void rl_pixl(void);
void rl_piyb(void);
void rl_piyc(void);
void rl_piyd(void);
void rl_piye(void);
void rl_piya(void);
void rl_piyh(void);
void rl_piyl(void);
void rr_pixb(void);
void rr_pixc(void);
void rr_pixd(void);
void rr_pixe(void);
void rr_pixa(void);
void rr_pixh(void);
void rr_pixl(void);
void rr_piyb(void);
void rr_piyc(void);
void rr_piyd(void);
void rr_piye(void);
void rr_piya(void);
void rr_piyh(void);
void rr_piyl(void);
void sla_pixb(void);
void sla_pixc(void);
void sla_pixd(void);
void sla_pixe(void);
void sla_pixa(void);
void sla_pixh(void);
void sla_pixl(void);
void sla_piyb(void);
void sla_piyc(void);
void sla_piyd(void);
void sla_piye(void);
void sla_piya(void);
void sla_piyh(void);
void sla_piyl(void);
void sra_pixb(void);
void sra_pixc(void);
void sra_pixd(void);
void sra_pixe(void);
void sra_pixa(void);
void sra_pixh(void);
void sra_pixl(void);
void sra_piyb(void);
void sra_piyc(void);
void sra_piyd(void);
void sra_piye(void);
void sra_piya(void);
void sra_piyh(void);
void sra_piyl(void);
void sll_pixb(void);
void sll_pixc(void);
void sll_pixd(void);
void sll_pixe(void);
void sll_pixa(void);
void sll_pixh(void);
void sll_pixl(void);
void sll_piyb(void);
void sll_piyc(void);
void sll_piyd(void);
void sll_piye(void);
void sll_piya(void);
void sll_piyh(void);
void sll_piyl(void);
void srl_pixb(void);
void srl_pixc(void);
void srl_pixd(void);
void srl_pixe(void);
void srl_pixh(void);
void srl_pixl(void);
void srl_pixa(void);
void srl_piyb(void);
void srl_piyc(void);
void srl_piyd(void);
void srl_piye(void);
void srl_piya(void);
void srl_piyh(void);
void srl_piyl(void);
void res_0_pixb(void);
void res_0_pixc(void);
void res_0_pixd(void);
void res_0_pixe(void);
void res_0_pixa(void);
void res_0_pixh(void);
void res_0_pixl(void);
void res_1_pixb(void);
void res_1_pixc(void);
void res_1_pixd(void);
void res_1_pixe(void);
void res_1_pixa(void);
void res_1_pixh(void);
void res_1_pixl(void);
void res_2_pixb(void);
void res_2_pixc(void);
void res_2_pixd(void);
void res_2_pixe(void);
void res_2_pixa(void);
void res_2_pixh(void);
void res_2_pixl(void);
void res_3_pixb(void);
void res_3_pixc(void);
void res_3_pixd(void);
void res_3_pixe(void);
void res_3_pixa(void);
void res_3_pixh(void);
void res_3_pixl(void);
void res_4_pixb(void);
void res_4_pixc(void);
void res_4_pixd(void);
void res_4_pixe(void);
void res_4_pixa(void);
void res_4_pixh(void);
void res_4_pixl(void);
void res_5_pixb(void);
void res_5_pixc(void);
void res_5_pixd(void);
void res_5_pixe(void);
void res_5_pixa(void);
void res_5_pixh(void);
void res_5_pixl(void);
void res_6_pixb(void);
void res_6_pixc(void);
void res_6_pixd(void);
void res_6_pixe(void);
void res_6_pixa(void);
void res_6_pixh(void);
void res_6_pixl(void);
void res_7_pixb(void);
void res_7_pixc(void);
void res_7_pixd(void);
void res_7_pixe(void);
void res_7_pixa(void);
void res_7_pixh(void);
void res_7_pixl(void);
void res_0_piyb(void);
void res_0_piyc(void);
void res_0_piyd(void);
void res_0_piye(void);
void res_0_piya(void);
void res_0_piyh(void);
void res_0_piyl(void);
void res_1_piyb(void);
void res_1_piyc(void);
void res_1_piyd(void);
void res_1_piye(void);
void res_1_piya(void);
void res_1_piyh(void);
void res_1_piyl(void);
void res_2_piyb(void);
void res_2_piyc(void);
void res_2_piyd(void);
void res_2_piye(void);
void res_2_piya(void);
void res_2_piyh(void);
void res_2_piyl(void);
void res_3_piyb(void);
void res_3_piyc(void);
void res_3_piyd(void);
void res_3_piye(void);
void res_3_piya(void);
void res_3_piyh(void);
void res_3_piyl(void);
void res_4_piyb(void);
void res_4_piyc(void);
void res_4_piyd(void);
void res_4_piye(void);
void res_4_piya(void);
void res_4_piyh(void);
void res_4_piyl(void);
void res_5_piyb(void);
void res_5_piyc(void);
void res_5_piyd(void);
void res_5_piye(void);
void res_5_piya(void);
void res_5_piyh(void);
void res_5_piyl(void);
void res_6_piyb(void);
void res_6_piyc(void);
void res_6_piyd(void);
void res_6_piye(void);
void res_6_piya(void);
void res_6_piyh(void);
void res_6_piyl(void);
void res_7_piyb(void);
void res_7_piyc(void);
void res_7_piyd(void);
void res_7_piye(void);
void res_7_piya(void);
void res_7_piyh(void);
void res_7_piyl(void);
void set_0_pixb(void);
void set_0_pixc(void);
void set_0_pixd(void);
void set_0_pixe(void);
void set_0_pixa(void);
void set_0_pixh(void);
void set_0_pixl(void);
void set_1_pixb(void);
void set_1_pixc(void);
void set_1_pixd(void);
void set_1_pixe(void);
void set_1_pixa(void);
void set_1_pixh(void);
void set_1_pixl(void);
void set_2_pixb(void);
void set_2_pixc(void);
void set_2_pixd(void);
void set_2_pixe(void);
void set_2_pixa(void);
void set_2_pixh(void);
void set_2_pixl(void);
void set_3_pixb(void);
void set_3_pixc(void);
void set_3_pixd(void);
void set_3_pixe(void);
void set_3_pixa(void);
void set_3_pixh(void);
void set_3_pixl(void);
void set_4_pixb(void);
void set_4_pixc(void);
void set_4_pixd(void);
void set_4_pixe(void);
void set_4_pixa(void);
void set_4_pixh(void);
void set_4_pixl(void);
void set_5_pixb(void);
void set_5_pixc(void);
void set_5_pixd(void);
void set_5_pixe(void);
void set_5_pixa(void);
void set_5_pixh(void);
void set_5_pixl(void);
void set_6_pixb(void);
void set_6_pixc(void);
void set_6_pixd(void);
void set_6_pixe(void);
void set_6_pixa(void);
void set_6_pixh(void);
void set_6_pixl(void);
void set_7_pixb(void);
void set_7_pixc(void);
void set_7_pixd(void);
void set_7_pixe(void);
void set_7_pixa(void);
void set_7_pixh(void);
void set_7_pixl(void);
void set_0_piyb(void);
void set_0_piyc(void);
void set_0_piyd(void);
void set_0_piye(void);
void set_0_piya(void);
void set_0_piyh(void);
void set_0_piyl(void);
void set_1_piyb(void);
void set_1_piyc(void);
void set_1_piyd(void);
void set_1_piye(void);
void set_1_piya(void);
void set_1_piyh(void);
void set_1_piyl(void);
void set_2_piyb(void);
void set_2_piyc(void);
void set_2_piyd(void);
void set_2_piye(void);
void set_2_piya(void);
void set_2_piyh(void);
void set_2_piyl(void);
void set_3_piyb(void);
void set_3_piyc(void);
void set_3_piyd(void);
void set_3_piye(void);
void set_3_piya(void);
void set_3_piyh(void);
void set_3_piyl(void);
void set_4_piyb(void);
void set_4_piyc(void);
void set_4_piyd(void);
void set_4_piye(void);
void set_4_piya(void);
void set_4_piyh(void);
void set_4_piyl(void);
void set_5_piyb(void);
void set_5_piyc(void);
void set_5_piyd(void);
void set_5_piye(void);
void set_5_piya(void);
void set_5_piyh(void);
void set_5_piyl(void);
void set_6_piyb(void);
void set_6_piyc(void);
void set_6_piyd(void);
void set_6_piye(void);
void set_6_piya(void);
void set_6_piyh(void);
void set_6_piyl(void);
void set_7_piyb(void);
void set_7_piyc(void);
void set_7_piyd(void);
void set_7_piye(void);
void set_7_piya(void);
void set_7_piyh(void);
void set_7_piyl(void);

/* BITS.c     */

void bit_7_b(void);
void bit_6_b(void);
void bit_5_b(void);
void bit_4_b(void);
void bit_3_b(void);
void bit_2_b(void);
void bit_1_b(void);
void bit_0_b(void);
void bit_7_c(void);
void bit_6_c(void);
void bit_5_c(void);
void bit_4_c(void);
void bit_3_c(void);
void bit_2_c(void);
void bit_1_c(void);
void bit_0_c(void);
void bit_7_d(void);
void bit_6_d(void);
void bit_5_d(void);
void bit_4_d(void);
void bit_3_d(void);
void bit_2_d(void);
void bit_1_d(void);
void bit_0_d(void);
void bit_7_e(void);
void bit_6_e(void);
void bit_5_e(void);
void bit_4_e(void);
void bit_3_e(void);
void bit_2_e(void);
void bit_1_e(void);
void bit_0_e(void);
void bit_7_h(void);
void bit_6_h(void);
void bit_5_h(void);
void bit_4_h(void);
void bit_3_h(void);
void bit_2_h(void);
void bit_1_h(void);
void bit_0_h(void);
void bit_7_Ix(void);
void bit_6_Ix(void);
void bit_5_Ix(void);
void bit_4_Ix(void);
void bit_3_Ix(void);
void bit_2_Ix(void);
void bit_1_Ix(void);
void bit_0_Ix(void);
void bit_7_Iy(void);
void bit_6_Iy(void);
void bit_5_Iy(void);
void bit_4_Iy(void);
void bit_3_Iy(void);
void bit_2_Iy(void);
void bit_1_Iy(void);
void bit_0_Iy(void);
void bit_7_l(void);
void bit_6_l(void);
void bit_5_l(void);
void bit_4_l(void);
void bit_3_l(void);
void bit_2_l(void);
void bit_1_l(void);
void bit_0_l(void);
void bit_7_iX(void);
void bit_6_iX(void);
void bit_5_iX(void);
void bit_4_iX(void);
void bit_3_iX(void);
void bit_2_iX(void);
void bit_1_iX(void);
void bit_0_iX(void);
void bit_7_iY(void);
void bit_6_iY(void);
void bit_5_iY(void);
void bit_4_iY(void);
void bit_3_iY(void);
void bit_2_iY(void);
void bit_1_iY(void);
void bit_0_iY(void);
void bit_7_a(void);
void bit_6_a(void);
void bit_5_a(void);
void bit_4_a(void);
void bit_3_a(void);
void bit_2_a(void);
void bit_1_a(void);
void bit_0_a(void);
void bit_7_phl(void);
void bit_6_phl(void);
void bit_5_phl(void);
void bit_4_phl(void);
void bit_3_phl(void);
void bit_2_phl(void);
void bit_1_phl(void);
void bit_0_phl(void);
void bit_7_pix(void);
void bit_6_pix(void);
void bit_5_pix(void);
void bit_4_pix(void);
void bit_3_pix(void);
void bit_2_pix(void);
void bit_1_pix(void);
void bit_0_pix(void);
void bit_7_piy(void);
void bit_6_piy(void);
void bit_5_piy(void);
void bit_4_piy(void);
void bit_3_piy(void);
void bit_2_piy(void);
void bit_1_piy(void);
void bit_0_piy(void);
void set_7_b(void);
void set_6_b(void);
void set_5_b(void);
void set_4_b(void);
void set_3_b(void);
void set_2_b(void);
void set_1_b(void);
void set_0_b(void);
void set_7_c(void);
void set_6_c(void);
void set_5_c(void);
void set_4_c(void);
void set_3_c(void);
void set_2_c(void);
void set_1_c(void);
void set_0_c(void);
void set_7_d(void);
void set_6_d(void);
void set_5_d(void);
void set_4_d(void);
void set_3_d(void);
void set_2_d(void);
void set_1_d(void);
void set_0_d(void);
void set_7_e(void);
void set_6_e(void);
void set_5_e(void);
void set_4_e(void);
void set_3_e(void);
void set_2_e(void);
void set_1_e(void);
void set_0_e(void);
void set_7_h(void);
void set_6_h(void);
void set_5_h(void);
void set_4_h(void);
void set_3_h(void);
void set_2_h(void);
void set_1_h(void);
void set_0_h(void);
void set_7_Ix(void);
void set_6_Ix(void);
void set_5_Ix(void);
void set_4_Ix(void);
void set_3_Ix(void);
void set_2_Ix(void);
void set_1_Ix(void);
void set_0_Ix(void);
void set_7_Iy(void);
void set_6_Iy(void);
void set_5_Iy(void);
void set_4_Iy(void);
void set_3_Iy(void);
void set_2_Iy(void);
void set_1_Iy(void);
void set_0_Iy(void);
void set_7_l(void);
void set_6_l(void);
void set_5_l(void);
void set_4_l(void);
void set_3_l(void);
void set_2_l(void);
void set_1_l(void);
void set_0_l(void);
void set_7_iX(void);
void set_6_iX(void);
void set_5_iX(void);
void set_4_iX(void);
void set_3_iX(void);
void set_2_iX(void);
void set_1_iX(void);
void set_0_iX(void);
void set_7_iY(void);
void set_6_iY(void);
void set_5_iY(void);
void set_4_iY(void);
void set_3_iY(void);
void set_2_iY(void);
void set_1_iY(void);
void set_0_iY(void);
void set_7_a(void);
void set_6_a(void);
void set_5_a(void);
void set_4_a(void);
void set_3_a(void);
void set_2_a(void);
void set_1_a(void);
void set_0_a(void);
void set_7_phl(void);
void set_6_phl(void);
void set_5_phl(void);
void set_4_phl(void);
void set_3_phl(void);
void set_2_phl(void);
void set_1_phl(void);
void set_0_phl(void);
void set_7_pix(void);
void set_6_pix(void);
void set_5_pix(void);
void set_4_pix(void);
void set_3_pix(void);
void set_2_pix(void);
void set_1_pix(void);
void set_0_pix(void);
void set_7_piy(void);
void set_6_piy(void);
void set_5_piy(void);
void set_4_piy(void);
void set_3_piy(void);
void set_2_piy(void);
void set_1_piy(void);
void set_0_piy(void);
void res_7_b(void);
void res_6_b(void);
void res_5_b(void);
void res_4_b(void);
void res_3_b(void);
void res_2_b(void);
void res_1_b(void);
void res_0_b(void);
void res_7_c(void);
void res_6_c(void);
void res_5_c(void);
void res_4_c(void);
void res_3_c(void);
void res_2_c(void);
void res_1_c(void);
void res_0_c(void);
void res_7_d(void);
void res_6_d(void);
void res_5_d(void);
void res_4_d(void);
void res_3_d(void);
void res_2_d(void);
void res_1_d(void);
void res_0_d(void);
void res_7_e(void);
void res_6_e(void);
void res_5_e(void);
void res_4_e(void);
void res_3_e(void);
void res_2_e(void);
void res_1_e(void);
void res_0_e(void);
void res_7_h(void);
void res_6_h(void);
void res_5_h(void);
void res_4_h(void);
void res_3_h(void);
void res_2_h(void);
void res_1_h(void);
void res_0_h(void);
void res_7_Ix(void);
void res_6_Ix(void);
void res_5_Ix(void);
void res_4_Ix(void);
void res_3_Ix(void);
void res_2_Ix(void);
void res_1_Ix(void);
void res_0_Ix(void);
void res_7_Iy(void);
void res_6_Iy(void);
void res_5_Iy(void);
void res_4_Iy(void);
void res_3_Iy(void);
void res_2_Iy(void);
void res_1_Iy(void);
void res_0_Iy(void);
void res_7_l(void);
void res_6_l(void);
void res_5_l(void);
void res_4_l(void);
void res_3_l(void);
void res_2_l(void);
void res_1_l(void);
void res_0_l(void);
void res_7_iX(void);
void res_6_iX(void);
void res_5_iX(void);
void res_4_iX(void);
void res_3_iX(void);
void res_2_iX(void);
void res_1_iX(void);
void res_0_iX(void);
void res_7_iY(void);
void res_6_iY(void);
void res_5_iY(void);
void res_4_iY(void);
void res_3_iY(void);
void res_2_iY(void);
void res_1_iY(void);
void res_0_iY(void);
void res_7_a(void);
void res_6_a(void);
void res_5_a(void);
void res_4_a(void);
void res_3_a(void);
void res_2_a(void);
void res_1_a(void);
void res_0_a(void);
void res_7_phl(void);
void res_6_phl(void);
void res_5_phl(void);
void res_4_phl(void);
void res_3_phl(void);
void res_2_phl(void);
void res_1_phl(void);
void res_0_phl(void);
void res_7_pix(void);
void res_6_pix(void);
void res_5_pix(void);
void res_4_pix(void);
void res_3_pix(void);
void res_2_pix(void);
void res_1_pix(void);
void res_0_pix(void);
void res_7_piy(void);
void res_6_piy(void);
void res_5_piy(void);
void res_4_piy(void);
void res_3_piy(void);
void res_2_piy(void);
void res_1_piy(void);
void res_0_piy(void);

/* JUMP.c     */

void jp_nn(void);
void jp_nz_nn(void);
void jp_z_nn(void);
void jp_nc_nn(void);
void jp_c_nn(void);
void jp_po_nn(void);
void jp_pe_nn(void);
void jp_p_nn(void);
void jp_m_nn(void);
void jr_e(void);
void jr_c_e(void);
void jr_nc_e(void);
void jr_z_e(void);
void jr_nz_e(void);
void jp_hl(void);
void jp_ix(void);
void jp_iy(void);
void djnz_e(void);

/* CALLRET.c  */

void call_nn(void);
void call_nz_nn(void);
void call_z_nn(void);
void call_nc_nn(void);
void call_c_nn(void);
void call_po_nn(void);
void call_pe_nn(void);
void call_p_nn(void);
void call_m_nn(void);
void ret(void);
void ret_nz(void);
void ret_z(void);
void ret_nc(void);
void ret_c(void);
void ret_po(void);
void ret_pe(void);
void ret_p(void);
void ret_m(void);
void reti(void);
void retn(void);
void rst_0(void);
void rst_8(void);
void rst_10(void);
void rst_18(void);
void rst_20(void);
void rst_28(void);
void rst_30(void);
void rst_38(void);

/* INOUT.c    */

void in_a_pn(void);
void in_b_pc(void);
void in_c_pc(void);
void in_d_pc(void);
void in_e_pc(void);
void in_h_pc(void);
void in_l_pc(void);
void in_a_pc(void);
void in_f_pc(void);
void ini(void);
void inir(void);
void ind(void);
void indr(void);
void out_pn_a(void);
void out_pc_b(void);
void out_pc_c(void);
void out_pc_d(void);
void out_pc_e(void);
void out_pc_h(void);
void out_pc_l(void);
void out_pc_a(void);
void out_pc_f(void);
void in_pc(void);
void out_pc_0(void);
void outi(void);
void otir(void);
void outd(void);
void otdr(void);

/* KERNEL.c   */

void do_reset(void);
void execute(void);

/* INTR.c     */

void do_interrupt(void);
void do_nmi_int(void);

/* MEM.c      */

void writebyte(USHORT, UCHAR );
void writeword(USHORT, USHORT );
UCHAR readbyte(USHORT);
USHORT readword(USHORT);
USHORT Getnextword(void);
void Init_Z80Display(void);
void Close_Z80Display(void);

/* PORTS.c    */

void writeport(USHORT, UCHAR);
UCHAR readport(USHORT);
UCHAR get_sbrdr(void);

/* INIT.c     */

void Init_Z80Emu(void);
void Close_Z80Emu(void);
void do_fullreset();
void do_warmreset();

/* ERROR.c    */

void Panic(const char *);
void Warning(const char *);
void exit_emu(void);

/* Z80DEBUG.c */

void trace(void);
void Init_Z80debugger(void);
void Close_Z80debugger(void);

/* DISSBL.c   */

char * ldissbl(USHORT);

/* IO.c       */

short write_mem(const char *, USHORT, USHORT, UCHAR);
short read_mem(const char *, USHORT, USHORT);

/* SHUTDOWN.c */
void shutdown_Z80(void);

/* CURSOR.c   */
#if defined(__TURBOC__) && defined(__MSDOS__)
void disable_cursor(void);
void restore_cursor(void);
#endif

/* GETKEY.c   */
short getkey(void);

/* READ_HEX.c */
USHORT xtoi(char *);
USHORT read_xnum(void);

/* HEXDUMP.c  */
void hexdump(USHORT);

/* BOX.c      */
void box(short, short, short, short);

/* CLRLINE.c  */
void clrStatusLine(void);

/* SNALOAD.c */
//int open_sna(LPSTR);
//void open_menu(HWND);
void reload_snap(void);
void level_loader_trap(void);
void save_snap(void);
void patch_rom(int do_it);

/* SNASAVE.c */
//void save_sna(LPSTR);
//void open_menu_save(HWND);
//void putbyte(UCHAR, HFILE);
//void put2(USHORT, HFILE);

/* WSPECEM.c */
void set_palette(UCHAR palettePos, UCHAR colour);
void pixel_host(USHORT, USHORT, UCHAR);
void resize_host(unsigned x, unsigned y);

/* VIDEO.C */
void do_flash(void);

/* INITEM.C */
//void init_emul(HINSTANCE );

/* NOTHING.C */
int do_nothing(USHORT *);

/* floating_bus.c */
int floating_bus(unsigned int t_states);

/* contention.c */
void contention_delay(uint32_t tstate, uint16_t address);

/* EOF: IGlobal.h */

