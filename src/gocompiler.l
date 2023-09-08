/* João Melo, 2019216747 */
/* Miguel Faria, 2019216809 */
	 
%X TEST_STRLIT TEST_ESC_SEQ TEST_COMMENT

%{


#include "ast.h"
#include "symbolTable.h"
//#include "generateLLVM.h"
#include "y.tab.h"


int yyparse(void);

int tokens_erros = 0;
int ast = 0;
int erro_sintaxe = 0;
int erro_semantica = 0;

int aux_nl = 0;
int aux_nc = 0;

int insert_semicolon = 0;
int num_linha = 1;
int num_coluna = 1;
int pos_aspa = 0;
int pos_com_l = 0;
int pos_com_c = 0; 

extern nodeAst* program;
elementST* elem;

%}

numero			[0-9]
letra_m			[a-z]
letra_M			[A-Z]

reserved 		("break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type"|"--"|"++")
reallit			(({numero}+)?"."{numero}+([Ee]([+-])?{numero}+)?|{numero}+"."([Ee]([+-])?{numero}+)?|{numero}+[eE]([+-])?{numero}+)
intlit			(0|0[xX][A-Fa-f0-9]+|0[0-7]+|[1-9][0-9]*)
strlit 			(\"([^\\\"\n\r]|\\[fnrt"\\])*\")
id   			(({letra_m}|{letra_M}|"_")+({numero}|{letra_m}|{letra_M}|"_")*)

octal_error    	(0([0-7]*[8-9]+[0-7]*)+)

%%

\"				{BEGIN TEST_STRLIT;
				pos_aspa = num_coluna;
				num_coluna += yyleng;
				}

"/*"			{BEGIN TEST_COMMENT;
				pos_com_l = num_linha;
				pos_com_c = num_coluna;
				num_coluna += yyleng;}

