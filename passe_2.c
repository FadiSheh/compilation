#include <stdio.h>
#include "common.h"
#include "defs.h"
#include "passe_2.h"
#include "utils/miniccutils.h"
#include <string.h>
int l1;
int l2;
int flagp2 = 0;
int aff=0;
int flagprint = 1;
node_type dernier_type2 = TYPE_NONE;
node_t tmp1;
node_t tmp2;
int32_t reg_to_free;
int32_t a;
int32_t b;
int r1;
int r2;
int r3;
int gloc = 0;
bool flagR1 = false;
bool flagR2 = false;
int value;

int offsetFunc = 0;
//int flagp3 = 0;

void gen_code_passe_2(node_t root) {

	if(!root){
		printf("Arbre est vide\n");
	}

	else{
		printf("Nature : %s\n", node_nature2string(root->nature));

		switch(root->nature){

			case NODE_PROGRAM:

				create_program();
				create_inst_data_sec(); 
				flagp2 = 1; //global
				gen_code_passe_2(root->opr[0]);
				ajoutStrings(get_global_strings_number());
				flagp2=0; //pas global
				create_inst_text_sec();
				gen_code_passe_2(root->opr[1]);
				syscallExit();
				break;

			case NODE_BLOCK:
				if(root->opr[0]!=NULL){
					gen_code_passe_2(root->opr[0]);
				}
				if(root->opr[1]!=NULL){
					gen_code_passe_2(root->opr[1]);
				}
				break;

		case NODE_IDENT: 


			if(flagp2==0){

				if(root->decl_node){
					printf("ici6\n");
					tmp2 = root->decl_node;
					printf("global_decl1 : %d\n", tmp2->global_decl);
					printf("ident : %s\n", tmp2->ident);
					printf("offset : %d\n", tmp2->offset);

				}
				else{
					tmp2 = NULL;
				}
			
              
				if(flagprint==0 ){
				
					if(tmp2->global_decl==true){
						create_inst_comment("PRINT VARIABLE 1 ");
						create_inst_lui(4,0x1001);
						create_inst_lw(4,tmp2->offset,4);
						create_inst_ori(2,0,0x1);
						create_inst_syscall();  
					} else {

						create_inst_comment("PRINT VARIABLE  2");
						create_inst_lw(4,tmp2->offset,29);
						create_inst_ori(2,0,0x1);
						create_inst_syscall();  


					}

				} 

			}

			break;

		case NODE_AFFECT:
			printf("ici\n");
			tmp2 = root->opr[0];
			if(root->decl_node){
				printf("ici1\n");
				tmp2 = root->opr[0]->decl_node;
			}


			if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){
					printf("ici2\n");
					affectation(root);
			} 
			else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature != NODE_IDENT){
				printf("ici3\n");
				gen_code_passe_2(root->opr[0]);
				//ici on a recup tmp2

				gen_code_passe_2(root->opr[1]);
			}
			else{
				printf("ici4\n");
				declaration_affectation(root);
			}
			break;


		case NODE_FUNC:
			reset_temporary_max_offset();
			create_inst_label_str(root->opr[1]->ident);
			set_temporary_start_offset(root->offset);
			create_inst_stack_allocation();
			gen_code_passe_2(root->opr[0]);
			gen_code_passe_2(root->opr[1]);
			gen_code_passe_2(root->opr[2]);
			//place à allouer en pile
			create_inst_comment("desallocation variables locales");
			create_inst_stack_deallocation(root->offset + get_temporary_max_offset());
			break;

		case NODE_LIST:

				if(root->opr[0]!=NULL){
					printf("fils1\n");
					gen_code_passe_2(root->opr[0]);
				}
				if(root->opr[1]!=NULL){
					printf("fils2\n");
					gen_code_passe_2(root->opr[1]);
				}
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
					aff=aff+4;

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
							create_inst_ori(get_current_reg()-1,0x0, root->opr[1]->value);
							create_inst_sw(get_current_reg()-1, root->opr[0]->offset, 29);
							release_reg();
						}
						else{

						}
					}			
				}
				else {
					create_inst_comment("null");
					//si un fils est null il y a rien à faire
				}
			}
			printf("TEST\n");
			gen_code_passe_2(root->opr[0]);

			gen_code_passe_2(root->opr[1]);

			break;
		case NODE_TYPE:
			dernier_type2 = root->type;
			break;

		case NODE_INTVAL:
			printf("hello\n");
			value = root->value;
			printf("value : %ld\n", value);
		break;

		case NODE_STRINGVAL:

				printf("CREATION PRINT PR STRING\n");
				create_inst_comment("PRINT");
				create_inst_lui(4,0x1001);
				//printf("AFF %d\n", aff);
    			create_inst_ori(4,4,aff);
    			create_inst_ori(2,0,0x4);
    			create_inst_syscall();
    			//printf("TAILLE CHAINE %ld\n",strlen(root->str));
    			aff=aff+strlen(root->str)-1;
				
			break;

		case NODE_BOOLVAL:
			value = root->value;
			break;
		case NODE_PLUS:

			r1 = return_reg1(r1);
			r2 = return_reg2(r2);
			printf("\n NATURE TMP2 : %s", node_nature2string(tmp2->nature));
			printf(" IDENT TMP2 : %s\n", tmp2->ident);
			if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_INTVAL){


				if(root->opr[0]->decl_node->global_decl==true){
					printf("GLOBAL TRUE\n");

					a=tmp2->offset;
					create_inst_lui(r1,0x1001);
					create_inst_lw(r1,a,r1);
				} else {
					printf("GLOBAL FALSE\n");
					recup_offset(root->opr[0], 1);
					create_inst_lw(r1, a, 29);
				}

				create_inst_ori(r2, get_r0(), root->opr[1]->value);
				ope_binaire(root,r1,r2);
				
			}
			else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){
				recup_offset(root->opr[0], 1);
				recup_offset(root->opr[1], 2);

				create_inst_lw(r1, a, 29);
				create_inst_lw(r2, b, 29);
				ope_binaire(root,r1,r2);
				//create_inst_addu(r1, r1, r2);
				//create_inst_sw(r1, a, 9);
				//liberer_reg(flagR1);
				//liberer_reg(flagR2);
			}
			else if(root->opr[0]->nature == NODE_INTVAL && root->opr[1]->nature == NODE_INTVAL){

				create_inst_ori(r1, get_r0(), root->opr[0]->value);
				create_inst_ori(r2, get_r0(), root->opr[1]->value);

				ope_binaire(root,r1,r2);
				//create_inst_addu(r1, r1, r2);

			}
			else{
				gen_code_passe_2(root->opr[0]);
				gen_code_passe_2(root->opr[1]);
				//r1 = return_reg1(r1);
				//r2 = return_reg2(r2);
				create_inst_lw(r1, tmp2->offset, 29);
				create_inst_addiu(r1, r1, root->opr[1]->value);
				create_inst_sw(r1, tmp2->offset, 29);
				liberer_reg(flagR1);
				liberer_reg(flagR2);

			}
			break;
		case NODE_MINUS:
			
			r1 = return_reg1(r1);
			r2 = return_reg2(r2);
			printf("\n NATURE TMP2 : %s", node_nature2string(tmp2->nature));
			printf(" IDENT TMP2 : %s\n", tmp2->ident);
			if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_INTVAL){


				if(root->opr[0]->decl_node->global_decl==true){
					printf("GLOBAL TRUE\n");

					a=tmp2->offset;
					create_inst_lui(r1,0x1001);
					create_inst_lw(r1,a,r1);
				} else {
					printf("GLOBAL FALSE\n");
					recup_offset(root->opr[0], 1);
					create_inst_lw(r1, a, 29);
				}

				create_inst_ori(r2, get_r0(), root->opr[1]->value);
				ope_binaire(root,r1,r2);
				
			}
			else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){
				recup_offset(root->opr[0], 1);
				recup_offset(root->opr[1], 2);

				create_inst_lw(r1, a, 29);
				create_inst_lw(r2, b, 29);
				ope_binaire(root,r1,r2);
				//create_inst_addu(r1, r1, r2);
				//create_inst_sw(r1, a, 9);
				//liberer_reg(flagR1);
				//liberer_reg(flagR2);
			}
			else if(root->opr[0]->nature == NODE_INTVAL && root->opr[1]->nature == NODE_INTVAL){

				create_inst_ori(r1, get_r0(), root->opr[0]->value);
				create_inst_ori(r2, get_r0(), root->opr[1]->value);

				ope_binaire(root,r1,r2);
				//create_inst_addu(r1, r1, r2);

			}
			else{
				gen_code_passe_2(root->opr[0]);
				gen_code_passe_2(root->opr[1]);
				//r1 = return_reg1(r1);
				//r2 = return_reg2(r2);
				create_inst_lw(r1, tmp2->offset, 29);
				create_inst_addiu(r1, r1, root->opr[1]->value);
				create_inst_sw(r1, tmp2->offset, 29);
				liberer_reg(flagR1);
				liberer_reg(flagR2);

			}
			break;
		case NODE_MUL:
			if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_INTVAL){
				recup_offset(root->opr[0], 1);
				r3 = root->opr[1]->value;
				r1 = return_reg1(r1);
				r2 = return_reg2(r2);	
				create_inst_lw(r1, a, 29);
				create_inst_ori(r2, get_r0(), r3);
				create_inst_mult(r1, r2);
				create_inst_mfhi(r1);
				create_inst_sw(r1, a, 29);
				liberer_reg(flagR1);
				liberer_reg(flagR2);
			}
			else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){
				recup_offset(root->opr[0], 1);
				recup_offset(root->opr[1], 2);
				r1 = return_reg1(r1);
				r2 = return_reg2(r2);
				create_inst_lw(r1, a, 29);
				create_inst_lw(r2, b, 29);
				create_inst_mult(r1, r2);
				create_inst_mfhi(r1);
				create_inst_sw(r1, a, 29);
				liberer_reg(flagR1);
				liberer_reg(flagR2);
			}
			else{
				gen_code_passe_2(root->opr[0]);
				gen_code_passe_2(root->opr[1]);
			}
			break;
		case NODE_DIV:
			if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_INTVAL){

				recup_offset(root->opr[0], 1);
				r3 = root->opr[1]->value;
				r1 = return_reg1(r1);
				r2 = return_reg2(r2);				
				create_inst_lw(r1, a, 29);
				create_inst_ori(r2, get_r0(), r3);
				create_inst_div(r1, r2);
				create_inst_mflo(r1);
				create_inst_sw(r1, a, 29);
				liberer_reg(flagR1);
			}
			else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){
				recup_offset(root->opr[0], 1);
				recup_offset(root->opr[1], 2);
				r1 = return_reg1(r1);
				r2 = return_reg2(r2);
				create_inst_lw(r1, a, 29);
				create_inst_lw(r2, b, 29);
				create_inst_div(r1, r2);
				create_inst_mflo(r1);
				create_inst_sw(r1, a, 29);
				liberer_reg(flagR1);
				liberer_reg(flagR2);
			}
			else{
				gen_code_passe_2(root->opr[0]);
				gen_code_passe_2(root->opr[1]);
			}
			break;
		case NODE_MOD:
			if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_INTVAL){

				recup_offset(root->opr[0], 1);
				r3 = root->opr[1]->value;

				r1 = return_reg1(r1);	
				r2 = return_reg2(r2);			
				create_inst_lw(r1, a, 29);
				create_inst_ori(r2, get_r0(), r3);
				create_inst_div(r1, r2);
				create_inst_mfhi(r1);
				create_inst_sw(r1, a, 29);
				liberer_reg(flagR1);
				liberer_reg(flagR2);
			}
			else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){

				recup_offset(root->opr[0], 1);
				recup_offset(root->opr[1], 2);
				r1 = return_reg1(r1);
				r2 = return_reg2(r2);
				create_inst_lw(r1, a, 29);
				create_inst_lw(r2, b, 29);
				create_inst_div(r1, r2);
				create_inst_mfhi(r1);
				create_inst_sw(r1, a, 29);
				liberer_reg(flagR1);
				liberer_reg(flagR2);
			}
			else{
				gen_code_passe_2(root->opr[0]);
				gen_code_passe_2(root->opr[1]);
			}
			break;
		case NODE_IF:
			l1 = get_new_label();
			l2 = get_new_label();
			create_inst_comment("IF");
			printf("!! IF !!\n");
			if(root->opr[0]!=NULL){
				if(root->opr[0]->nature != NODE_IDENT){
					printf("HERE\n");					
					gen_code_passe_2(root->opr[0]);
				}
				else{
					printf("Nature opr0 : %s\n", node_nature2string(root->opr[0]->nature));
					l1 = get_new_label();
					gen_code_passe_2(root->opr[0]);
					printf("yo\n");
					r1 = return_reg1(r1);
					r2 = return_reg2(r2);
					if(root->opr[0]->decl_node->global_decl == true){
						printf("yo1\n");
						create_inst_lui(r1, 29);
						create_inst_lw(r1, tmp2->offset, r1);
					}
					else{
						//voir quand on n'a pas de globale
						printf("pas ici\n");
					}
					create_inst_beq(r1, get_r0(), l1);
				}
			}
			if(root->opr[1]!=NULL){
				
				
				printf("Nature opr1 : %s\n", node_nature2string(root->opr[1]->nature));
				gen_code_passe_2(root->opr[1]);
				printf("on revient la\n");
				printf("value = %d\n", value); //value du boolval
				//faire le reste ici
				//r1 = return_reg1(r1);
				//r2 = return_reg2(r2);
				/*create_inst_ori(r1, get_r0(), value);
				create_inst_lui(r2, 29);
				create_inst_sw(r1, tmp2->offset, 9);*/
				create_inst_j(l2);
			}
			if(root->opr[2]!=NULL){
				create_inst_label(l1);
				printf("Nature opr2 : %s\n", node_nature2string(root->opr[2]->nature));
				gen_code_passe_2(root->opr[2]);
				create_inst_label(l2);
			}
			break;
		case NODE_WHILE:
			l1 = get_new_label();
			printf("%d\n", l1);
			create_inst_label(l1);
			l2 = get_new_label();

			gen_code_passe_2(root->opr[0]);

			gen_code_passe_2(root->opr[1]);
			create_inst_j(l1);
			create_inst_label(l2);
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
			r1 = return_reg1(r1);
			r2 = return_reg2(r2);
			gen_code_passe_2(root->opr[0]);
			recup_offset(root->opr[0], 1);
			recup_offset(root->opr[1], 2);
			printf("est revenu là\n");
			printf("global_decl2 : %d\n", tmp2->global_decl);
			create_inst_comment("dans le LT");
			if(tmp2->global_decl){
				printf("ICIIIIIIIII\n");
				printf("offset : %d\n", tmp2->offset);
				create_inst_lw(r1, tmp2->offset, 29);
			}
			else{
				if(root->opr[1]->nature == NODE_INTVAL){
					create_inst_ori(r2, get_r0(), value);
				}
				else{
					printf("est passé par là\n");
					create_inst_comment("yoooo");
					create_inst_lw(r1, tmp2->offset, 29);
				}
			}
			gen_code_passe_2(root->opr[1]);
			printf("HERE\n");
			if(tmp2->global_decl == true){
				printf("PASHERE1\n");
				create_inst_lw(r2, tmp2->offset, 9);
			}
			else{
				if(root->opr[1]->nature == NODE_INTVAL){
					create_inst_ori(r2, get_r0(), value);
				}
				else{
					printf("PASHERE2\n");
					create_inst_lw(r2, tmp2->offset, 29);
				}
			}
			printf("hello" );
			//printf("a = %d\n", a);
			//printf("b = %d\n", b);
			//allocate2(create_inst_slt);
			create_inst_slt(r1, r1, r2);
			printf("hello1\n");
			create_inst_beq(r1, 0, l2);
			//nv label
			liberer_reg(flagR1);
			liberer_reg(flagR2);
			
			break;
		case NODE_GT:
			r1 = return_reg1(r1);
			r2 = return_reg2(r2);
			gen_code_passe_2(root->opr[0]);
			if(tmp2->global_decl==true){
				if(!tmp2){
					printf("NULL\n");
				}
				printf("\n NATURE TMP2 : %s", node_nature2string(tmp2->nature));
				printf(" IDENT TMP2 : %s\n", tmp2->ident);
				printf("GLOBAL TRUE\n");

				a=tmp2->offset;
				create_inst_lui(r1,0x1001);
				create_inst_lw(r1,a,r1);
			} else {
				if(root->opr[1]->nature == NODE_INTVAL){
					create_inst_ori(r2, get_r0(), value);
				}
				else{
					printf("GLOBAL FALSE\n");
					recup_offset(root->opr[0], 1);
					create_inst_lw(r1, a, 29);
				}
			}

			gen_code_passe_2(root->opr[1]);

			if(tmp2->global_decl==true){

				if(!tmp2){
					printf("NULL\n");
				}
				printf("\n NATURE TMP2 : %s", node_nature2string(tmp2->nature));
				printf(" IDENT TMP2 : %s\n", tmp2->ident);
				printf("GLOBAL TRUE\n");

				a=tmp2->offset;
				create_inst_lui(r2,0x1001);
				create_inst_lw(r2,a,r2);
			} else {
				if(root->opr[1]->nature == NODE_INTVAL){
					create_inst_ori(r2, get_r0(), value);
				}
				else{
					printf("j'ai fini\n");
					printf("GLOBAL FALSE\n");
					recup_offset(root->opr[1], 1);
					create_inst_lw(r2, a, 29);
				}
			}
			create_inst_slt(r1, r2, r1);
			create_inst_beq(r1, 0, l1);
			liberer_reg(flagR1);
			liberer_reg(flagR2);
			break;
		case NODE_LE:

			break;
		case NODE_GE:

			break;
		case NODE_EQ:
			gen_code_passe_2(root->opr[0]);
			r1 = return_reg1(r1);
			r2 = return_reg2(r2);
			if(root->opr[0]->decl_node->global_decl==true){
				if(!tmp2){
					printf("NULL\n");
				}
				printf("\n NATURE TMP2 : %s", node_nature2string(tmp2->nature));
				printf(" IDENT TMP2 : %s\n", tmp2->ident);
				printf("GLOBAL TRUE\n");

				a=tmp2->offset;
				create_inst_lui(r1,0x1001);
				create_inst_lw(r1,a,r1);
			} else {
				printf("GLOBAL FALSE\n");
				recup_offset(root->opr[0], 1);
				create_inst_lw(r1, a, 29);
			}

			gen_code_passe_2(root->opr[1]);
			if(root->opr[1]->decl_node->global_decl==true){
				if(!tmp2){
					printf("NULL\n");
				}
				printf("\n NATURE TMP2 : %s", node_nature2string(tmp2->nature));
				printf(" IDENT TMP2 : %s\n", tmp2->ident);
				printf("GLOBAL TRUE\n");

				a=tmp2->offset;
				create_inst_lui(r2,0x1001);
				create_inst_lw(r2,a,r2);
			} else {
				printf("GLOBAL FALSE\n");
				recup_offset(root->opr[1], 1);
				create_inst_lw(r2, a, 29);
			}
			create_inst_xor(r1, r1, r2);
			create_inst_sltiu(r1, r1, 1);
			create_inst_beq(r1, 0, l1);
			liberer_reg(flagR1);
			liberer_reg(flagR2);
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

			flagprint = 0;
			gen_code_passe_2(root->opr[0]);
			flagprint = 1 ;

			break;
		default:
			break;
			

		}
	}
}

