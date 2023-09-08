#include "symbolTable.h"
#include "ast.h"

extern int erro_semantica;


elementST* global;


elementST* newElement(char *name, char *paramTypes, char *type, char *param, int isVar){
    elementST* newElement = (elementST *)malloc(sizeof(elementST));
    assert(newElement != NULL);
    newElement->name = name;
    newElement->paramTypes = paramTypes;
    newElement->type = type;
    newElement->param = param;
    newElement->isVar = isVar;
    newElement->body = NULL;
    newElement->nextElem = NULL;
    newElement->line = 0;
    newElement->col = 0;
    newElement->alreadyUsed = 0;
    return newElement;
} 

elementST* insertElement(elementST* lastEl, elementST* newEl){
    lastEl->nextElem = newEl;
    return newEl;
}


void createSymbolTable(elementST* lastElem, nodeAst* nodeAST){
    if (strcmp(nodeAST->type, "Program") == 0) {
        if (nodeAST->child != NULL) {
            global = lastElem;
            createSymbolTable(lastElem, nodeAST->child);
        }
    }
    else if (strcmp(nodeAST->type, "VarDecl") == 0) {
        getVars(nodeAST, lastElem, global, 0);
    }
    else if (strcmp(nodeAST->type, "FuncDecl") == 0) {
        createFunc(nodeAST, lastElem, global);
    }
    while(lastElem->nextElem)
        lastElem = lastElem->nextElem;

    if (nodeAST->sibling != NULL) {
        createSymbolTable(lastElem, nodeAST->sibling);
    }
}


void getVars(nodeAst* nodeAST, elementST* last, elementST* start, int flag){
    elementST* temp;
    if(nodeAST != NULL){
        if(strcmp(nodeAST->type, "VarDecl") == 0){
            if (checkSymbol(start, nodeAST->child->sibling->infoT->value, flag) == NULL) {
                temp = newElement(nodeAST->child->sibling->infoT->value, "", myToLowerCase(nodeAST->child->type), "", 1);
                temp->line = nodeAST->child->sibling->infoT->num_linha;
                temp->col = nodeAST->child->sibling->infoT->num_coluna;
                if(flag == 1){
                    last->body = temp;
                    last = temp;   
                }  
                else if(flag == 0){
                    insertElement(last, temp);
                }   
            } 
            else {
                erro_semantica = 1;                
                printf("Line %d, column %d: Symbol %s already defined\n", nodeAST->child->sibling->infoT->num_linha, nodeAST->child->sibling->infoT->num_coluna, nodeAST->child->sibling->infoT->value);
            }
        }                                                          
    }
    if(nodeAST != NULL && flag == 1)    
        getVars(nodeAST->sibling, last, start, flag);  
}





void createFunc(nodeAst* nodeAST, elementST* lastElem, elementST* start){
    // name da funcao
    char* name = nodeAST->child->child->infoT->value;
    // verificar se ja existe
    if (checkSymbol(start, name, 0) == NULL) {
        // type da funcao
        char* type;
        if(strcmp(nodeAST->child->child->sibling->type,"FuncParams") == 0)
            type = "none";
        else
            type = myToLowerCase(nodeAST->child->child->sibling->type);  
        
        // paramTypes da funcao
        nodeAst* temp = nodeAST->child; // FuncHeader
        elementST* newFunc = newElement(name, "", type, "", 0);

        lastElem = insertElement(lastElem,newFunc);
        
        getHeaderFunc(temp->child, newFunc);
        
        // variaveis definidas na funcao
        temp = temp->sibling; //FuncBody
        elementST* tempElBody = newFunc;
        while(tempElBody->body != NULL)
            tempElBody = tempElBody->body;
        if (temp->child != NULL)
            getVars(temp->child, tempElBody, newFunc, 1);
    }
    else {
        erro_semantica = 1;
        printf("Line %d, column %d: Symbol %s already defined\n", nodeAST->child->child->infoT->num_linha, nodeAST->child->child->infoT->num_coluna, nodeAST->child->child->infoT->value);
    }
}

