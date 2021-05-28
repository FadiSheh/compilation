
#include <stdio.h>
#include "common.h"
#include "defs.h"
#include "passe_2.h"

int l;
int flagp2 = 0;
node_type dernier_type2 = TYPE_NONE;
//int flagp3 = 0;

void gen_code_passe_2(node_t root) {
	//printf("helloworld\n");
	if(!root){
		printf("Arbre est vide\n");
	}
	//déclaration des chaines litterales en section .data
	//get_new_label() (int)
	//label pt prog : entier
	//label directive (.word)


	else{
		printf("Nature : %s\n", node_nature2string(root->nature));
		switch(root->nature){
			case NODE_PROGRAM:
				printf("helloworld\n");
				create_program();
				create_inst_data_sec();
				flagp2 = 1;
				/*.data
				start :
				end:
				.asciiz

				.text
				main:*/
				//flag=1;
				gen_code_passe_2(root->opr[0]);

				flagp2=0;
				create_inst_text_sec();
				gen_code_passe_2(root->opr[1]);
				//syscall
				create_inst_ori(2,0,0x1010);
				create_inst_syscall();
				break;
			case NODE_BLOCK: 
				printf("yo\n");
				printf("dernier_type2 : %s\n", node_type2string(dernier_type2));
				if(dernier_type2 != TYPE_VOID){
					
					l = get_new_label();
					printf("%d\n", l);
					create_inst_label(l);
				}
				gen_code_passe_2(root->opr[0]);
				gen_code_passe_2(root->opr[1]);

				break;

		case NODE_IDENT: //

			/*if (flag==1){


			} else {
			}*/
			

			break;

		case NODE_AFFECT: //

			break;
		case NODE_FUNC:
			//p.II-9/10
			create_inst_label_str(root->opr[1]->ident);
			set_temporary_start_offset(root->offset);
		//II-10
			create_inst_stack_allocation();
			//flagp3 = 1;
			gen_code_passe_2(root->opr[0]);
			gen_code_passe_2(root->opr[1]);
			gen_code_passe_2(root->opr[2]);
			//place à allouer en pile
			//somme entre offset du noeud NODE_FUNC et valeur 8
			//+place variables locales ? II-9
			create_inst_stack_deallocation(get_temporary_max_offset());
			break;

		case NODE_LIST:
			gen_code_passe_2(root->opr[0]);
			gen_code_passe_2(root->opr[1]);
			break;
		case NODE_DECLS:
			
			gen_code_passe_2(root->opr[1]);
			break;

		case NODE_DECL:
			if(flagp2 == 1){
				//variable globale
				if(root->opr[0] != NULL && root->opr[1] != NULL){
					create_inst_word(root->opr[0]->ident, root->opr[1]->value);
				}
			}
			else{
				dernier_type2 = root->opr[0]->type;
				printf("dernier_type2 v2: %s\n", node_type2string(dernier_type2));
				printf("variable locale\n");
				//lui - lw - sw
				create_inst_lui(8, 0x1001);
				create_inst_lw(8, 0, 8);
				create_inst_sw(8, 4, 29);
			}

			break;
		case NODE_TYPE:
			dernier_type2 = root->type;
			break;
		case NODE_STRINGVAL:

			break;
		case NODE_PLUS:

			break;
		case NODE_MINUS:

			break;
		case NODE_MUL:

			break;
		case NODE_DIV:

			break;
		case NODE_MOD:

			break;
		case NODE_IF:
			gen_code_passe_2(root->opr[0]);
			gen_code_passe_2(root->opr[1]);
			break;
		case NODE_WHILE:

			break;
		case NODE_FOR:
			gen_code_passe_2(root->opr[0]);
			gen_code_passe_2(root->opr[1]);
			gen_code_passe_2(root->opr[2]);
			gen_code_passe_2(root->opr[3]);
			break;
		case NODE_DOWHILE:

			break;
		case NODE_LT:

			break;
		case NODE_GT:

			break;
		case NODE_LE:

			break;
		case NODE_GE:

			break;
		case NODE_EQ:

			
			break;
		case NODE_NE:

			break;
		case NODE_AND:

			break;
		case NODE_OR:

			break;
		case NODE_BAND:

			break;
		case NODE_BOR:

			break;
		case NODE_BXOR:

			break;
		case NODE_NOT:

			break;
		case NODE_BNOT:

			break;
		case NODE_SLL:

			break;
		case NODE_SRA:


			break;
		case NODE_SRL:

			break;
		case NODE_UMINUS:

			break;
		case NODE_PRINT:
			break;
		default:
			break;
			

		}
	}
}
