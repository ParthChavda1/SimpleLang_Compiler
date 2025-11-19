#include "semantic.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
 
Symbol *sym_table;


int semantic_err_count = 0;
void semanticError(const char *err,...){
    printf("Semantic Error:");
    va_list args;
    va_start(args,err);
    vprintf(err,args);
    printf("\n");
    semantic_err_count++;
}

Symbol *addToSymbolTable(Scope *s){
    Symbol *ss = s->symbols;
    while(ss){
        Symbol *sym = malloc(sizeof(Symbol));
        sym->name = strdup(ss->name);
        sym->type = ss->type;
        if(sym_table ==NULL){
            sym->next = NULL;
        }
        else{
            sym->next = sym_table;
        }
        sym_table = sym;
        ss = ss->next;
    }
    return sym_table;
}

void freeSymbolTable(Symbol *s){
    while (s){
        Symbol *next = s->next;
        free(s);
        s= next; 
    }
}

Scope *createScope(Scope *parent){
    Scope *s = malloc(sizeof(Scope));
    if(s==NULL){
        semanticError("Memory Allocation Failed for Scope");
        exit(1);
    }
    s->parent = parent;
    s->symbols = NULL;
    return s;
}


void freeScope(Scope *scope){
    if(!scope) return;
    Symbol *s = scope->symbols;
    freeSymbolTable(s);
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
        semanticError("Proper Name and scope not provided.");
        return false;
    }
    if(symbolLookup(scope,name) != NULL){
        semanticError("Redeclaration of %s variable.",name);
        return false;
    } 
    Symbol *s = malloc(sizeof(Symbol));
    if(s==NULL) {
        semanticError("Memory Allocation Failed for Symbol");
        return false;
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
        semanticError("Expeced Expression Node.");
        return TYPE_UNKNOWN;
    }
    if(exp->child_count < 1){
        semanticError("Expression Node has no Children.");
        return TYPE_UNKNOWN;
    }
    ParserNode *lTerm = getChild(exp,0);
    ValueType left = checkTerm(scope,lTerm);
    if(exp->child_count == 1) return left;
    if(exp->child_count %2 == 0){
        semanticError("Error is Expression.");
        return TYPE_UNKNOWN;
    }
    for(int i=1;i<exp->child_count; i+=2){
        ParserNode *op = getChild(exp,i);
        ParserNode *rTerm = getChild(exp,i+1);
        ValueType right = checkTerm(scope, rTerm);
        if(op->token.type == TOKEN_ADD || op->token.type ==TOKEN_SUB){
            if(left!= TYPE_INT || right != TYPE_INT){
                semanticError("Operator + and - require in operands.");
                return TYPE_UNKNOWN;
            }
        }
        else{
            semanticError("Unknown operator in Expression.");
            return TYPE_UNKNOWN;
        } 
        
    }
    return left;
}

// Condition-> Expression Comparator Expression 
ValueType checkCondition(Scope *scope, ParserNode *cond){
    if(!cond || cond->type != NODE_CONDITION ){
        semanticError("Expected Condition Node.");
        return TYPE_UNKNOWN;
    }
    if(cond->child_count == 1){
        ParserNode *firstOnly = getChild(cond,0);
        ValueType t = checkExpression(scope,firstOnly);
        if(t != TYPE_BOOL || (t== TYPE_INT && strcmp(firstOnly->token.value,"0") == 0)){
                return TYPE_BOOL; 
        }
    }
    if(cond->child_count == 2 || cond->child_count >3){
        semanticError("Condition Node must have atleast 3 Children.");
        return TYPE_UNKNOWN;
    }

    ParserNode *lNode = getChild(cond,0);
    ParserNode *op = getChild(cond,1);
    ParserNode *rNode = getChild(cond,2);

    ValueType left = checkExpression(scope,lNode);
    ValueType right = checkExpression(scope,rNode);

    if(op->token.type != TOKEN_EQ && op->token.type != TOKEN_NEQ &&
        op->token.type != TOKEN_GTE && op->token.type != TOKEN_GT &&
         op->token.type != TOKEN_LTE && op->token.type != TOKEN_LT ){
        semanticError("%s is not a valid operator",op->token.value);
        return TYPE_UNKNOWN;
    }
    if(left != right){
        semanticError("Condition requires Both expression to have same type.");
        return TYPE_UNKNOWN;
    }
    return TYPE_BOOL;
}

