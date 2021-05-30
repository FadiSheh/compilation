
#include <stdio.h>

#include "defs.h"
#include "passe_1.h"
#include "common.h"
#include <string.h>
#include "utils/miniccutils.h"
extern int trace_level;
node_type dernier_type = TYPE_NONE;
int flag =0; //0 global 1 sinon
node_t tmp;
int ligne;

void analyse_passe_1(node_t root) {
	ligne = root->lineno;

	if(trace_level==2 || trace_level>3){
	printf("NATURE: %s\n",node_nature2string(root->nature));}
	
	if(!root){
		//printf("Arbre est vide\n");

	} else {

	//printf("Nature : %s\n", node_nature2string(root->nature));
	switch(root->nature){

		case NODE_PROGRAM:

			push_global_context(); //Initialise le contexte pour les varibales globales

			if(root->opr[0] != NULL){
				analyse_passe_1(root->opr[0]);
				
			}
			flag=1;
			if(root->opr[1] != NULL){
				analyse_passe_1(root->opr[1]);
				//flag=0;
			}


			

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

	

			if(root && strcmp(root->ident,"main") != 0 ){


				if (flag==0){

					root->global_decl =true;

				} else {

					root->global_decl =false;
				}

				if(root->offset<0){
				//Variable existe deja?
					fprintf(stderr, "Error line %d: variable existe deja\n", root->lineno);
					exit(1);

				}

				root->type = dernier_type;
			
				tmp = get_decl_node(root->ident);
			
				if (tmp){
					root->decl_node = tmp;
					root->type=tmp->type;
					}

					else {

				fprintf(stderr, "Error line %d: variable nn definie\n", root->lineno);
					exit(1);
			
			} 
			}

			break;

		case NODE_AFFECT: //
			analyse(root, root->nops);
			root->type = root->opr[0]->type;
			undeclVar(root);
			break;

		case NODE_FUNC:
			reset_temporary_max_offset();
			reset_env_current_offset();
			errorMain(root);
			analyse(root, root->nops);
			root->offset = get_env_current_offset();
			break;

		case NODE_LIST:
			analyse(root, root->nops);
			break;

		case NODE_DECLS:
			analyse(root, root->nops);
			if(root->opr[0]->type==TYPE_VOID){
				fprintf(stderr, "Error line %d: Variable de type void\n", root->lineno);
				exit(1);
			}
			break;

		case NODE_DECL:
			root->opr[0]->offset=env_add_element(root->opr[0]->ident,root->opr[0]);
			analyse(root, root->nops);
			errorTYPE(root);

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

		case NODE_IF:
			analyse(root, root->nops);
			errorWHILE(root);
			break;

		case NODE_WHILE:
			analyse(root, root->nops);
			errorWHILE(root);
			break;

		case NODE_FOR:
			analyse(root, root->nops);
			errorFOR(root);
			break;

		case NODE_DOWHILE:
			analyse(root, root->nops);
			errorDOWHILE(root);
			break;
		
		case NODE_NOT: case NODE_UMINUS: case NODE_BNOT:
			
			if(root->nature!=NODE_UMINUS){
				errorNONCONST(flag);
			}
			analyse(root, root->nops);
			root->type=type_op_unaire(root->nature,root->opr[0]->type);
			break;

		case NODE_SRL: case NODE_SRA: case NODE_SLL: case NODE_BXOR: case NODE_BOR: case NODE_BAND: case NODE_LT: case NODE_DIV: case NODE_MUL:
		case NODE_OR: case NODE_AND: case NODE_NE: case NODE_EQ: case NODE_GE: case NODE_LE: case NODE_GT: case NODE_MOD: case NODE_MINUS: case NODE_PLUS:
			errorNONCONST(flag);
			analyse(root, root->nops);
            root->type=type_op_binaire(root->nature,root->opr[0]->type,root->opr[1]->type);

            

			break;

		case NODE_PRINT:
			analyse(root, root->nops);
			break;

		default:
			analyse(root, root->nops);
			break;
		}
	}


	
}


void analyse(node_t node, int nops){
	for (int i = 0; i<nops; i++){
		if(node->opr[i]){
		analyse_passe_1(node->opr[i]);}
	}
}

