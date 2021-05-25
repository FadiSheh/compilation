
#include <stdio.h>

#include "defs.h"
#include "passe_1.h"
#include "common.h"
#include "utils/miniccutils.h"
extern int trace_level;
node_type dernier_type = TYPE_NONE;
node_t noeud;
int flag = 0;
int cpt = 0;

void analyse_passe_1(node_t root) {
	//printf("helloworld\n");
	if(!root){
		printf("Arbre est vide\n");

	}
	else{
	printf("Nature : %s\n", node_nature2string(root->nature));
	switch(root->nature){

		case NODE_PROGRAM:
			push_global_context(); //Initialise le contexte pour les varibales globales
			flag = 1;
			analyse_passe_1(root->opr[0]);
	
			flag = 0;
			analyse_passe_1(root->opr[1]);
			
			//pop_context();
			break;

		case NODE_BLOCK: //

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
			//pas d'appel à A1
			//Deux choses en deux temps : type
			root->type = dernier_type;
			//root-> offset = env_add_element(root->ident, root);

			int test = env_add_element(root->ident, root);
		
			if( test < 0){

					//root-> offset = 0;
					root->decl_node = mggode(root->ident);
					printf("variable existe deja\n");

			} else  { 
				root-> offset = test;
			}

			if(flag){
				//variable gloable
				root-> global_decl = true;
			}
			break;

		case NODE_AFFECT: //
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_FUNC:
			reset_env_current_offset();
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			root->offset = get_env_current_offset();
			analyse_passe_1(root->opr[2]);

			
			break;
		case NODE_LIST:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_DECLS:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_DECL:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_TYPE:
			//pas d'appel à A1
			dernier_type = root->type;
			//printf("type : %d\n", dernier_type);
			break;
		case NODE_INTVAL:
			//pas d'appel à A1
			break;
		case NODE_BOOLVAL:
			//pas d'appel à A1
			break;
		case NODE_STRINGVAL:
			//pas d'appel à A1
			//offset
			root-> offset = add_string(root->str);
			break;

		//type de tous les noeuds des opérateurs
		case NODE_PLUS:
			//deux fils
			//contexte?
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_MINUS:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_MUL:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_DIV:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_MOD:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_IF:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			analyse_passe_1(root->opr[2]);
			break;
		case NODE_WHILE:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_FOR:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			analyse_passe_1(root->opr[2]);
			analyse_passe_1(root->opr[3]);
			break;
		case NODE_DOWHILE:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_LT:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_GT:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_LE:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_GE:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_EQ:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_NE:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_AND:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_OR:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_BAND:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_BOR:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_BXOR:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_NOT:
			analyse_passe_1(root->opr[0]);
			break;
		case NODE_BNOT:
			analyse_passe_1(root->opr[0]);
			break;
		case NODE_SLL:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_SRA:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_SRL:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;
		case NODE_UMINUS:
			analyse_passe_1(root->opr[0]);
			break;
		case NODE_PRINT:
			analyse_passe_1(root->opr[0]);
			break;
		default:
			break;
	}
	}
	//printf("finito %d\n", cpt++);
}