void getHeaderFunc(nodeAst* nodeAST, elementST* first) {
    first->body = newElement("return", "", myToLowerCase(first->type), "", 2);
    elementST* tempEl = first->body;
    
    nodeAst* temp = nodeAST;
    char* paramType = "";
    
    while (temp != NULL && strcmp(temp->type, "FuncParams") != 0) {
        temp = temp->sibling;
    }
    
    if(temp != NULL && temp->child != NULL){
        temp = temp->child;
        elementST* newEl; 
        while (temp != NULL && strcmp(temp->type, "ParamDecl") == 0) {
            // no body
            if(checkSymbol(first->body, temp->child->sibling->infoT->value, 1) == NULL){
                newEl = newElement(temp->child->sibling->infoT->value, "", myToLowerCase(temp->child->type), "param", 2);
                newEl->line = temp->child->sibling->infoT->num_linha;
                newEl->col = temp->child->sibling->infoT->num_coluna;
                tempEl->body = newEl;
                tempEl = newEl;
                // atualizar paramTypes
                if(strlen(paramType) == 0)
                    paramType = myStrCat(paramType, myToLowerCase(temp->child->type), false, true);
                else{
                    paramType = myStrCat(paramType, ",", true, false);
                    paramType = myStrCat(paramType, myToLowerCase(temp->child->type),true, true);
                }
            }
           else {
                erro_semantica = 1;
                printf("Line %d, column %d: Symbol %s already defined\n", temp->child->sibling->infoT->num_linha, temp->child->sibling->infoT->num_coluna, temp->child->sibling->infoT->value);
            }

            temp = temp->sibling;
        }

        if(strlen(paramType) != 0)
            first->paramTypes = paramType;
    }
}

char* checkSymbol(elementST* st, char* name, int flag) {
    while (st != NULL) {
        if (strcmp(st->name, name) == 0)
            return st->type;

        if (flag == 0)
            st = st->nextElem;
        else if (flag == 1)
            st = st->body;
    }

    return NULL;
}

bool functionExists(elementST* lastElem, char* funcName){
    while(lastElem != NULL) {
        if(strcmp(lastElem->name, funcName)==0 && lastElem->isVar == 0) {
            if (lastElem->alreadyUsed == 0) {
                lastElem->alreadyUsed = 1;
                return true;
            }
            else {
                return false;
            }
        }
        lastElem = lastElem->nextElem;
    }

    return false;
}

void semanticAnalysisAndNotations(nodeAst* nodeAST, elementST* lastElem, char* funcName){
    
    if(strcmp(nodeAST->type, "FuncDecl")==0){
        funcName = nodeAST->child->child->infoT->value;
        if(functionExists(lastElem, funcName)){
            if (nodeAST->child)    
                semanticAnalysisAndNotations(nodeAST->child, lastElem, funcName);
            if (nodeAST->sibling)  
                semanticAnalysisAndNotations(nodeAST->sibling, lastElem, NULL);
        }
        else{
            if (nodeAST->sibling)  
                semanticAnalysisAndNotations(nodeAST->sibling, lastElem, NULL);
        }
    }
    else{
        if (nodeAST->child)    
            semanticAnalysisAndNotations(nodeAST->child, lastElem, funcName);
        if (nodeAST->sibling)  
            semanticAnalysisAndNotations(nodeAST->sibling, lastElem, funcName);
    }
    
    if(isTwoMemberOperation(nodeAST->type)){
        checkTwoMemberOperation(nodeAST, lastElem, funcName, false);         // Add div etc
    }
    else if(isLogicalOperator(nodeAST->type)){
        checkTwoMemberOperation(nodeAST, lastElem, funcName, true);          // Eq Lt Gt etc
    }

    else if(isNewValue(nodeAST));                                            // se estiver só Intlit Reallit
        
    else if(isOneMemberOperationNonLogical(nodeAST->type)){
        checkOneMemberOperation(nodeAST, lastElem, funcName, false);
    }

    else if(isOneMemberOperationLogical(nodeAST->type)){
        checkOneMemberOperation(nodeAST, lastElem, funcName, true);
    }
    else if(strcmp(nodeAST->type, "Call") == 0){
        checkCallOperation(nodeAST, lastElem, funcName);
    }  
}

