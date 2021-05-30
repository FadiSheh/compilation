
#include <stdio.h>
#include "common.h"
#include "defs.h"
#include "passe_2.h"
#include "utils/miniccutils.h"

int l1;
int l2;
int flagp2 = 0;
node_type dernier_type2 = TYPE_NONE;
int32_t reg_to_free;
int32_t a;
int32_t b;
int r1;
int r2;
int r3;
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

				gen_code_passe_2(root->opr[0]);

				flagp2=0;
				create_inst_text_sec();
				gen_code_passe_2(root->opr[1]);
				//syscall
				create_inst_comment("exit");
				create_inst_ori(2,0,0x1010);
				create_inst_syscall();
				break;
			case NODE_BLOCK: 
				gen_code_passe_2(root->opr[0]);
				gen_code_passe_2(root->opr[1]);

				break;

		case NODE_IDENT: 

			break;

		case NODE_AFFECT:
			if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){
					affectation(root);
			} 
			else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature != NODE_IDENT){
				//ajouter option quand ya un plus donc continuer de parcourir l'arbre
				gen_code_passe_2(root->opr[0]);
				gen_code_passe_2(root->opr[1]);
			}
			else{
				declaration_affectation(root);
			}
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
			create_inst_comment("desallocation variables locales");
			create_inst_stack_deallocation(root->offset + get_temporary_max_offset());
			break;

		case NODE_LIST:
			gen_code_passe_2(root->opr[0]);
			gen_code_passe_2(root->opr[1]);
			break;
		case NODE_DECLS:
			gen_code_passe_2(root->opr[0]);
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
				//si un fils est null : pas de valeur
				//si un fils declaration - affectation
				//si nouvelle déclaration
				dernier_type2 = root->opr[0]->type;
				if(root->opr[0]!=NULL && root->opr[1] != NULL){
					if(root->opr[1]->nature == NODE_IDENT){
						//declaration et affectation
						create_inst_comment("declaration - affectation");
						declaration_affectation(root);
					}		
					else{
						//nouvelle déclaration
						create_inst_comment("declaration");
						if(reg_available()){
							allocate_reg();
							create_inst_ori(get_current_reg(),0x0, root->opr[1]->value);
							create_inst_sw(get_current_reg(), root->opr[0]->offset, 29);
							release_reg();
						}
						else{

						}
					}			
				}
				if(root->opr[0]!=NULL && root->opr[1] != NULL){
					gen_code_passe_2(root->opr[1]);
				}
				else {
					create_inst_comment("null");
					//si un fils est null il y a rien à faire
				}
			}

			break;
		case NODE_TYPE:
			dernier_type2 = root->type;
			break;
		case NODE_STRINGVAL:

			break;
		case NODE_PLUS:
			if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_INTVAL){
				create_inst_comment("i = i+1");
				printf("i = i + 1\n");
				recup_offset(root->opr[0], 1);
				r3 = root->opr[1]->value;
				printf("imm = %d\n", r3);
				allocate2_imm(create_inst_ori);
			}
			else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){
				create_inst_comment("sum = sum + i");
				recup_offset(root->opr[0], 1);
				recup_offset(root->opr[1], 2);
				allocate2_sw(create_inst_addu);
			}
			else{
				//deux intval

			}
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
			create_inst_comment("debut label");				
			l1 = get_new_label();
			printf("%d\n", l1);
			create_inst_label(l1);
			l2 = get_new_label();
			printf("%d\n", l2);
			//printf("Nature [1] dans for: %s\n", node_nature2string(root->opr[1]->nature));
			gen_code_passe_2(root->opr[1]);
			gen_code_passe_2(root->opr[3]);
			//printf("Nature [1] dans for: %s\n", node_nature2string(root->opr[2]->nature));
			gen_code_passe_2(root->opr[2]);
			//printf("Nature [1] dans for: %s\n", node_nature2string(root->opr[3]->nature));
			create_inst_comment("retour au test de boucle");
			create_inst_j(l1);
			create_inst_label(l2);
			break;
		case NODE_DOWHILE:

			break;
		case NODE_LT:
			recup_offset(root->opr[0], 1);
			recup_offset(root->opr[1], 2);
			//printf("a = %d\n", a);
			//printf("b = %d\n", b);
			allocate2(create_inst_slt);
			create_inst_beq(r1, 0, l2);
			//nv label
			
			
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

