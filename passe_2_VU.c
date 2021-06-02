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
node_t tmp3;
node_t tmp4;
node_t tmp5;
node_t tmp6;
int32_t reg_to_free;
int32_t a;
int32_t b;
int r1;
int r2;
int r3;
int gloc = 0;
bool flagR1 = false;
bool flagR2 = false;
int OPR=0;
int calcul=0;
int flagenchainement = 0;
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
				gen_code(root, root->nops);
				break;

		case NODE_IDENT: 

			if(flagp2==0){ //Si dans func

				if(root->decl_node){
					tmp2 = root->decl_node;
				} else { tmp2 = root;} //j'ai rajoute ca
			

		if (calcul==1){
			printf("COUCOU LES AMIS ZE MAPPELLE FAZI\n");
				//printf("VALEUR INTVAL %ld \n", root->value);
				if(OPR==1){
					r2=return_reg1(r2);
					identOp(tmp2,r2);
					//create_inst_ori(r2,get_r0(),root->value);

				} else if(OPR==0){

					r1=return_reg1(r1);
					identOp(tmp2,r1);
					ope_binaire(tmp2,r2,r1);
					liberer_reg(flagR1);

				}
			}


              	printf("DANS NODE IDENT %s\n", root->ident);
				if(flagprint==0 ){
				
					if(tmp2->global_decl==true){

						//create_inst_comment("PRINT VARIABLE GLOBALE ");
						create_inst_lui(4,0x1001);
						printf("PRINT\n");
						create_inst_lw(4,tmp2->offset,4);
						create_inst_ori(2,0,0x1);
						create_inst_syscall();  
					} else {

						//create_inst_comment("PRINT VARIABLE VARIABLE LOCALE");
						//create_inst_comment("printmerdeux local");
						printf("PRINT\n");
						create_inst_lw(4,tmp2->offset,29);
						create_inst_ori(2,0,0x1);
						create_inst_syscall();  


					}

				} 

			}

			break;

		case NODE_AFFECT:

			tmp2 = root->opr[0];
			if(root->decl_node){ //Si variable globale
				tmp2 = root->opr[0]->decl_node;
			}


			if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){
					affectation(root);
			} 
			else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature != NODE_IDENT){

				if(root->opr[1]->nature != NODE_INTVAL){
				//printf("ANALYSE DU FILS 2%s", node_nature2string(root->opr[1]->nature));
				gen_code_passe_2(root->opr[1]);
				//printf("ANALYSE DU FILS 1 %s", node_nature2string(root->opr[0]->nature));
				gen_code_passe_2(root->opr[0]);



					calcul=0;


				if(tmp2->global_decl==true){
					printf("\nECRASEMENT VALEUR GLOBALE\n");

					a=tmp2->offset;
					printf("VALEUR DE L'OFFSET %d\n",a);
					create_inst_lui(r1,0x1001);
					create_inst_sw(r2,a,r1);
					} else {
					printf("\nCHANGEMENT VALEUR DE VARIABLE LOCALE\n");
					create_inst_sw(r2, tmp2->offset, 29);}

					//quand c'est variable locale
					//liberer_reg(flagR1);
					liberer_reg(flagR2);
				

			} else if(root->opr[1]->nature == NODE_INTVAL){

					r2=return_reg1(r2);
					r1=return_reg1(r1);
					create_inst_ori(r2,get_r0(),root->opr[1]->value);
					

				if(tmp2->global_decl==true){
					printf("\nECRASEMENT VALEUR GLOBALE\n");

					a=tmp2->offset;
					printf("VALEUR DE L'OFFSET %d\n",a);
					create_inst_lui(r1,0x1001);
					create_inst_sw(r2,a,r1);
					} else {
					printf("\nCHANGEMENT VALEUR DE VARIABLE LOCALE\n");
					create_inst_sw(r2, tmp2->offset, 29);}
					liberer_reg(flagR2);
					liberer_reg(flagR1);

			}


	


			

			}
			else{
				declaration_affectation(root);
			}

			break;


		case NODE_FUNC:
			reset_temporary_max_offset();
			create_inst_label_str(root->opr[1]->ident);
			set_temporary_start_offset(root->offset);
			create_inst_stack_allocation();
			gen_code(root, root->nops);
			create_inst_stack_deallocation(root->offset + get_temporary_max_offset());
			break;

		case NODE_LIST:
				gen_code(root, root->nops);
			break;

		case NODE_DECLS:
				gen_code(root, root->nops);
			break;

		case NODE_DECL:

			 //Si variable globale on ajoute a .data et incremente aff
				dernier_type2 = root->opr[0]->type;
				if(root->opr[0] != NULL && root->opr[1] != NULL){

					if(flagp2 == 1){
						create_inst_word(root->opr[0]->ident, root->opr[1]->value);
						aff=aff+4;
					}

					else { if(root->opr[1]->nature == NODE_IDENT){
							//declaration et affectation
							//create_inst_comment("declaration - affectation");
							declaration_affectation(root);
					
						} else {
						//nouvelle déclaration
							//create_inst_comment("declaration");
							if(reg_available()){
								allocate_reg();
								create_inst_ori(get_current_reg()-1,0x0, root->opr[1]->value);
								create_inst_sw(get_current_reg()-1, root->opr[0]->offset, 29);
								release_reg();
							}
						}			
					}

			}
			printf("TEST\n");
			gen_code(root, root->nops);

			break;
		case NODE_TYPE:
			dernier_type2 = root->type;
			break;



		case NODE_STRINGVAL:

				create_inst_lui(4,0x1001);
    			create_inst_ori(4,4,aff);
    			create_inst_ori(2,0,0x4);
    			create_inst_syscall();

    			int taille= strlen(root->str);

    			aff=aff+taille-1;
				
			break;

		case NODE_INTVAL:
		value = root->value;
		if(calcul==1){
				printf("VALEUR INTVAL %ld \n", root->value);
				if(OPR){
					r2=return_reg1(r2);
					create_inst_ori(r2,get_r0(),root->value);

				} else if(OPR==0){

					r1=return_reg1(r1);
					create_inst_ori(r1,get_r0(),root->value);
					ope_binaire(root,r2,r1);
					liberer_reg(flagR1);

				}

		}
		break;

		case NODE_PLUS: case NODE_MINUS: case NODE_BAND: case NODE_AND: case NODE_OR: case NODE_BXOR: case NODE_SRL: case NODE_SLL: case NODE_SRA: case NODE_BOR:
				calcul=1;
				OPR=1;
				gen_code_passe_2(root->opr[0]);
				OPR=0;
				gen_code_passe_2(root->opr[1]);
				ope_binaire(root,r2,r1);
				

			break;

	
		case NODE_MUL:

			break;
		case NODE_DIV:

			break;
		case NODE_MOD:
		
			break;
		case NODE_IF:
			
			break;
		case NODE_WHILE:

			break;
		case NODE_FOR:
			gen_code_passe_2(root->opr[0]);
			create_inst_comment("Debut label");				
			l1 = get_new_label();
			printf("%d\n", l1);
			create_inst_label(l1);
			l2 = get_new_label();
			gen_code_passe_2(root->opr[1]);
			create_inst_beq(r1, 0, l2);
			gen_code_passe_2(root->opr[3]);
			
			gen_code_passe_2(root->opr[2]);
			create_inst_comment("Retour au test de boucle");
			create_inst_j(l1);
			create_inst_label(l2);
		
			break;
		case NODE_DOWHILE:

			break;
		
