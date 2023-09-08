%{
/* João Melo, 2019216747 */
/* Miguel Faria, 2019216809 */

#include "ast.h"
#include "symbolTable.h"
#include "y.tab.h"

int yylex(void);
void yyerror(char *s);

extern int num_linha;
extern int num_coluna;

/* definicao da raiz da ast */
nodeAst* program;

/* definacao do elemento inicial da tabela de simbolos */ 
extern elementST* elem;

%}

%union{
    infoToken* infoT;
    nodeAst* node;
}

%token SEMICOLON COMMA BLANKID 
%token<infoT> ASSIGN STAR DIV MINUS PLUS MOD NE NOT
%token<infoT> EQ GE GT LE LT
%token<infoT> AND OR 
%token LBRACE LPAR LSQ RBRACE RPAR RSQ
%token PACKAGE ELSE FOR IF 
%token VAR INT FLOAT32 BOOL STRING 
%token FUNC CMDARGS 
%token<infoT> PARSEINT RETURN PRINT
%token<infoT> RESERVED REALLIT INTLIT ID STRLIT 

%type<node> Program
%type<node> Declarations VarDeclaration 
%type<node> VarSpec VarSpecOp 
%type<node> Type ParseArgs
%type<node> FuncDeclaration FuncBody FuncInvocation FuncInvocationOp FuncHeader FuncParams
%type<node> Parameters ParametersOp
%type<node> Statement StatementOp VarsAndStatements
%type<node> Expr


// Precedencias da linguagem Go e deiGo
%right ASSIGN
%left OR
%left AND
%left EQ NE LT LE GT GE
%left PLUS MINUS
%left STAR DIV MOD
%right NOT

%nonassoc LPAR RPAR

%% 

