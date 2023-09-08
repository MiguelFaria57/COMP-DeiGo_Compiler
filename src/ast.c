#include "ast.h"



infoToken* newInfoToken(char *value, int num_linha, int num_coluna) {
    infoToken* newInfoToken = (infoToken *)malloc(sizeof(infoToken));
    assert(newInfoToken != NULL);
    newInfoToken->value = value;
    newInfoToken->num_linha = num_linha;
    newInfoToken->num_coluna = num_coluna;
    return newInfoToken;
}


nodeAst* newNode(char *type, infoToken* infoT, char* exprNotation){
    nodeAst* newNode = (nodeAst *)malloc(sizeof(nodeAst));
    assert(newNode != NULL);
    newNode->type = type;
    newNode->infoT = infoT;
    newNode->exprNotation = exprNotation;
    newNode->child = NULL;
    newNode->sibling = NULL;
    return newNode;
} 

void addChild(nodeAst *parent, nodeAst* newMember) {
    parent->child = newMember;
}

void addSibling(nodeAst *brother, nodeAst* newMember) {
    while(brother->sibling != NULL) 
        brother = brother->sibling;
    brother->sibling = newMember;
}

void printAST(nodeAst *node, int identation, int option){
    for (int i = 0; i<identation; i++){
        printf("..");
    }
    if(option == 1){
        if (node->infoT->value != NULL){
            if(strcmp(node->type, "StrLit") == 0)
                printf("%s(\"%s)\n",node->type,node->infoT->value);
            else
                printf("%s(%s)\n",node->type,node->infoT->value);
        }
        else 
            printf("%s\n",node->type);
    }
    else if(option == 2){
        if (node->infoT->value != NULL){
            if(strcmp(node->type, "StrLit") == 0)
                printf("%s(\"%s)\n",node->type,node->infoT->value);
            else if(node->exprNotation != NULL)
                 printf("%s(%s) - %s\n",node->type,node->infoT->value, node->exprNotation);
            else
                printf("%s(%s)\n",node->type,node->infoT->value);
        } 
        else{
            if(node->exprNotation != NULL)
                printf("%s - %s\n",node->type, node->exprNotation);
            else
                printf("%s\n", node->type);
        }
    }

    if (node->child)    printAST(node->child, identation+1, option);
    if (node->sibling)  printAST(node->sibling, identation, option);

    //free(node);
}

void freeAST(nodeAst *node){
    if (node->child != NULL)    
        freeAST(node->child);
    if (node->sibling != NULL)  
        freeAST(node->sibling);
    
    if (node->infoT->value != NULL && strlen(node->infoT->value) != 0) {
        free(node->infoT->value);
        free(node->infoT);
    }
    /*if (node->infoT != NULL)
        free(node->infoT);*/
    free(node);
}