// Term -> Identifier | Number | '(' Expression ')'
ValueType checkTerm(Scope *scope, ParserNode *term){
     if(!term){
        semanticError("Term Node is Expected.");
        return TYPE_UNKNOWN;
     }
     switch(term->type) {
        case NODE_IDENTIFIER:
            Symbol *s = symbolLookup(scope,term->token.value);
            if(!s){
                semanticError("%s variable is not declared.",term->token.value);
                return TYPE_UNKNOWN;
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
                ParserNode *exp = getChild(term,0);
                if(!exp){
                    semanticError("Invalid Parenthesis Term: Expression Missing.");
                    return TYPE_UNKNOWN;
                }
                return checkExpression(scope,exp);
            }

        default:
            semanticError("Invalid Node inside Term.");
            return TYPE_UNKNOWN;
        }

}

// statement -> Ifstatmemt | Declaration | Assignment
void checkStatement(Scope *scope, ParserNode *stmt){
    if(!stmt){
        semanticError("Null Statement Node.");
        return ;
    }
    ParserNode *type = stmt->children[0];
    switch(type->type){
        case NODE_DECLARATION:
            ParserNode *id = getChild(type,0);
            if(!id || id->type != NODE_IDENTIFIER){
                semanticError("Declaration is missing identifier.");
                return ;
            }
            char *name = id->token.value;
            if(!addSymbol(scope,name,TYPE_INT)){
                semanticError("Identifier Already Exist.");
                return ;
            }
            break;
        case NODE_ASSIGNMENT:
            if(type->child_count < 2 ){
                semanticError("Assignment node has too few children.");
                return ;
            }
            id = getChild(type,0);
            ParserNode *exp = getChild(type,1);
            if(!id || id->type != NODE_IDENTIFIER){
                semanticError("Assignment Missing Identifier.");
                return ;
            }
            if(!exp){
                semanticError("Assgnment missing RHS Expression.");
                return ;
            }
            Symbol *s = symbolLookup(scope,id->token.value);
            if(!s){
                semanticError("Assgnment to undeclared variable %s.",id->token.value);
                return ;
            }
            ValueType rhs = checkExpression(scope,exp);
            if(rhs != TYPE_INT){
                semanticError("Type mismatch in assignment to %s.",id->token.value);
                return ;
            }
            break;
        case NODE_IF:
            if(type->child_count <2){
                semanticError("If node Malformed.");
                return ;
            }
            ParserNode *cond = getChild(type,0);
            if(!cond || cond->type != NODE_CONDITION){
                semanticError("If statement missing condition.");
                return ;
                
            }
            ValueType cType = checkCondition(scope,cond);
            if(cType != TYPE_BOOL){
                semanticError("If condition must be boolean.");
            } 
            Scope *inner = createScope(scope);
            ParserNode *body;
            for(int i=1;i<type->child_count;i++){
                body = getChild(type,i);
                checkStatement(inner,body);
                
            }
            addToSymbolTable(inner);
            freeScope(inner);
            break;
        default:
            semanticError("Unknown Statement Type.");
            break;
        }
    }
    
    Symbol *checkSemantic(ParserNode *root){
        if(!root || root->type != NODE_ROOT ){
            semanticError("Root Node Expected.");
            return NULL;
        }
    
    Scope *global = createScope(NULL);
    for(int i=0; i<root->child_count;i++){
        ParserNode *child = getChild(root,i);
        if(child){
            checkStatement(global,child);
        }
    }
    if(semantic_err_count>0){
        printf("Total Semantic Errors: %d\n",semantic_err_count);
    }
    addToSymbolTable(global);
    freeScope(global);
    return sym_table;

}