Program: PACKAGE ID SEMICOLON Declarations                                      {$$ = program = newNode("Program", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild(program, $4);}                            

Declarations: VarDeclaration SEMICOLON Declarations                             {$$ = $1; addSibling($$, $3);}             
            | FuncDeclaration SEMICOLON Declarations                            {$$ = $1; addSibling($$, $3);}
            | /* vazio */                                                       {$$ = NULL;}
            ;

VarDeclaration: VAR VarSpec                                                     {$$ = $2;}                                                
              | VAR LPAR VarSpec SEMICOLON RPAR                                 {$$ = $3;}
              ;

VarSpec: ID VarSpecOp Type                                                      {$$ = newNode("VarDecl", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, $3); addSibling($3, newNode("Id", $1, NULL));addSibling($$, $2);
                                                                                    nodeAst *current = $$->sibling, *temp;
                                                                                    while(current != NULL){
                                                                                        temp = current->child;
                                                                                        addChild(current, newNode($3->type, $3->infoT, NULL));
                                                                                        addSibling(current->child, temp); 
                                                                                        current = current->sibling;
                                                                                    }                                                                            
                                                                                }

VarSpecOp:  COMMA ID VarSpecOp                                                  {$$ = newNode("VarDecl", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, newNode("Id", $2, NULL));addSibling($$,$3);}
         |  /* vazio */                                                         {$$ = NULL;}
         ;

Type: INT                                                                       {$$ = newNode("Int", newInfoToken(NULL, num_linha, num_coluna), NULL);}
    | FLOAT32                                                                   {$$ = newNode("Float32", newInfoToken(NULL, num_linha, num_coluna), NULL);}
    | BOOL                                                                      {$$ = newNode("Bool", newInfoToken(NULL, num_linha, num_coluna), NULL);}
    | STRING                                                                    {$$ = newNode("String", newInfoToken(NULL, num_linha, num_coluna), NULL);}
    ;

FuncDeclaration: FUNC FuncHeader FuncBody                                       {$$ = newNode("FuncDecl", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$,$2); addSibling($2,$3);}

FuncHeader: ID FuncParams Type                                                  {$$ = newNode("FuncHeader", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, newNode("Id", $1, NULL)); addSibling($$->child,$3); addSibling($3,$2);}               
          | ID FuncParams                                                       {$$ = newNode("FuncHeader", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, newNode("Id", $1, NULL)); addSibling($$->child,$2);}
          ;

FuncParams: LPAR Parameters RPAR                                                {$$ = newNode("FuncParams", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$,$2);}
          | LPAR RPAR                                                           {$$ = newNode("FuncParams", newInfoToken(NULL, num_linha, num_coluna), NULL);}
          ;

Parameters: ID Type ParametersOp                                                {$$ = newNode("ParamDecl", newInfoToken(NULL, num_linha, num_coluna), NULL); addSibling($$, $3); addChild($$, $2); addSibling($2, newNode("Id", $1, NULL));}                                    

ParametersOp: COMMA ID Type ParametersOp                                        {$$ = newNode("ParamDecl", newInfoToken(NULL, num_linha, num_coluna), NULL); addSibling($$, $4); addChild($$, $3); addSibling($3, newNode("Id", $2, NULL));}
            | /* vazio */                                                       {$$ = NULL;}   
            ;

FuncBody: LBRACE VarsAndStatements RBRACE                                       {$$ = newNode("FuncBody", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$,$2);}       


VarsAndStatements: VarDeclaration SEMICOLON VarsAndStatements                   {$$ = $1; addSibling($$,$3);}
                 | Statement SEMICOLON VarsAndStatements                        {if($$ != NULL){$$ = $1; addSibling($$,$3);} else{$$ = $3;}} 
                 | SEMICOLON  VarsAndStatements                                 {$$ = $2;}
                 | /* vazio */                                                  {$$ = NULL;}
                 ;

Statement: ID ASSIGN Expr                                                       {$$ = newNode("Assign", $2, NULL); addChild($$, newNode("Id", $1, NULL)); addSibling($$->child, $3);}
         | LBRACE StatementOp RBRACE                                            {if($2!=NULL){if($2->sibling!=NULL){$$ = newNode("Block", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, $2);}else{$$ = $2;}}else $$ = $2;}
         | IF Expr LBRACE StatementOp RBRACE ELSE LBRACE StatementOp RBRACE     {$$ = newNode("If", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, $2); addSibling($2, newNode("Block", newInfoToken(NULL, num_linha, num_coluna), NULL)); addChild($2->sibling, $4); addSibling($2->sibling, newNode("Block", newInfoToken(NULL, num_linha, num_coluna), NULL)); addChild($2->sibling->sibling, $8);}
         | IF Expr LBRACE StatementOp RBRACE                                    {$$ = newNode("If", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, $2);addSibling($2, newNode("Block", newInfoToken(NULL, num_linha, num_coluna), NULL));addSibling($2->sibling, newNode("Block", newInfoToken(NULL, num_linha, num_coluna), NULL)); addChild($2->sibling, $4);}
         | FOR Expr LBRACE StatementOp RBRACE                                   {$$ = newNode("For", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, $2); addSibling($2, newNode("Block", newInfoToken(NULL, num_linha, num_coluna), NULL)); addChild($2->sibling, $4);}
         | FOR LBRACE StatementOp RBRACE                                        {$$ = newNode("For", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, newNode("Block", newInfoToken(NULL, num_linha, num_coluna), NULL)); addChild($$->child, $3);}
         | RETURN Expr                                                          {$$ = newNode("Return", $1, NULL); addChild($$, $2);}                                          
         | RETURN                                                               {$$ = newNode("Return", $1, NULL);}
         | FuncInvocation                                                       {$$ = $1;}
         | ParseArgs                                                            {$$ = $1;}
         | PRINT LPAR Expr RPAR                                                 {$$ = newNode("Print", $1, NULL); addChild($$, $3);}
         | PRINT LPAR STRLIT RPAR                                               {$$ = newNode("Print", $1, NULL); addChild($$, newNode("StrLit", $3, NULL));}
         | error                                                                {$$ = newNode(NULL, newInfoToken(NULL, num_linha, num_coluna), NULL);}
         ;

StatementOp: Statement SEMICOLON StatementOp                                    {if($$ == NULL)$$ = $3;else{$$ = $1;addSibling($$, $3);}}
           | /* vazio */                                                        {$$ = NULL;}
           ;

Expr: Expr OR Expr                                                              {$$ = newNode("Or", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr AND Expr                                                             {$$ = newNode("And", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr LT Expr                                                              {$$ = newNode("Lt", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr GT Expr                                                              {$$ = newNode("Gt", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr EQ Expr                                                              {$$ = newNode("Eq", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr NE Expr                                                              {$$ = newNode("Ne", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr LE Expr                                                              {$$ = newNode("Le", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr GE Expr                                                              {$$ = newNode("Ge", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr PLUS Expr                                                            {$$ = newNode("Add", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr MINUS Expr                                                           {$$ = newNode("Sub", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr STAR Expr                                                            {$$ = newNode("Mul", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr DIV Expr                                                             {$$ = newNode("Div", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | Expr MOD Expr                                                             {$$ = newNode("Mod", $2, NULL); addChild($$, $1); addSibling($1, $3);}
    | NOT Expr                                                                  {$$ = newNode("Not", $1, NULL); addChild($$, $2);}
    | MINUS Expr %prec NOT                                                      {$$ = newNode("Minus", $1, NULL); addChild($$, $2);}
    | PLUS Expr  %prec NOT                                                      {$$ = newNode("Plus", $1, NULL); addChild($$, $2);}
    | INTLIT                                                                    {$$ = newNode("IntLit", $1, NULL);}
    | REALLIT                                                                   {$$ = newNode("RealLit", $1, NULL);}
    | ID                                                                        {$$ = newNode("Id", $1, NULL);}
    | FuncInvocation                                                            {$$ = $1;}
    | LPAR Expr RPAR                                                            {$$ = $2;}
    | LPAR error RPAR                                                           {$$ = newNode(NULL, newInfoToken(NULL, num_linha, num_coluna), NULL);}
    ;  

FuncInvocation: ID LPAR Expr FuncInvocationOp RPAR                              {$$ = newNode("Call", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, newNode("Id", $1, NULL)); addSibling($$->child,$3); addSibling($3,$4);}    
              | ID LPAR RPAR                                                    {$$ = newNode("Call", newInfoToken(NULL, num_linha, num_coluna), NULL); addChild($$, newNode("Id", $1, NULL));}
              | ID LPAR error RPAR                                              {$$ = newNode(NULL, newInfoToken(NULL, num_linha, num_coluna), NULL);}
              ;

FuncInvocationOp: COMMA Expr FuncInvocationOp                                   {$$ = $2; addSibling($2, $3);}
                | /* vazio */                                                   {$$ = NULL;}
                ;  

ParseArgs: ID COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ Expr RSQ RPAR      {$$ = newNode("ParseArgs", $5, NULL); addChild($$, newNode("Id", $1, NULL)); addSibling($$->child, $9);}
         | ID COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR                     {$$ = newNode(NULL, newInfoToken(NULL, num_linha, num_coluna), NULL);}
         ;                                                               

%%    