/*			r1 = return_reg1(r1);
			r2 = return_reg2(r2);
			gen_code_passe_2(root->opr[0]);
			recup_offset(root->opr[0], 1);
			recup_offset(root->opr[1], 2);
			create_inst_comment("dans le LT");
			if(tmp2->global_decl){
				create_inst_lw(r1, tmp2->offset, 29);
			}
			else{
				if(root->opr[1]->nature == NODE_INTVAL){
					create_inst_ori(r2, get_r0(), value);
				}
				else{

					create_inst_lw(r1, tmp2->offset, 29);
				}
			}
			gen_code_passe_2(root->opr[1]);
			if(tmp2->global_decl == true){

				create_inst_lw(r2, tmp2->offset, 9);
			}
			else{
				if(root->opr[1]->nature == NODE_INTVAL){
					create_inst_ori(r2, get_r0(), value);
				}
				else{

					create_inst_lw(r2, tmp2->offset, 29);
				}
			}

			create_inst_slt(r1, r1, r2);
			//create_inst_beq(r1, 0, l2);
			liberer_reg(flagR1);
			liberer_reg(flagR2);*/
			
			//break;
		case NODE_EQ: case NODE_NE: case NODE_GE: case NODE_LE: case NODE_GT: case NODE_LT:


				calcul=1;
				OPR=1;
				gen_code_passe_2(root->opr[0]);
				OPR=0;
				gen_code_passe_2(root->opr[1]);
				ope_binaire(root,r2,r1);

			break;

		case NODE_BNOT: case NODE_UMINUS: case NODE_NOT:

				calcul=1;
				OPR=1;
				gen_code_passe_2(root->opr[0]);
				OPR=0;
				ope_unaire(root,r2);

			break;

		case NODE_PRINT:

			flagprint = 0;
			gen_code(root, root->nops);
			flagprint = 1 ;

			break;

		default:
			break;
			

		}
	}
}

