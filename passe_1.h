
#ifndef _PASSE_1_
#define _PASSE_1_

#include "defs.h"
#include <stdlib.h>

void analyse_passe_1(node_t root);
void analyse(node_t node, int nops);
void errorTYPE(node_t node);
void errorFOR(node_t node);
void errorWHILE(node_t node);
void errorDOWHILE(node_t node);
void undeclVar(node_t node);
void errorMain(node_t root);
node_type type_op_unaire(node_nature nature,node_type type);
node_type type_op_binaire(node_nature nature,node_type type1,node_type type2);
void errorNONCONST(int val);
#endif

