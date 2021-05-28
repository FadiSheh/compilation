#include <stdio.h>

#include "defs.h"
#include "passe_2.h"

int aff = 0;
//int flag = 0;
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
        switch(root->nature){
            case NODE_PROGRAM:
                printf("helloworld\n");
                create_program();
                create_inst_data_sec();
                /*.data
                start :
                end:
                .asciiz
                .text
                main:*/
                //flag=1;
                ajoutStrings(get_global_strings_number());
                gen_code_passe_2(root->opr[0]);
                
                create_inst_text_sec();
                gen_code_passe_2(root->opr[1]);

                create_inst_ori(2,0,10);
                create_inst_syscall();
            
                break;
            case NODE_BLOCK: 


            if(root->opr[0] != NULL){
                gen_code_passe_2(root->opr[0]);
                
            }
            if(root->opr[1] != NULL){
                gen_code_passe_2(root->opr[1]);
                
            }

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
            set_temporary_start_offset(root->offset);
        //II-10
            create_inst_stack_allocation();
            gen_code_passe_2(root->opr[0]);
            gen_code_passe_2(root->opr[1]);
            gen_code_passe_2(root->opr[2]);
            //place à allouer en pile
            //somme entre offset du noeud NODE_FUNC et valeur 8
            //+place variables locales ? II-9
            create_inst_stack_deallocation(get_temporary_max_offset());
            break;

        case NODE_LIST:
            printf("NODE_LIST");
            gen_code_passe_2(root->opr[0]);
            gen_code_passe_2(root->opr[1]);
            break;

        case NODE_DECLS:
            gen_code_passe_2(root->opr[1]);
            break;

        case NODE_DECL:
            create_inst_word(root->opr[0]->ident, root->opr[1]->value);
            break;

        case NODE_PRINT:
            create_inst_lui(4,0x1001);
            create_inst_ori(4,4,(0+aff)*4);
            aff++;
            create_inst_ori(2,0,4);
            create_inst_syscall();

            break;
        default:
            break;
            

        
    }

}


}


void ajoutStrings(int nb){

    for (int i =0;i<nb;i++){
        create_inst_asciiz(NULL,get_global_string(i));
    }
}