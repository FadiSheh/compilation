
#include <stdio.h>

#include "defs.h"
#include "passe_1.h"
#include "./utils/miniccutils.h"
extern int trace_level;
context_t list_context;
void analyse_passe_1(node_t root) {

	if (!root){
		printf("il est vide\n");
		return;
	}

	switch (root->nature){

		case NODE_PROGRAM:
		    list_context = create_context();
			push_global_context();
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;

		case NODE_DECLS:
			analyse_passe_1(root->opr[0]);
			analyse_passe_1(root->opr[1]);
			break;

		case NODE_IDENT:
			
			printf("OFFSET: %d \n",get_env_current_offset());
			break;

		case NODE_STRINGVAL:
			break;

		case NODE_FUNC:
			break;

		case NODE_AFFECT:
			break;

		case NODE_DECL:
			break;

		case NODE_TYPE:
			break;

		case NODE_LIST:
			break;

		default:
			fprintf(stderr, "Error line %d: Node error %d\n", root->lineno, root->nature);
			break;


	}

	
}





/*

    node_nature nature;
    node_type type;

    int64_t value;
    int32_t offset;
    bool global_decl;
    int32_t lineno;

    int32_t nops;
    struct _node_s ** opr;
    
    struct _node_s * decl_node;

    char * ident;
    char * str;

    // Pour l'affichage du graphe
    int32_t node_num;



    */