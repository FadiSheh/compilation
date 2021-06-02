#include <stdio.h>
#include "common.h"
#include "defs.h"
#include "passe_2.h"
#include "utils/miniccutils.h"
#include <string.h>

int flagp2 = 0;		//global ou local
int aff=0;			//cpt des adresses des .word
int flagprint = 1;	//procédure d'affichage
node_type dernier_type2 = TYPE_NONE;
node_t tmp1;		//noeud temporaire 1
node_t tmp2;		//noeud temporaire 2

int32_t reg_to_free; //registre à liberer
int32_t a;			 //offset a
int32_t b;			 //offset b
int r1;
int r2;
int l1;				 //label 1
int l2;				 //label 2

bool flagR1 = false; //type d'allocation du registre r1
bool flagR2 = false; //type d'allocation du registre r2
int OPR=0; 			 //opérande 1 ou 2
int calcul=0; 		 //si on effectue un calcul ou pas

int value;			 //valeur recuperee dans un INTVAL ou BOOLVAL


void gen_code_passe_2(node_t root) {

	if(!root){
		//printf("Arbre est vide\n");
	}

	else{
		//printf("Nature : %s\n", node_nature2string(root->nature));
		switch(root->nature){

			case NODE_PROGRAM:
				create_program();
				create_inst_data_sec();		//creation section data
				flagp2 = 1; 	//global
				gen_code_passe_2(root->opr[0]);
				ajoutStrings(get_global_strings_number()); //ajout des chaines de caracteres
				flagp2=0; 		//local
				create_inst_text_sec();		//creation section text
				gen_code_passe_2(root->opr[1]);
				syscallExit();	//terminaison du code
			break;

			case NODE_IDENT: 
				if(flagp2==0 && strcmp(root->ident, "main")!=0){ //Si dans func

					if(root->decl_node){ 	//recuperation du noeud de la declaration
						tmp2 = root->decl_node;
					} else { 
						tmp2 = root;
					} 
				

					if (calcul==1){			//Si on effectue une opération binaire		
						if(OPR==1){
							r2=return_reg1(r2);
							identOp(tmp2,r2);
						} else if(OPR==0){
							r1=return_reg1(r1);
							identOp(tmp2,r1);
							ope_binaire(tmp2,r2,r1);
							liberer_reg(flagR1);
						}
					}

					if(flagprint==0 ){ 		//Si on effectue un affichage					
						if(tmp2->global_decl==true){ //d'une variable globale
							create_inst_lui(4,0x1001);
							create_inst_lw(4,tmp2->offset,4);
							create_inst_ori(2,0,0x1);
							create_inst_syscall();  
						} else {
							create_inst_lw(4,tmp2->offset,29);
							create_inst_ori(2,0,0x1);
							create_inst_syscall();  
						}
					} 
				}
				//Sinon il s'agit d'une declaration dans le point data
			break;

			case NODE_AFFECT:
				tmp2 = root->opr[0];

				if(root->decl_node){ //Si la variable est globale
					tmp2 = root->opr[0]->decl_node;
				}


				if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature == NODE_IDENT){
						affectation(root); //appel à la fonction d'affectation
				} 
				else if(root->opr[0]->nature == NODE_IDENT && root->opr[1]->nature != NODE_IDENT){
					if(root->opr[1]->nature != NODE_INTVAL){ //s'il ne s'agit pas d'un NODE_INTVAL
						gen_code_passe_2(root->opr[1]);
						calcul=0; 		   //declaration simple
						gen_code_passe_2(root->opr[0]);	
						if(tmp2->global_decl==true){ 
							a=tmp2->offset;
							create_inst_lui(r1,0x1001);
							create_inst_sw(r2,a,r1);
						} else {
							create_inst_sw(r2, tmp2->offset, 29);
						}
						liberer_reg(flagR2); //liberation du registre utilise
					

					} else if(root->opr[1]->nature == NODE_INTVAL){ //s'il s'agit d'un NODE_INTVAL
						r2=return_reg1(r2);
						r1=return_reg1(r1);
						create_inst_ori(r2,get_r0(),root->opr[1]->value);					
						if(tmp2->global_decl==true){
							a=tmp2->offset;
							create_inst_lui(r1,0x1001);
							create_inst_sw(r2,a,r1);
						} else {
							create_inst_sw(r2, tmp2->offset, 29);
						}
						liberer_reg(flagR2); //liberation des deux registres utilises
						liberer_reg(flagR1);
					}		
				}
				else{
					declaration_affectation(root); //il s'agit d'une declaration avec affectation
				}
			break;


			case NODE_FUNC:
				reset_temporary_max_offset(); 		//reinitialise offset maximum des temporaires
				create_inst_label_str(root->opr[1]->ident);
				set_temporary_start_offset(root->offset); //definit offset de début des temporaires
				create_inst_stack_allocation(); //crée une instruction d’allocation en pile
				gen_code(root, root->nops);
				create_inst_stack_deallocation(root->offset + get_temporary_max_offset());
			break;

			case NODE_LIST: case NODE_DECLS: case NODE_BLOCK:
				gen_code(root, root->nops);
			break;

			case NODE_DECL:
				dernier_type2 = root->opr[0]->type;
				if(root->opr[0] != NULL && root->opr[1] != NULL){
					if(flagp2 == 1){  //Si variable globale : ajout au .data et incrementation de aff
						if(root->opr[1]->nature == NODE_UMINUS){
							create_inst_word(root->opr[0]->ident, -root->opr[1]->opr[0]->value);
							aff = aff+4;
						}
						else{
							create_inst_word(root->opr[0]->ident, root->opr[1]->value);
							aff=aff+4;
						}
					}
					else { 
						if(root->opr[1]->nature == NODE_IDENT){
							declaration_affectation(root);	//declaration et affectation				
						} else { //nouvelle déclaration	
							if(reg_available()){
								allocate_reg();
								create_inst_ori(get_current_reg()-1,0x0, root->opr[1]->value);
								create_inst_sw(get_current_reg()-1, root->opr[0]->offset, 29);
								release_reg();
							}
						}			
					}
				}
				else{
					if(root->opr[0] != NULL && root->opr[1] == NULL){
						if(flagp2 == 1){							
							create_inst_word(root->opr[0]->ident, 0);
							aff=aff+4;
						}
					}
				}
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
    			int taille= strlen(root->str);	//taille de la chaine de caracteres
    			aff=aff+taille-1;	
			break;

			case NODE_INTVAL: case NODE_BOOLVAL:
				value = root->value;
				if(calcul==1){		//si on effectue une operation
					if(OPR){		//operande 1
						r2=return_reg1(r2);
						create_inst_ori(r2,get_r0(),root->value);
					} else if(OPR==0){	//operande 2
						r1=return_reg1(r1);
						create_inst_ori(r1,get_r0(),root->value);
						ope_binaire(root,r2,r1);
						liberer_reg(flagR1);
					}

				}
			break;			

			case NODE_PLUS: case NODE_MINUS: case NODE_BAND: case NODE_AND: case NODE_OR: case NODE_BXOR: case NODE_SRL: case NODE_SLL: case NODE_SRA: case NODE_BOR: case NODE_MUL: case NODE_DIV:case NODE_MOD:
				calcul=1;
				OPR=1;	//operande 1
				gen_code_passe_2(root->opr[0]);
				OPR=0;	//operande 2
				gen_code_passe_2(root->opr[1]);
				ope_binaire(root,r2,r1); //on effectue une operation binaire
			break;
		
			case NODE_IF:
				l1 = get_new_label();
				l2 = get_new_label();
				gen_code_passe_2(root->opr[0]);
				create_inst_beq(r2, 0, l1);
				if(root->opr[1] != NULL){ 	//inctructions du if
					gen_code_passe_2(root->opr[1]);
				}
				create_inst_j(l2);
				create_inst_label(l1);		//instructions du else
				if(root->opr[2] != NULL){
					gen_code_passe_2(root->opr[2]);
				}
				create_inst_label(l2);		//sortie du if
			break;

			case NODE_WHILE:
				l1 = get_new_label();
				create_inst_label(l1);
				l2 = get_new_label();
				gen_code_passe_2(root->opr[0]);
				create_inst_beq(r2, 0, l2);
				gen_code_passe_2(root->opr[1]);
				create_inst_j(l1);
				create_inst_label(l2);
			break;

			case NODE_FOR:
				gen_code_passe_2(root->opr[0]);			
				l1 = get_new_label();
				printf("%d\n", l1);
				create_inst_label(l1);
				l2 = get_new_label();

				gen_code_passe_2(root->opr[1]);
				create_inst_beq(r2, 0, l2);
				liberer_reg(flagR1);
				gen_code_passe_2(root->opr[3]);

				gen_code_passe_2(root->opr[2]);
				create_inst_comment("Retour au test de boucle");
				create_inst_j(l1);
				create_inst_label(l2);			
			break;

			case NODE_DOWHILE:
				l1 = get_new_label();
				create_inst_label(l1);
				gen_code_passe_2(root->opr[0]);				
				gen_code_passe_2(root->opr[1]);
				create_inst_bne(r2, 0, l1);
			break;			

			case NODE_EQ: case NODE_NE: case NODE_GE: case NODE_LE: case NODE_GT: case NODE_LT:
					calcul=1;
					OPR=1;
					gen_code_passe_2(root->opr[0]);
					OPR=0;
					gen_code_passe_2(root->opr[1]);
					ope_binaire(root,r2,r1);	//operation unaire
			break;

			case NODE_BNOT: case NODE_UMINUS: case NODE_NOT:
				if(flagp2 == 0){
					calcul=1;
					OPR=1;
					gen_code_passe_2(root->opr[0]);
					OPR=0;
					ope_unaire(root,r2); 	//operation unaire
				}
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

void syscallExit(){		//fonction d'exit
	create_inst_ori(2,0,10);
	create_inst_syscall();
}


void ajoutStrings(int nb){	//ajout d'une chaine de caracteres
    for (int i =0;i<nb;i++){
        create_inst_asciiz(NULL,get_global_string(i));
    }
}

void ajoutInstancePrint(int emplacement){	//ajout d'une chaine de caracteres
	create_inst_lui(4,0x1001);
    create_inst_ori(4,4,emplacement);
    create_inst_ori(2,0,0x4);
    create_inst_syscall();
}

void affectation(node_t root){		//fonction d'affectation, gestion registres
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

void declaration_affectation(node_t root){		//fonction declaration + affectation, gestion des registres
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

void recup_offset(node_t root, int32_t i){		//fonction de recuperation des offsets
	node_t tmp1 = root->decl_node;
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

int32_t return_reg1(int32_t r){		//retourne registre r1 alloue
	if(reg_available()){
		r = get_current_reg();
		allocate_reg();
		flagR1 = true;		//registre alloue normalement
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		r = get_current_reg();
		flagR1 = false;		//registre alloue avec la pile
	}
	return r;
}

int32_t return_reg2(int32_t r){		//retourne registre r2 alloue
	if(reg_available()){
		r = get_current_reg();
		allocate_reg();
		flagR2 = true; 
	}
	else{
		reg_to_free = get_restore_reg();
		push_temporary(reg_to_free);
		r = get_current_reg();
		flagR2 = false;
	}
	return r;
}

void liberer_reg(bool drap){	//free le registre en fonction de comment il a été alloué
	if(drap){
		release_reg();
	}
	else{
		pop_temporary(reg_to_free);
	}
}


void ope_binaire(node_t root, int reg1 , int reg2){		//gestion des instructions pour les operations binaires

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

		case NODE_GT:
			create_inst_slt(reg1,reg2,reg1);
		break;

		case NODE_LT:
			create_inst_slt(reg1,reg1,reg2);
		break;

		case NODE_LE:
			create_inst_slt(reg1,reg2,reg1);
			create_inst_xori(reg1,reg1,1);
		break;

		case NODE_GE:
			create_inst_slt(reg1,reg1,reg2);
			create_inst_xori(reg1,reg1,1);
		break;

		case NODE_EQ:
			create_inst_xor(reg1,reg1,reg2);
			create_inst_sltiu(reg1,reg1,1);
		break;

		case NODE_NE:
			create_inst_xor(reg1,reg1,reg2);
			create_inst_sltu(reg1,0,reg1);
		break;

		case NODE_MUL: 
			create_inst_mult(reg1,reg2);
			create_inst_mflo(reg1);
		break;

		case NODE_DIV:
			create_inst_div(reg1,reg2);
			create_inst_teq(reg2,0);
			create_inst_mflo(reg1);
		break;

		case NODE_MOD:
			create_inst_div(reg1,reg2);
			create_inst_teq(reg2,0);
			create_inst_mfhi(reg1);	
		break;

		default:
		break;
	}
};

void ope_unaire(node_t root, int reg1){		//gestion des instructions pour les opérations unaires
	switch (root->nature){
		case NODE_UMINUS:
			create_inst_subu(reg1, 0, reg1);
		break;

		case NODE_BNOT:
			create_inst_nor(reg1, 0, reg1);
		break;

		case NODE_NOT: 
			create_inst_xori(reg1, reg1,1);
		break;

		default:
		break;
	}
};

void gen_code(node_t node, int nops){	//fonction de parcours de l'arbre en fonction du nombre de fils
	for (int i = 0; i<nops; i++){
		if(node->opr[i]){
		gen_code_passe_2(node->opr[i]);}
	}
}

void identOp(node_t root,int r){		//instruction de load en fonction de si c'est global ou local
	if(root->global_decl==true){
		create_inst_lui(r,0x1001);
		create_inst_lw(r,root->offset,r);
	} else {
		create_inst_lw(r, root->offset, 29);
	}
}