void checkCallOperation(nodeAst* nodeAST, elementST* last, char* funcName){
    int exists = 0;
    char* type = "undef";
    char* paramTypes = "\0";
    char* params = "\0";
    elementST* lastCopy = last;
    while(last != NULL){
        if(strcmp(last->name, nodeAST->child->infoT->value)==0) {
            exists = 1;
            type = last->type;
            paramTypes = last->paramTypes;
            char* aux = "(";
            aux = myStrCat(aux, paramTypes, false, false);
            aux = myStrCat(aux, ")", true, false);
            nodeAST->child->exprNotation = aux;
        }
                
        last = last->nextElem;
    }
 
    nodeAst* copy = nodeAST->child->sibling;
    char* typeVar;
    while (copy != NULL) {
        if(strcmp(copy->type, "Id") == 0){
            typeVar = isDeclared(copy, lastCopy, funcName);
            copy->exprNotation = typeVar;
            if(strcmp(typeVar,"undef")==0){
                printf("Line %d, column %d: Cannot find symbol %s\n", copy->infoT->num_linha,copy->infoT->num_coluna,copy->infoT->value);
                erro_semantica = 1;
                }
        }
        if(strcmp(copy->type, "Call") == 0 && copy->exprNotation == NULL){
            if(strlen(params) == 0)
                params = myStrCat(params, "none", false, false);
            else{
                params = myStrCat(params, ",", true, false);
                params = myStrCat(params, "none",true, false);
            }
        }
        else{ 
            if(strlen(params) == 0)
                params = myStrCat(params, myToLowerCase(copy->exprNotation), false, true);
            else{
                params = myStrCat(params, ",", true, false);
                params = myStrCat(params, myToLowerCase(copy->exprNotation),true, true);
            }
        }

        copy = copy->sibling;
    }
    
    if (exists == 1 && strcmp(paramTypes, params) != 0) {
        printf("Line %d, column %d: Cannot find symbol %s(%s)\n", nodeAST->child->infoT->num_linha,nodeAST->child->infoT->num_coluna,nodeAST->child->infoT->value,params);
        erro_semantica = 1;
        type = "undef";
    }
    else if (exists == 0) {
        nodeAST->child->exprNotation = "undef";
        printf("Line %d, column %d: Cannot find symbol %s(%s)\n", nodeAST->child->infoT->num_linha,nodeAST->child->infoT->num_coluna,nodeAST->child->infoT->value,params);
        erro_semantica = 1;
        type = "undef";
    }
    if(strcmp(type, "none")!= 0)
        nodeAST->exprNotation = type;
    if (strcmp(params, "\0") != 0)
        free(params);
}


