#ifndef _PASSE_2_H_
#define _PASSE_2_H_

#include "defs.h"

void gen_code_passe_2(node_t root);
void declaration_affectation(node_t root);
void affectation(node_t root);
void recup_offset(node_t root, int32_t i);
void allocate2(void (*mon_ope)());
void allocate2_sw(void (*mon_ope)());
void allocate2_imm(void (*mon_ope)());
void ajoutStrings(int nb);
void ajoutInstancePrint(int emplacement);
void syscallExit();
int32_t return_reg1(int32_t r);
int32_t return_reg2(int32_t r);
void liberer_reg(bool drap);
void ope_binaire(node_t root, int reg1 , int reg2);
void gen_code(node_t node, int nops);
node_t updateTMP(node_t tmp, node_t root, int i,int r);

#endif