void syscallExit(){

		//create_inst_comment("exit");
		create_inst_ori(2,0,10);
		create_inst_syscall();
}


void ajoutStrings(int nb){

    for (int i =0;i<nb;i++){
        create_inst_asciiz(NULL,get_global_string(i));
    }
}

void ajoutInstancePrint(int emplacement){

	//create_inst_comment("PRINT");
	create_inst_lui(4,0x1001);
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
		
		printf("amelie bouffonne\n");
		create_inst_lw(regist, a, 29);
		create_inst_sw(regist, b, 29);
		release_reg();
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		printf("Bye1\n");
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
		printf("BQGQERG\n");
		create_inst_lw(regist, a, regist);
		create_inst_sw(regist, b, 29);
		release_reg();
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		create_inst_lui(reg_to_free, 0x1001);
		printf("Bye3");
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

switch (root->nature){

	case NODE_PLUS:
		create_inst_addu(reg1, reg1, reg2);
	break;

	case NODE_MINUS:
		create_inst_subu(reg1, reg1, reg2);
	break;

	case NODE_BAND: case NODE_AND:
		create_inst_and(reg1,reg1,reg2);
	break;

	case NODE_OR: case NODE_BOR:
		create_inst_or(reg1,reg1,reg2);
	break;

	case NODE_BXOR:
		create_inst_xor(reg1,reg1,reg2);
	break;

	case NODE_SRL:
		create_inst_srav(reg1,reg1,reg2);
	break;

	case NODE_SLL:
		create_inst_sllv(reg1,reg1,reg2);
	break;

	case NODE_SRA:
		create_inst_srlv(reg1,reg1,reg2);
	break;

	case NODE_GT://CONFIRMED
		create_inst_slt(reg1,reg2,reg1);
	break;
	case NODE_LT://CONFIRMED
		create_inst_slt(reg1,reg2,reg1);
	break;
	case NODE_LE://CONFIRMED
		create_inst_slt(reg1,reg2,reg1);
		create_inst_xori(reg1,reg1,1);
	break;

	case NODE_GE://CONFIRMED
		create_inst_slt(reg1,reg1,reg2);
		create_inst_xori(reg1,reg1,1);
	break;

	case NODE_EQ://CONFIRMED
		create_inst_xor(reg1,reg1,reg2);
		create_inst_sltiu(reg1,reg1,1);
	break;

	case NODE_NE://CONFIRMED
		create_inst_xor(reg1,reg1,reg2);
		create_inst_sltu(reg1,0,reg1);
	break;

	default:
	break;
}


};





void ope_unaire(node_t root, int reg1){

switch (root->nature){

	case NODE_UMINUS:
		create_inst_subu(reg1, 0, reg1);
	break;

	case NODE_BNOT: //pr les entiers
		create_inst_nor(reg1, 0, reg1);
	break;

	case NODE_NOT: //pr les booleens
		create_inst_xori(reg1, reg1,1);
	break;
	default:
	break;
}



};





void gen_code(node_t node, int nops){
	for (int i = 0; i<nops; i++){
		if(node->opr[i]){
		gen_code_passe_2(node->opr[i]);}
	}
}

node_t updateTMP(node_t tmp, node_t root,int i,int r){
	
	tmp = root->opr[i];
	//printf("NOM TMP DANS UPDATE : %s ", tmp->ident);
					if(tmp->decl_node){
						tmp = root->opr[i]->decl_node;}

				if(tmp->global_decl==true){
					//printf("GLOBAL TRUE\n");

					a=tmp->offset;
					create_inst_lui(r,0x1001);
					printf("Bye5");
					create_inst_lw(r,a,r);
				} else {
					//printf("GLOBAL FALSE\n");
					recup_offset(root->opr[i], 1);
					printf("Bye6");
					create_inst_lw(r, a, 29);
				}

	return tmp;

}



void identOp(node_t root,int r){
	create_inst_comment("LOAD VARIABLE DE LA MEMOIRE");
				if(root->global_decl==true){
					create_inst_lui(r,0x1001);
					printf("LOL\n");
					create_inst_lw(r,root->offset,r);
				} else {
					printf("LO3\n");
					create_inst_lw(r, root->offset, 29);
				}


}