void checkOneMemberOperation(nodeAst* nodeAST, elementST* last, char* funcName, bool logical){
    char* type;
    if(nodeAST->child == NULL)
            type = "none";
    else if(strcmp(nodeAST->child->type, "Id") == 0){
        type = isDeclared(nodeAST->child, last, funcName);
        nodeAST->child->exprNotation = type;
        if(strcmp(type,"undef")==0){
           printf("Line %d, column %d: Cannot find symbol %s\n", nodeAST->child->infoT->num_linha,nodeAST->child->infoT->num_coluna,nodeAST->child->infoT->value);
            erro_semantica = 1;
        }
    }
    else{
        type = nodeAST->child->exprNotation;            
    }
    if(type != NULL && strcmp(type, "undef") != 0){
        if(logical){ 
            if(strcmp(type, "bool") == 0){
                if(strcmp(nodeAST->type, "If") != 0 && strcmp(nodeAST->type, "For") != 0)
                    nodeAST->exprNotation = "bool";
            }
            
            else{
                 if(strcmp(nodeAST->type, "If") == 0 || strcmp(nodeAST->type, "For") == 0){
                    printf("Line %d, column %d: Incompatible type %s in %s statement\n", nodeAST->child->infoT->num_linha, nodeAST->child->infoT->num_coluna, type, getOperator(nodeAST->type));
                    erro_semantica = 1;  
                    }
                else{
                    printf("Line %d, column %d: Operator %s cannot be applied to type %s\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, getOperator(nodeAST->type), type);
                    erro_semantica = 1;
                    nodeAST->exprNotation = "bool";
                }
            }
        }
        else if(strcmp(nodeAST->type, "Return") == 0){
                checkReturn(last, funcName, type, nodeAST);
        }
        else if(strcmp(nodeAST->type, "Plus") == 0 || strcmp(nodeAST->type, "Minus") == 0){
            if(strcmp(type, "bool") == 0){
                printf("Line %d, column %d: Operator %s cannot be applied to type %s\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, getOperator(nodeAST->type), type);
                erro_semantica = 1;
                nodeAST->exprNotation = "undef";
            }
            else
                nodeAST->exprNotation = type;

        }

        else if(strcmp(nodeAST->type, "Print")!=0)
            nodeAST->exprNotation = type;          
    }
    else{
        if(strcmp(nodeAST->type, "Return") == 0 ){
            printf("Line %d, column %d: Incompatible type %s in %s statement\n", nodeAST->child->infoT->num_linha, nodeAST->child->infoT->num_coluna, type, getOperator(nodeAST->type));
            erro_semantica = 1;
        }
        else if(strcmp(nodeAST->type, "Print")==0 && type != NULL){
            printf("Line %d, column %d: Incompatible type %s in %s statement\n", nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, type, getOperator(nodeAST->type));
            erro_semantica = 1;
        } 
        else if((strcmp(nodeAST->type, "If") == 0 || strcmp(nodeAST->type, "For") == 0) && type != NULL){
            printf("Line %d, column %d: Incompatible type %s in %s statement\n", nodeAST->child->infoT->num_linha, nodeAST->child->infoT->num_coluna, type, getOperator(nodeAST->type));
            erro_semantica = 1;
        }
        else if(strcmp(nodeAST->type, "Print")!=0 && strcmp(nodeAST->type, "If") != 0 && strcmp(nodeAST->type, "For") != 0){
            printf("Line %d, column %d: Operator %s cannot be applied to type %s\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, getOperator(nodeAST->type), type);
            erro_semantica = 1;
            if(!logical)
                nodeAST->exprNotation = "undef";
            else
                nodeAST->exprNotation = "bool";
        }
    }    
}

void checkReturn(elementST* last, char*funcName, char* type, nodeAst* nodeAST){
    while(last != NULL){
        if(strcmp(last->name, funcName)==0){
            if(strcmp(last->type, type) != 0) {
                erro_semantica = 1;
                if(nodeAST->child != NULL)
                    printf("Line %d, column %d: Incompatible type %s in return statement\n",nodeAST->child->infoT->num_linha, nodeAST->child->infoT->num_coluna, type);
                else
                    printf("Line %d, column %d: Incompatible type %s in return statement\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna+6, type);
            }
        }
        last = last ->nextElem;
    }

}

bool isNewValue(nodeAst* nodeAST){
    if(strcmp("RealLit", nodeAST->type)==0){
        nodeAST->exprNotation = "float32";
        return true;
    }
    else if(strcmp("IntLit", nodeAST->type)==0){
        nodeAST->exprNotation = "int";
        return true;   
    }
    return false;
    
}

void checkTwoMemberOperation(nodeAst* nodeAST, elementST* last, char* funcName, bool logical){                    
    char* type1;
    if(strcmp(nodeAST->child->type, "Id") == 0){
        type1 = isDeclared(nodeAST->child, last, funcName);
        nodeAST->child->exprNotation = type1;
        if(strcmp(type1,"undef") == 0){
            erro_semantica = 1;
            printf("Line %d, column %d: Cannot find symbol %s\n", nodeAST->child->infoT->num_linha,nodeAST->child->infoT->num_coluna,nodeAST->child->infoT->value);
        }
    }
    else 
        type1 = nodeAST->child->exprNotation;
    char* type2;
    if(strcmp(nodeAST->child->sibling->type, "Id") == 0){
        type2 = isDeclared(nodeAST->child->sibling, last, funcName);
        nodeAST->child->sibling->exprNotation = type2;
        if(strcmp(type2, "undef") == 0){
            erro_semantica = 1;
            printf("Line %d, column %d: Cannot find symbol %s\n", nodeAST->child->sibling->infoT->num_linha,nodeAST->child->sibling->infoT->num_coluna,nodeAST->child->sibling->infoT->value);
        }
    }
    else
        type2 = nodeAST->child->sibling->exprNotation;

    if(type1 != NULL && type2 != NULL && strcmp(type1, type2)==0){
        if(strcmp(type1, "undef") == 0 && strcmp(type2, "undef") == 0){
            erro_semantica = 1;
            printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, getOperator(nodeAST->type), type1, type2);
             if(logical)
                nodeAST->exprNotation = "bool";
            else
                nodeAST->exprNotation = "undef";
        }
        else{
            if(logical){
                if(strcmp(nodeAST->type,"And")==0 || strcmp(nodeAST->type,"Or")==0){
                    if(strcmp(type1, "bool")!=0) {
                        erro_semantica = 1;
                        printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, getOperator(nodeAST->type), type1, type2);
                    }
                }
                else if(strcmp(nodeAST->type,"Ge")==0 || strcmp(nodeAST->type,"Le")==0 || strcmp(nodeAST->type,"Gt")==0|| strcmp(nodeAST->type,"Lt")==0){
                    if(strcmp(type1, "bool")==0) {
                        erro_semantica = 1;
                        printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, getOperator(nodeAST->type), type1, type2);
                        
                    }
                }
                nodeAST->exprNotation = "bool";}
            else{
                if(strcmp(nodeAST->type,"Assign") == 0)
                    nodeAST->exprNotation = type1;
                else if(strcmp(nodeAST->type,"ParseArgs") == 0){
                    if(strcmp(type1, "int") == 0 )
                        nodeAST->exprNotation = type1;
                    else
                        printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, getOperator(nodeAST->type), type1, type2);
                }
                else{
                    if(strcmp(type1, "bool")!=0) {
                        nodeAST->exprNotation = type1;
                    }
                    else {
                        printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, getOperator(nodeAST->type), type1, type2);
                        nodeAST->exprNotation = "undef";
                    }
                }
            }
        }
    }
    else{
        if(type1 == NULL)
            type1 = "none";
        if(type2 == NULL)
            type2 = "none";
        erro_semantica = 1;
        printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",nodeAST->infoT->num_linha, nodeAST->infoT->num_coluna, getOperator(nodeAST->type), type1, type2);
        if(logical)
            nodeAST->exprNotation = "bool";
        else
            nodeAST->exprNotation = "undef";
    }
    
}