void syscallExit(){

		create_inst_comment("exit");
		create_inst_ori(2,0,0x10);
		create_inst_syscall();
}


void ajoutStrings(int nb){

    for (int i =0;i<nb;i++){
        create_inst_asciiz(NULL,get_global_string(i));
    }
}

void ajoutInstancePrint(int emplacement){

	create_inst_comment("PRINT");
	create_inst_lui(4,0x1001);
	//printf("\nemplacement %d\n", emplacement);
    create_inst_ori(4,4,emplacement);
    create_inst_ori(2,0,0x4);
    create_inst_syscall();


}

void affectation(node_t root){
	recup_offset(root->opr[1], 1);
	recup_offset(root->opr[0], 2);
	if(reg_available()){
		int regist = get_current_reg();
		allocate_reg();
		create_inst_lw(regist, a, 29);
		create_inst_sw(regist, b, 29);
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
	recup_offset(root->opr[1], 1);
	recup_offset(root->opr[0], 2);
	if(reg_available()){
		int regist = get_current_reg();
		allocate_reg();
		create_inst_lui(regist,0x1001);
		create_inst_lw(regist, a, regist);
		create_inst_sw(regist, b, 29);
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
	//printf("Nature 1 dans off: %s\n", node_nature2string(root->nature));
	node_t tmp1 = root->decl_node;
	//printf("Nature 2 dans off: %s\n", node_nature2string(tmp1->nature));
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

int32_t return_reg1(int32_t r){
	if(reg_available()){
		r = get_current_reg();
		allocate_reg();
		flagR1 = true; //true allocate
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		r = get_current_reg();
		flagR1 = false;
	}
	return r;
}

int32_t return_reg2(int32_t r){
	if(reg_available()){
		r = get_current_reg();
		allocate_reg();
		flagR2 = true; //true allocate
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		r = get_current_reg();
		flagR2 = false;
	}
	return r;
}

void liberer_reg(bool drap){
	if(drap){
		release_reg();
	}
	else{
		pop_temporary(reg_to_free);
	}
}


void ope_binaire(node_t root, int reg1 , int reg2){

	printf("\nNature : %s\n", node_nature2string(root->nature));
	printf("REG1 %d\n",reg1);
	printf("REG2 %d\n",reg2);

switch (root->nature){

	case NODE_PLUS:
	
		create_inst_addu(r1, r1, r2);

	break;
	case NODE_MINUS:
		create_inst_subu(r1, r1, r2);
	break;
	default:
	break;
}
	create_inst_sw(r1, tmp2->offset, 29);
	liberer_reg(flagR1);
	liberer_reg(flagR2);

};