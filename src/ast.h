#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>



typedef struct infoToken{
    char *value;
    int num_linha;
    int num_coluna;
}infoToken;

typedef struct nodeAST {
    char *type;
    struct infoToken *infoT;
    char *exprNotation;
    struct nodeAST *child;
    struct nodeAST *sibling;
}nodeAst;


infoToken* newInfoToken(char *value, int num_linha, int num_coluna);
nodeAst* newNode(char *type, infoToken *infoT, char *exprNotation);
void addChild(nodeAst *parent, nodeAst *newMember);
void addSibling(nodeAst *brother, nodeAst *newMember);
void printAST(nodeAst *node, int identation, int option);
void freeAST(nodeAst *node);
#endif