%{
#define Return(X) {\
	return X;\
}
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "defs.h"
#include "common.h"
#include <inttypes.h>

#include "y.tab.h"

int yyparse(node_t * program_root);
void analyse_tree(node_t root);


char * infile = NULL;
char * outfile = DEFAULT_OUTFILE;
bool stop_after_syntax = false;
bool stop_after_verif = false;

#if YYDEBUG
extern int yydebug;
#endif


%}

%option yylineno


LETTRE          [a-zA-Z]
CHIFFRE			[0-9]
IDF             {LETTRE}(_|{LETTRE}|{CHIFFRE})*
CHIFFRE_NON_NUL [1-9]
ENTIER_DEC		[0]|{CHIFFRE_NON_NUL}{CHIFFRE}*
LETTRE_HEXA 	[a-fA-F]
ENTIER_HEXA		0[X|x]|[0-9A-F]+
CHAINE_CAR		[^\042\134]
ENTIER          {ENTIER_HEXA}|{ENTIER_DEC}
CHAINE          \042({CHAINE_CAR}|\134|\\n)*\042
COMMENTAIRE     \/\/.*

%%
"void"  Return(TOK_VOID);
"for"	Return(TOK_FOR);
"if"	Return(TOK_IF);
"else"	Return(TOK_ELSE);
"int"	Return(TOK_INT);
"bool"	Return(TOK_BOOL);
"true"	Return(TOK_TRUE);
"false" Return(TOK_FALSE);
"while" Return(TOK_WHILE);
"do"	Return(TOK_DO);
"print"	Return(TOK_PRINT);
"("		Return(TOK_LPAR);
")"		Return(TOK_RPAR);
","		Return(TOK_COMMA);
"{"		Return(TOK_LACC);
"}"		Return(TOK_RACC);
";" 	Return(TOK_SEMICOL);
"+" 	Return(TOK_PLUS);
"-"     Return(TOK_MINUS);
"*"     Return(TOK_MUL);
"/"     Return(TOK_DIV);
"%"     Return(TOK_MOD);
">"     Return(TOK_GT);
"<"     Return(TOK_LT);
"!"     Return(TOK_NOT);
"~"     Return(TOK_BNOT);
"&"     Return(TOK_BAND);
"|"     Return(TOK_BOR);
"^"     Return(TOK_BXOR);
"="     Return(TOK_AFFECT);
">>"    Return(TOK_SRL);
">>>"   Return(TOK_SRA);
"<<"    Return(TOK_SLL);
">="    Return(TOK_GE);
"<="    Return(TOK_LE);
"=="    Return(TOK_EQ);
"!="    Return(TOK_NE);
"&&"    Return(TOK_AND);
"||"    Return(TOK_OR);

{IDF} {
                yylval.strval = strdup(yytext);
                Return(TOK_IDENT);
}

{ENTIER} {
                /* A completer : affecter yylval.intval et faire des verifications */
                if (strcmp(yytext,"4294967296")>=0 && strlen(yytext)==10){

                   fprintf(stderr, "Error line %d: out of range\n", yylineno);
                    exit(1);
                }
              
                yylval.intval=atoi(yytext);
                printf("%d \n",yylval.intval);
                Return(TOK_INTVAL);
}

{CHAINE} {
                /* A completer : affecter yylval.strval */
				yylval.strval = strdup(yytext);
               
                Return(TOK_STRING);
}

{COMMENTAIRE} {			
				//return NULL;
}


[ \t\n]+

. {
                fprintf(stderr, "Error line %d: Lexical error\n", yylineno);
                exit(1);
}


%%

int yywrap(void) {
    return 1;
}


int main(int argc, char ** argv) {
    node_t program_root;
    parse_args(argc, argv);
    #if YYDEBUG
    yydebug = 1;
    #endif
    
    yyin = fopen(infile, "r");
    yyparse(&program_root);
    fclose(yyin);
    analyse_tree(program_root);
    //check_program_tree(program_root);
    yylex_destroy();
    return 0;
}