";"				{if (tokens_erros == 1)
					printf("SEMICOLON\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return SEMICOLON;}
","				{if (tokens_erros == 1)
					printf("COMMA\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return COMMA;}
"_"				{if (tokens_erros == 1)
					printf("BLANKID\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return BLANKID;}
"="				{if (tokens_erros == 1)
					printf("ASSIGN\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return ASSIGN;}}
"*"				{if (tokens_erros == 1)
					printf("STAR\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return STAR;}}
"/"				{if (tokens_erros == 1)
					printf("DIV\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return DIV;}}
"-"				{if (tokens_erros == 1)
					printf("MINUS\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return MINUS;}}
"+"				{if (tokens_erros == 1)
					printf("PLUS\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return PLUS;}}
"=="			{if (tokens_erros == 1)
					printf("EQ\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return EQ;}}
">="			{if (tokens_erros == 1)
					printf("GE\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return GE;}}
">"				{if (tokens_erros == 1)
					printf("GT\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return GT;}}
"{"				{if (tokens_erros == 1)
					printf("LBRACE\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return LBRACE;}
"<="			{if (tokens_erros == 1)
					printf("LE\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1){
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return LE;}}
"("				{if (tokens_erros == 1)
					printf("LPAR\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return LPAR;}
"["				{if (tokens_erros == 1)
					printf("LSQ\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return LSQ;}
"<"				{if (tokens_erros == 1)
					printf("LT\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1){ 
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return LT;}}
"%"				{if (tokens_erros == 1)
					printf("MOD\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1){
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return MOD;}}
"!="			{if (tokens_erros == 1)
					printf("NE\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1){ 
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return NE;}}
"!"				{if (tokens_erros == 1)
					printf("NOT\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1){ 
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return NOT;}}
"&&"			{if (tokens_erros == 1)
					printf("AND\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return AND;}}
"||"			{if (tokens_erros == 1)
					printf("OR\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return OR;}}
"}"				{if (tokens_erros == 1)
					printf("RBRACE\n");
				insert_semicolon = 1;
				num_coluna += yyleng;
				if (ast == 1)
					return RBRACE;}
")"				{if (tokens_erros == 1)
					printf("RPAR\n");
				insert_semicolon = 1;
				num_coluna += yyleng;
				if (ast == 1)
					return RPAR;}
"]"				{if (tokens_erros == 1)
					printf("RSQ\n");
				insert_semicolon = 1;
				num_coluna += yyleng;
				if (ast == 1)
					return RSQ;}
"package"		{if (tokens_erros == 1)
					printf("PACKAGE\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return PACKAGE;}
"return"		{if (tokens_erros == 1)
					printf("RETURN\n");
				insert_semicolon = 1;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return RETURN;}}
"else"			{if (tokens_erros == 1)
					printf("ELSE\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return ELSE;}
"for"			{if (tokens_erros == 1)
					printf("FOR\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return FOR;}
"if"			{if (tokens_erros == 1)
					printf("IF\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return IF;}
"var"			{if (tokens_erros == 1)
					printf("VAR\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return VAR;}
"int"			{if (tokens_erros == 1)
					printf("INT\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					return INT;}}
"float32"		{if (tokens_erros == 1)
					printf("FLOAT32\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return FLOAT32;}
"bool"			{if (tokens_erros == 1)
					printf("BOOL\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return BOOL;}
"string"		{if (tokens_erros == 1)
					printf("STRING\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return STRING;}
"fmt.Println"	{if (tokens_erros == 1)
					printf("PRINT\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna+1);
					yylval.infoT = info;
					return PRINT;}}
"strconv.Atoi"	{if (tokens_erros == 1)
					printf("PARSEINT\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(NULL, num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return PARSEINT;}}
"func"			{if (tokens_erros == 1)
					printf("FUNC\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return FUNC;}
"os.Args"		{if (tokens_erros == 1)
					printf("CMDARGS\n");
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1)
					return CMDARGS;}

({reserved})	{if (tokens_erros == 1)
					printf("RESERVED(%s)\n", yytext);
				insert_semicolon = 0;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(strdup(yytext), num_linha, num_coluna-yyleng); 
					yylval.infoT = info;
					return RESERVED;}}
({reallit})		{if (tokens_erros == 1)
					printf("REALLIT(%s)\n", yytext);
				insert_semicolon = 1;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(strdup(yytext), num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return REALLIT;}}
({intlit})		{if (tokens_erros == 1)
					printf("INTLIT(%s)\n", yytext);
				insert_semicolon = 1;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(strdup(yytext), num_linha, num_coluna-yyleng); 
					yylval.infoT = info;
					return INTLIT;}}
({id})			{if (tokens_erros == 1)
					printf("ID(%s)\n", yytext);
				insert_semicolon = 1;
				num_coluna += yyleng;
				if (ast == 1) {
					infoToken* info = newInfoToken(strdup(yytext), num_linha, num_coluna-yyleng);
					yylval.infoT = info;
					return ID;}}

({octal_error})	{printf("Line %d, column %d: invalid octal constant (%s)\n", num_linha, num_coluna, yytext);
				num_coluna += yyleng;
				insert_semicolon = 0;}


" "				{num_coluna += yyleng;}
\t				{num_coluna += yyleng;}
\n				{aux_nl = num_linha;
				aux_nc = num_coluna;
				num_linha++;
				num_coluna = 1;
				if (insert_semicolon == 1) {
					if (tokens_erros == 1)
						printf("SEMICOLON\n");
					insert_semicolon = 0;
					if (ast == 1) {
						return SEMICOLON;}}
				}

"//".*			{num_coluna += yyleng;}
.				{printf("Line %d, column %d: illegal character (%s)\n", num_linha, num_coluna, yytext);
				insert_semicolon = 0;
				num_coluna += yyleng;}


<TEST_STRLIT><<EOF>> 										{printf("Line %d, column %d: unterminated string literal\n", num_linha, pos_aspa);
															insert_semicolon = 0;
															return 0;
															}
<TEST_STRLIT>((([^\\\"\n\r]|\\[fnrt"\\])*)\\)				{num_coluna += yyleng;
															printf("Line %d, column %d: invalid escape sequence (%s)\n", num_linha, num_coluna-1, &yytext[yyleng-1]);
															insert_semicolon = 0;
															BEGIN TEST_ESC_SEQ;}										
<TEST_STRLIT>((([^\\\"\n\r]|\\[fnrt"\\])*)\\[^fnrt"\\\n])	{num_coluna += yyleng;
															printf("Line %d, column %d: invalid escape sequence (%s)\n", num_linha, num_coluna-2, &yytext[yyleng-2]);
															insert_semicolon = 0;
															BEGIN TEST_ESC_SEQ;}
<TEST_STRLIT>((([^\\\"\n\r]|\\[fnrt"\\])*)\n)				{printf("Line %d, column %d: unterminated string literal\n", num_linha, pos_aspa);
															insert_semicolon = 0;
															num_linha++;
															num_coluna = 1;
															BEGIN 0;
															}
<TEST_STRLIT>([^\\\"\n\r]|\\[fnrt"\\])+\"					{if (tokens_erros == 1) 
																printf("STRLIT(\"%s)\n", yytext);
															insert_semicolon = 1;
															num_coluna += yyleng;
															BEGIN 0;
															if (ast == 1) {
																infoToken* info = newInfoToken(strdup(yytext), num_linha, num_coluna-yyleng); 
																yylval.infoT = info;
																return STRLIT;}}
<TEST_STRLIT>([^\\\"\n\r]|\\[fnrt"\\])+						{;}
<TEST_STRLIT>\"												{if ((num_coluna - pos_aspa) == 1) {
																printf("STRLIT(\"%s)\n", yytext);
																insert_semicolon = 1;}
															num_coluna += yyleng;
															BEGIN 0;
															if (ast == 1) {
																infoToken* info = newInfoToken(strdup(yytext), num_linha, num_coluna-yyleng); 
																yylval.infoT = info;
																return STRLIT;}}


<TEST_ESC_SEQ>\\								{num_coluna += yyleng;
												printf("Line %d, column %d: invalid escape sequence (%s)\n", num_linha, num_coluna-1, &yytext[yyleng-1]);
												insert_semicolon = 0;}										
<TEST_ESC_SEQ>\\[^fnrt"\\\n\r]					{num_coluna += yyleng;
												printf("Line %d, column %d: invalid escape sequence (%s)\n", num_linha, num_coluna-2, &yytext[yyleng-2]);
												insert_semicolon = 0;}
<TEST_ESC_SEQ>\n								{printf("Line %d, column %d: unterminated string literal\n", num_linha, pos_aspa);
												insert_semicolon = 0;
												num_linha++;
												num_coluna = 1;
												BEGIN 0;}
<TEST_ESC_SEQ><<EOF>> 							{printf("Line %d, column %d: unterminated string literal\n", num_linha, pos_aspa);
												insert_semicolon = 0;
												return 0;
												}
<TEST_ESC_SEQ>([^\\\"\n\r]|\\[fnrt"\\])*		{num_coluna += yyleng;}
<TEST_ESC_SEQ>\"								{num_coluna += yyleng;
												BEGIN 0;}


<TEST_COMMENT>"*/"			{num_coluna += yyleng;
							BEGIN 0;}
<TEST_COMMENT>\n			{num_linha++;
							num_coluna = 1;}
<TEST_COMMENT>.				{num_coluna += yyleng;}
<TEST_COMMENT><<EOF>>		{if (insert_semicolon == 1) {
								if (tokens_erros == 1) {
									printf("SEMICOLON\n");}
								insert_semicolon = 0;
								if (ast == 1) {
									return SEMICOLON;}}
							printf("Line %d, column %d: unterminated comment\n", pos_com_l, pos_com_c);
							return 0;} 

<<EOF>>			{if (insert_semicolon == 1) {
					if (tokens_erros == 1) {
						printf("SEMICOLON\n");
						insert_semicolon = 0;
					}
					if (ast == 1) {
						insert_semicolon = 0;
						return SEMICOLON;
					}
				}
				return 0;}


%%

int main(int argc, char **argv) {
	if (argc == 1) {
		ast = 1;
		yyparse();
		if (erro_sintaxe != 1) {
			elem = newElement("", "", "", "", 0);
			createSymbolTable(elem, program);
			semanticAnalysisAndNotations(program, elem, NULL);
			checkUnusedVariables(elem);

			/*if (erro_semantica == 0) {
				generate(program, elem);
			}*/

			freeST(elem);
			freeAST(program);
		}
	}
	else if ((argc == 2) && (strcmp(argv[1],"-l") == 0)) {
		tokens_erros = 1;
		yylex();
	}
	else if ((argc == 2) && (strcmp(argv[1],"-t") == 0)) {
		ast = 1;
		yyparse();
		if (erro_sintaxe != 1) {
			printAST(program,0,1);
			freeAST(program);
		}
	}
	else if ((argc == 2) && (strcmp(argv[1],"-s") == 0)) {
		ast = 1;
		yyparse();
		if (erro_sintaxe != 1) {
			elem = newElement("", "", "", "", 0);
			createSymbolTable(elem, program);
			semanticAnalysisAndNotations(program, elem, NULL);
			checkUnusedVariables(elem);
			printSymbolTable(elem->nextElem);
			printAST(program,0,2);
			freeST(elem);
			freeAST(program);
		}
	}

	return 0;
} 

int yywrap() {
	return 1;
}

void yyerror (char *s) { 
	erro_sintaxe = 1;

	char temp[yyleng+1];
	strcpy(temp, yytext-1);
	if(num_coluna-yyleng == 0){
		num_coluna++;
	}
	if (strcmp(yytext, "\n") == 0) {
		printf ("Line %d, column %d: %s: %s\n", aux_nl, aux_nc, s, yytext); 
	}
	else {
		if(yytext[yyleng-1] == '\"' && temp[0] == '\"')
			printf ("Line %d, column %d: %s: %s\n", num_linha, num_coluna-yyleng-1, s, yytext-1);
		else
			printf ("Line %d, column %d: %s: %s\n", num_linha, num_coluna-yyleng, s, yytext);
	}
}