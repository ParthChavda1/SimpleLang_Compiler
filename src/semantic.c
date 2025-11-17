#include "semantic.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

 



Scope *createScope(Scope *parent){
    Scope *s = malloc(sizeof(Scope));
    if(s==NULL){
        printf("Error In Momory Allocation.\n");
        exit(1);
    }
    s->parent = parent;
    s->symbols = NULL;
    return s;
}


void freeScope(Scope *scope){
    if(!scope) return;
    Symbol *s = scope->symbols;
    while(s){
        Symbol *nxt = s->next;
        if(s->name) free(s->name);
        free(s);
        s = nxt; 
    }
    free(scope);
}

Symbol* symbolLookup(Scope *scope, char *name){
    while(scope){
        Symbol *s = scope->symbols;
        while(s){
            if(strcmp(s->name,name) == 0) return s;
            s = s->next;  
        }
        scope = scope->parent;
    }
    return NULL;
}

bool addSymbol(Scope *scope, char *name, ValueType type){
    if(!scope || !name){
        printf("Proper Name and scope not provided\n");
        return false;
    }
    if(symbolLookup(scope,name) != NULL) return false;
    Symbol *s = malloc(sizeof(Symbol));
    if(s==NULL) {
        printf("Memory Allocation Failed\n");
        exit(1);
    }
    // strdup(s->name,name);
    s->name = strdup(name);
    s->type = type;
    s->next = scope->symbols;
    scope->symbols = s;
    return true;
}

ParserNode *getChild(ParserNode *node,int idx){
    if(!node) return NULL;
    if(idx<0 || idx>=node->child_count) return NULL;
    return node->children[idx];
}

// expression -> Term {('+' || '-')  Term }
ValueType checkExpression(Scope *scope, ParserNode *exp){
    if(!exp || exp->type != NODE_EXPRESSION){
        printf("Expeced Expression Node\n");
    }
    if(exp->child_count < 1){
        printf("Expression Node has no Children\n");
    }
    ParserNode *lTerm = getChild(exp,0);
    if(!lTerm) {
        printf("Missing First Term is Expression\n");
        exit(1);
    }
    ValueType left = checkTerm(scope,lTerm);
    if(exp->child_count == 1) return left;
    if(exp->child_count %2 == 0){
        printf("Error is Expression\n");
        exit(1);
    }
    for(int i=1;i<exp->child_count; i+=2){
        ParserNode *op = getChild(exp,i);
        ParserNode *rTerm = getChild(exp,i+1);
        ValueType right = checkTerm(scope, rTerm);
        if(op->token.type == TOKEN_ADD || op->token.type ==TOKEN_SUB){
            if(left!= TYPE_INT || right != TYPE_INT){
                printf("Semantic Error: Operator + and - require in operands\n");
                exit(1);
            }
        }
        else{
            printf("Semantic Error: Unknown operator in Expression\n");
            exit(1);
        } 
        
    }
    return left;
}

// Condition-> Expression Comparator Expression 
ValueType checkCondition(Scope *scope, ParserNode *cond){
    if(!cond || cond->type != NODE_CONDITION ){
        printf("Expected Condition Node\n");
        exit(1);
    }
    if(cond->child_count == 1){
        ParserNode *firstOnly = getChild(cond,0);
        ValueType t = checkExpression(scope,firstOnly);
        if(t != TYPE_BOOL || (t== TYPE_INT && strcmp(firstOnly->token.value,"0") == 0)){
                return TYPE_BOOL; 
        }
    }
    if(cond->child_count == 2 || cond->child_count >3){
        printf("Condition Node must have atleast 3 Children\n");
        exit(1);
    }

    ParserNode *lNode = getChild(cond,0);
    ParserNode *op = getChild(cond,1);
    ParserNode *rNode = getChild(cond,2);

    ValueType left = checkExpression(scope,lNode);
    ValueType right = checkExpression(scope,rNode);

    if(op->token.type != TOKEN_EQ || op->token.type != TOKEN_GT || op->token.type != TOKEN_GTE || op->token.type != TOKEN_LT || op->token.type != TOKEN_LTE || op->token.type != TOKEN_NEQ ){
        printf("Semantic Error: Not a valid operator\n");
    }
    if(left != right){
        printf("Condition requires Both expression to have same type\n");
        exit(1);
    }
    return TYPE_BOOL;
}

