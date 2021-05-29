
#ifndef _PASSE_1_
#define _PASSE_1_

#include "defs.h"
#include <stdlib.h>

void analyse_passe_1(node_t root);
void analyse(node_t node, int nops);
void errorBOOL(node_t node);
void errorINT(node_t node);
void errorFOR(node_t node);
void errorCOND(node_t node);
void undeclVar(node_t node);
void errorMain(node_t root);

#endif

