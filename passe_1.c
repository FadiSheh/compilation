
#include <stdio.h>

#include "defs.h"
#include "passe_1.h"
#include "common.h"
#include "utils/miniccutils.h"
extern int trace_level;
node_type dernier_type = TYPE_NONE;


void analyse_passe_1(node_t root) {
	//printf("helloworld\n");
	if(!root){
		printf("Arbre est vide\n");

	} else {

	printf("Nature : %s\n", node_nature2string(root->nature));
	switch(root->nature){

		case NODE_PROGRAM:
			push_global_context(); //Initialise le contexte pour les varibales globales



			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			

			break;

		case NODE_BLOCK: 

			push_context();

			if(root->opr[0] != NULL){
				analyse_passe_1(root->opr[0]);
			}
			if(root->opr[1] != NULL){
				analyse_passe_1(root->opr[1]);
			}


			pop_context();
			break;

		case NODE_IDENT: //

			root->type = dernier_type;
			node_t tmp;
			tmp = get_decl_node(root->ident);
			
			if (tmp){
				root->decl_node = tmp;
			}	
			else {printf("ERREUr");}	
			break;

		case NODE_AFFECT: //
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_FUNC:

			reset_env_current_offset();
			analyse(root, root->nops);
			root->offset = get_env_current_offset();
			break;
			
		case NODE_LIST:
			analyse(root, root->nops);
			break;
		case NODE_DECLS:
			analyse(root, root->nops);
			break;

		case NODE_DECL:
			root->opr[0]->offset=env_add_element(root->opr[0]->ident,root->opr[0]);
			analyse(root, root->nops);
			break;

		case NODE_TYPE:
			//pas d'appel à A1
			dernier_type = root->type;
			//printf("type : %d\n", dernier_type);
			break;
		case NODE_INTVAL:
			root->type = TYPE_INT;
			break;
		case NODE_BOOLVAL:
			root->type = TYPE_BOOL;
			break;
		case NODE_STRINGVAL:
			root->type = TYPE_STRING;
			root-> offset = add_string(root->str);
			break;

		//type de tous les noeuds des opérateurs
		case NODE_PLUS:

			analyse(root, root->nops);
			break;
		case NODE_MINUS:
			analyse(root, root->nops);
			break;
		case NODE_MUL:
			analyse(root, root->nops);
			break;
		case NODE_DIV:
			analyse(root, root->nops);
			break;
		case NODE_MOD:
			analyse(root, root->nops);
			break;
		case NODE_IF:
			analyse(root, root->nops);
			break;
		case NODE_WHILE:
			analyse(root, root->nops);
			break;
		case NODE_FOR:
			analyse(root, root->nops);
			break;
		case NODE_DOWHILE:
			analyse(root, root->nops);
			break;
		case NODE_LT:
			analyse(root, root->nops);
			break;
		case NODE_GT:
			analyse(root, root->nops);
			break;
		case NODE_LE:
			analyse(root, root->nops);
			break;
		case NODE_GE:
			analyse(root, root->nops);
			break;
		case NODE_EQ:
			analyse(root, root->nops);
			break;
		case NODE_NE:
			analyse(root, root->nops);
			break;
		case NODE_AND:
			analyse(root, root->nops);
			break;
		case NODE_OR:
			analyse(root, root->nops);
			break;
		case NODE_BAND:
			analyse(root, root->nops);
			break;
		case NODE_BOR:
			analyse(root, root->nops);;
			break;
		case NODE_BXOR:
			analyse(root, root->nops);
			break;
		case NODE_NOT:
			analyse(root, root->nops);
			break;
		case NODE_BNOT:
			analyse(root, root->nops);
			break;
		case NODE_SLL:
			analyse(root, root->nops);
			break;
		case NODE_SRA:
			analyse(root, root->nops);
			break;
		case NODE_SRL:
			analyse(root, root->nops);
			break;
		case NODE_UMINUS:
			analyse(root, root->nops);
			break;
		case NODE_PRINT:
			analyse(root, root->nops);
			break;
		default:
		analyse(root, root->nops);
			break;
	}
	}
	//printf("finito %d\n", cpt++);
}


void analyse(node_t node, int nops){
	for (int i = 0; i<nops; i++){
		analyse_passe_1(node->opr[i]);
	}
}