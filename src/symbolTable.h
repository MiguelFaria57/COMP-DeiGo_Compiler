#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include "ast.h"



typedef struct elementST {
    char *name;
    char *paramTypes;
    char *type;
    char *param;
    int isVar;
    struct elementST *body; 
    struct elementST *nextElem;
    int line;
    int col;
    int alreadyUsed;
}elementST;


elementST* newElement(char *name, char *paramTypes, char *type, char *param, int isVar);
elementST* insertElement(elementST* lastEl, elementST* newEl);
void createSymbolTable(elementST* lastElem, nodeAst* nodeAST);
void createFunc(nodeAst* nodeAST, elementST* lastElem, elementST* start);
void getHeaderFunc(nodeAst* nodeAST, elementST* first);
void semanticAnalysisAndNotations(nodeAst* nodeAST, elementST* lastElem, char* funcName);
void checkOneMemberOperation(nodeAst* nodeAST, elementST* last, char* funcName, bool logical);
bool isNewValue(nodeAst* nodeAST);
void checkTwoMemberOperation(nodeAst* nodeAST, elementST* last, char* funcName, bool logical);
char* isDeclared(nodeAst* nodeAST, elementST* last, char* funcName);
bool isOneMemberOperationNonLogical(char* type);
bool isOneMemberOperationLogical(char* type);
bool isTwoMemberOperation(char* type);
char* getOperator(char* string);
bool isLogicalOperator(char* type);
void checkReturn(elementST* last, char*funcName, char* type, nodeAst* nodeAST);
void checkCallOperation(nodeAst* nodeAST, elementST* last, char* funcName);
char* myStrCat(char* s1, char* s2, bool free1, bool free2);
char* myToLowerCase(char* s1);
void getVars(nodeAst* nodeAST, elementST* last, elementST* start, int flag);
char* checkSymbol(elementST* st, char* name, int flag);
void printSymbolTable(elementST* elem);
void checkUnusedVariables(elementST* elem);
void freeST(elementST *elem);
#endif