void errorTYPE(node_t node){

	if(node->opr[1]!=NULL){
		if(node->opr[0]->type!=node->opr[1]->type){

			fprintf(stderr, "Error line %d: Operation not permitted, first operand is type %s and second is type %s. \n", node->lineno, node_type2string(node->opr[0]->type),node_type2string(node->opr[1]->type));
			exit(1);
		}
	
	}
}	




void errorFOR(node_t node){

	/*if(node->opr[0]->type!=TYPE_INT){

		fprintf(stderr, "Error line %d: Operation not permitted, first argument is type %s. Must be TYPE_INT \n", node->lineno, node_type2string(node->opr[0]->type));
		exit(1);

	} else 
*/
	if(node->opr[1]->type!=TYPE_BOOL){

		fprintf(stderr, "Error line %d: Operation not permitted, second argument is type %s. Must be TYPE_BOOL \n", node->lineno, node_type2string(node->opr[1]->type));
		exit(1);
	} 

	/*else if(node->opr[2]->type!=TYPE_INT){

		fprintf(stderr, "Error line %d: Operation not permitted, third argument is type %s. Must be TYPE_INT \n", node->lineno, node_type2string(node->opr[2]->type));
		exit(1);


	}*/
}	

void errorWHILE(node_t node){

	if(node->opr[0]->type!=TYPE_BOOL){

		fprintf(stderr, "Error line %d: Operation not permitted, argument is type %s. Must be TYPE_BOOL \n", node->lineno, node_type2string(node->opr[0]->type));
		exit(1);

	}

}

void errorDOWHILE(node_t node){

	if(node->opr[1]->type!=TYPE_BOOL){

		fprintf(stderr, "Error line %d: Operation not permitted, argument is type %s. Must be TYPE_BOOL \n", node->lineno, node_type2string(node->opr[1]->type));
		exit(1);

	}

}

void undeclVar(node_t node){

			if ( node->type ==TYPE_VOID ) { 
				fprintf(stderr, "Error line %d: Oubli type de variable pour %s\n", node->lineno,node->opr[0]->ident);
				exit(1);}

			else { if(node->opr[0]->type==node->opr[1]->type){

				   		tmp = get_decl_node(node->opr[0]->ident);

						if ((!tmp && (node->opr[0]->global_decl==false)) ){
							//variable nexiste pas dans le context
							//printf("Nexiste pas dans le contexte courant ni en globale");
							fprintf(stderr, "Error line %d: Oubli type de variable pour %s\n", node->lineno,node->opr[0]->ident);
							exit(1);

						}
					} else {

					fprintf(stderr, "Error line %d: type affect\n", node->lineno);
					exit(1);}	

			

			}
}

void errorMain(node_t root){
	if (root->opr[0]->type!=TYPE_VOID){

		fprintf(stderr, "Error line %d: Main must be TYPE_VOID, here type %s.\n", root->opr[0]->lineno,node_type2string(root->opr[0]->type));
		exit(1);

	}
}


node_type type_op_unaire(node_nature nature,node_type type){

	node_type retour;

	switch(type){

		case TYPE_INT:

			if(nature==NODE_UMINUS || nature==NODE_BNOT) {retour = TYPE_INT;}

			break;

		case TYPE_BOOL:

			if(nature==NODE_NOT){retour = TYPE_BOOL;

			} else {

				fprintf(stderr, "Error line %d: Erreur de type %s.\n", ligne,node_type2string(type));
				exit(1);
			}
		
			break;	

		default:
			printf("PROBLEME OP UNAIRE\n");
			break;

	}	
	return retour;

}


node_type type_op_binaire(node_nature nature,node_type type1,node_type type2){

	node_type retour;

	if(type1!=type2){

		fprintf(stderr, "Error line %d: Erreur de type.\n", ligne);
		exit(1);

	}
	if(type1 == TYPE_INT){

		switch(nature){
			case NODE_EQ: case NODE_NE: case NODE_LT: case NODE_GT: case NODE_LE: case NODE_GE:
				retour = TYPE_BOOL;
				break;

			default:
				retour = TYPE_INT;

				//printf("PROBLEME OP BINAIRE");
			break;
		}

	} else { retour = type1;}


	//printf("DE TYPE %s et %s\n", node_type2string(retour),node_type2string(type2));

	return retour;


}

void errorNONCONST(int val){

	if(val==0){
		
		fprintf(stderr, "Error line %d: Operation not permitted. Pas constante. \n", ligne);
		exit(1);

	}
}