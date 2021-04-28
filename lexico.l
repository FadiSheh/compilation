%{
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "defs.h"
//#include "common.h"

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
ENTIER_DEC		[0] | {CHIFFRE_NON_NUL} {CHIFFRE}*
LETTRE_HEXA 	[a-fA-F]
ENTIER_HEXA		0[X|x]|[0-9A-F] 
CHAINE_CAR		[ -!#-[]-DEL]
ENTIER           {ENTIER_HEXA} | {ENTIER_DEC}
CHAINE          '"' ({CHAINE_CAR} | '\n' | '\"') '"'
COMMENTAIRE     \/\/.*

%%

"void"          return TOK_VOID;
"for"			return TOK_FOR;
"if"			return TOK_IF;
"else"			return TOK_ELSE;
"int"			return TOK_INT;
"bool"			return TOK_BOOL;
"true"			return TOK_TRUE;
"false"			return TOK_FALSE;
"while"			return TOK_WHILE;
"do"			return TOK_DO;
"print"			return TOK_PRINT;
"("				return TOK_LPAR;
")"				return TOK_RPAR;
","				return TOK_COMMA;
"{"				return TOK_LACC;
"}"				return TOK_RACC;
";" 			return TOK_SEMICOL;
"+" 			return TOK_PLUS;
"-"             return TOK_MINUS;
"*"             return TOK_MUL;
"/"             return TOK_DIV;
"%"             return TOK_MOD;
">"             return TOK_GT;
"<"             return TOK_LT;
"!"             return TOK_NOT;
"~"             return TOK_BNOT;
"&"             return TOK_BAND;
"|"             return TOK_BOR;
"^"             return TOK_BXOR;
"="             return TOK_EQ;
">>"            return TOK_;
">>>"           return TOK_;
"<<"            return TOK_;
">="            return TOK_GE;
"<="            return TOK_LE;
"=="            return TOK_BE;
"!="            return TOK_NE;
"&&"            return TOK_AND;
"||"            return TOK_OR;

{IDF} {
                //yylval.strval = strdup(yytext);
                return TOK_IDENT;
}

{ENTIER} {
                /* A completer : affecter yylval.intval et faire des verifications */
				//yylval.intval = atoi(yytext); /*permet de transformer une chaine de caracteres en une donnee de type int*/
                return TOK_INT;
}

{CHAINE} {
                /* A completer : affecter yylval.strval */
				//yylval.strval = strdup(yytext);
                return TOK_STRING;
}

{COMMENTAIRE} {			
				return;
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

/*
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
    yylex_destroy();
    return 0;
}

*/