void affectation(node_t root){
	recup_offset(root->opr[1], 1);
	recup_offset(root->opr[0], 2);
	//printf("a dans aff = %d\n", a);
	//printf("b dans aff = %d\n", b);
	if(reg_available()){
		allocate_reg();
		create_inst_lw(get_current_reg(), a, get_current_reg());
		create_inst_sw(get_current_reg(), b, 29);
		release_reg();
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		create_inst_lw(reg_to_free, a, reg_to_free);
		create_inst_sw(reg_to_free, b, 29);
		pop_temporary(reg_to_free);
	}
}
void declaration_affectation(node_t root){
	//printf("off1 : %d\n", root->opr[1]->offset);
	recup_offset(root->opr[1], 1);
	recup_offset(root->opr[0], 2);
	if(reg_available()){
		allocate_reg();
		create_inst_lui(get_current_reg(),0x1001);
		create_inst_lw(get_current_reg(), a, get_current_reg());
		create_inst_sw(get_current_reg(), b, 29);
		release_reg();
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		create_inst_lui(reg_to_free, 0x1001);
		create_inst_lw(reg_to_free, a, reg_to_free);
		create_inst_sw(reg_to_free, b, 29);
		pop_temporary(reg_to_free);
	}
}

void recup_offset(node_t root, int32_t i){
	printf("Nature 1 dans off: %s\n", node_nature2string(root->nature));
	node_t tmp1 = root->decl_node;
	printf("Nature 2 dans off: %s\n", node_nature2string(tmp1->nature));
	if(tmp1){
		if(i==1)
			a = tmp1->offset;
		else
			b = tmp1->offset;
	}
	else{
		i = root->offset;
	}
}

void allocate2(void (*mon_ope)()){
	if(reg_available()){
		allocate_reg();
		r1 = get_current_reg();
	
		if(reg_available()){
			allocate_reg();
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			create_inst_lw(r2, b, 29);
			mon_ope(r1, r1, r2);
			release_reg();
		}
		else{
			reg_to_free = get_restore_reg();
			push_temporary(reg_to_free);
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			create_inst_lw(r2, b, 29);
			mon_ope(r1, r1, r2);
			pop_temporary(reg_to_free);
		}
		release_reg();
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		r1 = get_current_reg();
		if(reg_available()){
			allocate_reg();
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			create_inst_lw(r2, b, 29);
			mon_ope(r1, r1, r2);
			release_reg();
		}
		else{
			reg_to_free = get_restore_reg();
			push_temporary(reg_to_free);
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			create_inst_lw(r2, b, 29);
			mon_ope(r1, r1, r2);
			pop_temporary(reg_to_free);		
		}
		pop_temporary(reg_to_free);	
	}
}

void allocate2_sw(void (*mon_ope)()){
	if(reg_available()){
		allocate_reg();
		r1 = get_current_reg();
	
		if(reg_available()){
			allocate_reg();
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			create_inst_lw(r2, b, 29);
			mon_ope(r1, r1, r2);
			create_inst_sw(r1, a, 29);
			release_reg();
		}
		else{
			reg_to_free = get_restore_reg();
			push_temporary(reg_to_free);
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			create_inst_lw(r2, b, 29);
			mon_ope(r1, r1, r2);
			create_inst_sw(r1, a, 29);
			pop_temporary(reg_to_free);
		}
		release_reg();
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		r1 = get_current_reg();
		if(reg_available()){
			allocate_reg();
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			create_inst_lw(r2, b, 29);
			mon_ope(r1, r1, r2);
			create_inst_sw(r1, a, 29);
			release_reg();
		}
		else{
			reg_to_free = get_restore_reg();
			push_temporary(reg_to_free);
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			create_inst_lw(r2, b, 29);
			mon_ope(r1, r1, r2);
			create_inst_sw(r1, a, 29);
			pop_temporary(reg_to_free);		
		}
		pop_temporary(reg_to_free);	
	}
}

void allocate2_imm(void (*mon_ope)()){
	if(reg_available()){
		allocate_reg();
		r1 = get_current_reg();
	
		if(reg_available()){
			allocate_reg();
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			mon_ope(r1, r1, r3);
			create_inst_addu(r1, r1, r2);
			create_inst_sw(r1, a, 29);
			release_reg();
		}
		else{
			reg_to_free = get_restore_reg();
			push_temporary(reg_to_free);
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			mon_ope(r1, r1, r2);
			create_inst_addu(r1, r1, r2);
			create_inst_sw(r1, a, 29);
			pop_temporary(reg_to_free);
		}
		release_reg();
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		r1 = get_current_reg();
		if(reg_available()){
			allocate_reg();
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			mon_ope(r1, r1, r2);
			create_inst_addu(r1, r1, r2);
			create_inst_sw(r1, a, 29);
			release_reg();
		}
		else{
			reg_to_free = get_restore_reg();
			push_temporary(reg_to_free);
			r2 = get_current_reg();
			create_inst_lw(r1, a, 29);
			mon_ope(r1, r1, r2);
			create_inst_addu(r1, r1, r2);
			create_inst_sw(r1, a, 29);
			pop_temporary(reg_to_free);		
		}
		pop_temporary(reg_to_free);	
	}
}