char* isDeclared(nodeAst* nodeAST, elementST* last, char* funcName){
    char* type = "undef";
    while(last != NULL){
        if(strcmp(last->name, nodeAST->infoT->value)==0 && last->isVar == 1){ 
            type = last->type;
            last->alreadyUsed = 1;
        }
        else if(strcmp(last->name, funcName) == 0){
            elementST* temp = last->body;
            while(temp != NULL ){
                if(temp->line < nodeAST->infoT->num_linha){
                        if(strcmp(temp->name, nodeAST->infoT->value)==0){
                            type = temp->type;
                            temp->alreadyUsed = 1;
                            return type;            
                        }
                         
                    }
                else if(temp->line == nodeAST->infoT->num_linha && temp->col < nodeAST->infoT->num_coluna){  
                    if(strcmp(temp->name, nodeAST->infoT->value)==0){
                            type = temp->type;
                            temp->alreadyUsed = 1;
                            return type;            
                        }
                    
                }
               temp = temp->body;  
            } 
        }
        if(last != NULL)
            last = last->nextElem;
    }
    return type;
}

bool isOneMemberOperationNonLogical(char* type){
    if(strcmp(type, "Minus") == 0)
        return true;
    else if(strcmp(type, "Plus") == 0)
        return true;
    else if(strcmp(type, "Print") == 0)
        return true;
    else if(strcmp(type, "Return") == 0)
        return true;
    else 
        return false;
}

bool isOneMemberOperationLogical(char* type){
    if(strcmp(type, "Not") == 0)
        return true;
    if(strcmp(type, "If") == 0)
        return true;
    else if(strcmp(type, "For") == 0 )
        return true;
    else if(strcmp(type, "While") == 0)
        return true;
    else
        return false;
}


bool isTwoMemberOperation(char* type){
    if(strcmp(type, "Add") == 0)
        return true;
    else if(strcmp(type, "Sub") == 0)
        return true;
    else if(strcmp(type, "Mul") == 0)
        return true;
    else if(strcmp(type, "Div") == 0)
        return true;
    else if(strcmp(type, "Mod") == 0)
        return true;
    else if(strcmp(type, "Assign")==0)
        return true;
    else if(strcmp(type, "ParseArgs")==0)
        return true;
    else 
        return false;
}


char* getOperator(char* string){
    if(strcmp(string, "Assign")==0)
        return "=";
    else if(strcmp(string, "Add")==0)
        return "+";
    else if(strcmp(string, "Sub")==0)
        return "-";
    else if(strcmp(string, "Mul")==0)
        return "*";
    else if(strcmp(string, "Div")==0)
        return "/";
    else if(strcmp(string, "Mod")==0)
        return "%";
    else if(strcmp(string, "And")==0)
        return "&&";
    else if(strcmp(string, "Or")==0)
        return "||";
    else if(strcmp(string, "Ne")==0)
        return "!=";
    else if(strcmp(string, "Lt")==0)
        return "<";
    else if(strcmp(string, "Gt")==0)
        return ">";
    else if(strcmp(string, "Ge")==0)
        return ">=";
    else if(strcmp(string, "Le")==0)
        return "<=";
    else if(strcmp(string, "Return")==0)
        return "return";
    else if(strcmp(string, "For")==0)
        return "for";
    else if(strcmp(string, "If")==0)
        return "if";
    else if(strcmp(string, "ParseArgs") == 0)
        return "strconv.Atoi";
    else if(strcmp(string, "Not") == 0)
        return "!";
    else if(strcmp(string, "Print") == 0)
        return "fmt.Println";
    else if(strcmp(string, "Plus") == 0)
        return "+";
    else if(strcmp(string, "Minus") == 0)
        return "-";
    else if(strcmp(string, "Eq") == 0)
        return "==";

    else return NULL;    
}

