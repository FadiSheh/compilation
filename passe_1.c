
#include <stdio.h>

#include "defs.h"
#include "passe_1.h"
#include "utils/miniccutils.h"
#include "common.h"
extern int trace_level;
node_type dernier_type = TYPE_NONE;

void analyse_passe_1(node_t root) {
	
	if(!root){
		printf("Arbre est vide\n");
		exit(1);
	}
    printf("Nature: %s\n", node_nature2string(root->nature));
	switch(root->nature){
		case NODE_PROGRAM:	//
			//initialisation contexte global vide
			//voir fonction
			push_global_context();
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			pop_context();
			break;
		case NODE_BLOCK: //
			push_context();
			if (root->opr[0]!=NULL){
			analyse_passe_1(root->opr[0]);}
			if (root->opr[1]!=NULL){
			analyse_passe_1(root->opr[1]);}
			pop_context();
			break;
		case NODE_IDENT: //
			//pas d'appel à A1
			//Deux choses en deux temps : type
			root->type = dernier_type;
			root->offset = env_add_element(root->ident,root);
			if(root->offset<=0){
				node_t tmp = get_decl_node(root->ident);
				root->decl_node = tmp;
				printf("Variable existe deja\n");


			}
			   /* TYPE_NONE,
			}
    TYPE_INT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_VOID,*/
			//offset : int32_t
			//locale : offset de la pile
			//globale : offset dans la section .data
			//root-> offset = ;
			//global_decl : bool
			//root-> global_decl = ;
			//decl_node : pointeur _node_s *
			//adresse du noeud contenant la déclaration de la variable référencée
			//root-> decl_node = ;
			break;
		/*case NODE_AFFECT: //
			//type
			break;*/
		case NODE_FUNC:
			printf("NODE FUNC\n");
			reset_env_current_offset();
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			analyse_passe_1(root->opr[2]);
			//apres analyse de la fonction
			//offset
			root->offset = env_add_element(root->ident,root);
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
			if (root->opr[0]!=NULL){
			analyse_passe_1(root->opr[0]);}
			if (root->opr[1]!=NULL){
			analyse_passe_1(root->opr[1]);}
			break;
		case NODE_TYPE:
			//pas d'appel à A1
			dernier_type = root->type;
			printf("type node type: %d\n", dernier_type);
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

		case NODE_PRINT:
			analyse_passe_1(root->opr[0]);
			break;	

		//type de tous les noeuds des opérateurs
		/*case NODE_PLUS:
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
			//push_context();
			break;
		case NODE_WHILE:
			break;
		case NODE_FOR:
			break;
		case NODE_DOWHILE:
			break;*/
		default:
			break;
	}
	printf("finito\n");
}


  /*  NODE_LT,
    NODE_GT,
    NODE_LE,
    NODE_GE,
    NODE_EQ,
    NODE_NE,
    NODE_AND,
    NODE_OR,
    NODE_BAND,
    NODE_BOR,
    NODE_BXOR,
    NODE_NOT,
    NODE_BNOT,
    NODE_SLL,
    NODE_SRA,
    NODE_SRL,
    NODE_UMINUS,
    NODE_PRINT,*/