// Term -> Identifier | Number | '(' Expression ')'
ValueType checkTerm(Scope *scope, ParserNode *term){
     if(!term){
        printf("Term Node is Expected");
        exit(1);
     }
     switch(term->type) {
        case NODE_IDENTIFIER:
            Symbol *s = symbolLookup(scope,term->token.value);
            if(!s){
                printf("%s not declared",term->token.value);
                exit(1);
            }
            return s->type;
        case NODE_NUMBER:
            return TYPE_INT;
        case NODE_TERM:
            if(term->children[0]->type != NODE_EXPRESSION){
                ParserNode *trm = getChild(term,0);
                return checkTerm(scope,trm);
            }
            else{
                // ParserNode *open = getChild(term,0);
                ParserNode *exp = getChild(term,0);
                // ParserNode *close = getChild(term,2);
                if(!exp){
                    printf("Invalid Parenthesis Term: Expression Missing");
                    exit(1);
                }
                return checkExpression(scope,exp);
            }
            // else{
            //     printf("Semantic Error: Invalid Term Node shape");
            //     return TYPE_UNKNOWN;
            // }
        default:
            printf("Invalid Node inside Term");
            return TYPE_UNKNOWN;
        }

}

// statement -> Ifstatmemt | Declaration | Assignment
void checkStatement(Scope *scope, ParserNode *stmt){
    if(!stmt){
        printf("Null Statement Node\n");
        exit(1);
    }
    ParserNode *type = stmt->children[0];
    switch(type->type){
        case NODE_DECLARATION:
            if(type->child_count != 1){
                printf("Declaration must have Identifier child\n");
                exit(1);
            }
            ParserNode *id = getChild(type,0);
            if(!id || id->type != NODE_IDENTIFIER){
                printf("Declaration missing identifier\n");
                exit(1);
            }
            char *name = id->token.value;
            if(!addSymbol(scope,name,TYPE_INT)){
                printf("Identifier Already Exist\n");
                exit(1);
            }
            break;
        case NODE_ASSIGNMENT:
            if(type->child_count < 2 ){
                printf("Assignment node has too few children.\n");
                exit(1);
            }
            id = getChild(type,0);
            ParserNode *exp = getChild(type,1);
            if(!id || id->type != NODE_IDENTIFIER){
                printf("Assignment Missing Identifie\nr");
            }
            if(!exp){
                printf("Assgnment missing RHS Expression\n");
            }
            Symbol *s = symbolLookup(scope,id->token.value);
            if(!s){
                printf("Undeclared Variable\n");
            }
            ValueType rhs = checkExpression(scope,exp);
            if(rhs != TYPE_INT){
                printf("Assignment Type mismatch\n");
                exit(1);
            }
            break;
        case NODE_IF:
            if(type->child_count <2){
                printf("If node Malformed\n");
                exit(1);
            }
            ParserNode *cond = getChild(type,0);
            if(!cond || cond->type != NODE_CONDITION){
                printf("Condition Node Expected\n");
                exit(1);
            }
            ValueType cType = checkCondition(scope,cond);
            if(cType != TYPE_BOOL){
                printf("If condition should be of type Boolean");
            } 
            Scope *inner = createScope(scope);
            ParserNode *body;
            for(int i=1;i<type->child_count;i++){
                body = getChild(type,i);
                if(!body || body->type != NODE_BODY){
                    printf("Body Node Expected\n");
                    exit(1);
                }
                checkStatement(inner,body);

            }
                freeScope(inner);
                break;
        default:
            printf("Unknown Statement Type\n");
            exit(1);
            break;
    }
}

void checkSemantic(ParserNode *root){
    if(!root || root->type != NODE_ROOT ){
        printf("Root Node Expected\n");
        exit(1);
    }
    Scope *global = createScope(NULL);
    for(int i=0; i<root->child_count;i++){
        ParserNode *child = getChild(root,i);
        if(child){
            checkStatement(global,child);
        }
    }
    freeScope(global);
}