bool isLogicalOperator(char* type){
    if(strcmp(type, "Or") == 0)
        return true;
    else if(strcmp(type, "Eq") == 0)
        return true;
    else if(strcmp(type, "Ne") == 0)
        return true;
    else if(strcmp(type, "Lt") == 0)
        return true;
    else if(strcmp(type, "Gt") == 0)
        return true;
    else if(strcmp(type, "Le") == 0)
        return true;
    else if(strcmp(type, "Ge") == 0)
        return true;
    else if(strcmp(type, "And") == 0)
        return true;
    else 
        return false;
}


bool isStatement(char* type){
    if(strcmp(type, "If") == 0)
        return true;
    else if(strcmp(type, "For") == 0 )
        return true;
    else
        return false;
}


char* myStrCat(char* s1, char* s2, bool free1, bool free2){
    static char* temps1;
    static char* temps2;
    temps1 = (char*)malloc((strlen(s1) + strlen(s2)) * sizeof(char));
    temps2 = (char*)malloc(strlen(s2) * sizeof(char));
    temps1 = strdup(s1);
    temps2 = strdup(s2);
    strcat(temps1, temps2);
    if (free1)
        free(s1);
    //free(temps2);
    if(free2)
        free(s2);
    return temps1;
}

char* myToLowerCase(char* s1){
    static char* temps1;
    temps1 = (char*)malloc(strlen(s1) * sizeof(char));
    strcpy(temps1, s1);
    int i=0;
    while(temps1[i]){
        temps1[i] = tolower(temps1[i]);
        i++;
    }
    return temps1;
}


void printSymbolTable(elementST* elem) {
    elementST* copy = elem;

    // Tabela Global
    printf("===== Global Symbol Table =====\n");
    while (elem != NULL) {
        if (elem->isVar == 1)
            printf("%s\t\t%s\n", elem->name, elem->type);
        else if (elem->isVar == 0)
            printf("%s\t(%s)\t%s\n", elem->name, elem->paramTypes, elem->type);
        elem = elem->nextElem;
    }
    
    // Tabelas de funcoes
    while (copy != NULL) {
        if (copy->body != NULL) {
            if (strcmp(copy->paramTypes, "") == 0)
                printf("\n===== Function %s() Symbol Table =====\n", copy->name);
            else
                printf("\n===== Function %s(%s) Symbol Table =====\n", copy->name, copy->paramTypes);
            
            elementST* func = copy->body;
            while (func != NULL) {
                if (func->isVar == 1 || func->isVar == 2) {
                    if (strcmp(func->param, "") == 0)
                        printf("%s\t\t%s\n", func->name, func->type);
                    else
                        printf("%s\t\t%s\t%s\n", func->name, func->type, func->param);
                }
                else if (func->isVar == 0)
                    printf("%s\t(%s)\t%s\t%s\n", func->name, func->paramTypes, func->type, func->param);
                func = func->body;
            }
        }
        copy = copy->nextElem;
    }
    printf("\n");
}

void checkUnusedVariables(elementST* elem){
    while (elem != NULL) {
        if (elem->body != NULL) {
            elementST* copy = elem;
            while (copy != NULL) {        
                if (copy->isVar == 1 && copy->alreadyUsed == 0){
                    printf("Line %d, column %d: Symbol %s declared but never used\n", copy->line, copy->col, copy->name);
                    erro_semantica = 1;
                }
                copy = copy->body;
            }
        }

        elem = elem->nextElem;
    }
}


void freeST(elementST *elem){
    /*if (elem->type != NULL && strlen(elem->type) != 0 && strcmp(elem->type, "none") != 0)
        free(elem->type);
    if (elem->paramTypes != NULL && strlen(elem->paramTypes) != 0)
        free(elem->paramTypes);*/
    if (elem->nextElem != NULL)    
        freeST(elem->nextElem);
    if (elem->body != NULL)  
        freeST(elem->body);

    free(elem);
}