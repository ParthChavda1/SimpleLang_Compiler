#ifndef SEMENTIC_H
#define SEMENTIC_H

#include "parser.h"
#include<stdbool.h>

extern int semantic_err_count;
typedef enum{
    TYPE_INT,
    TYPE_BOOL,
    TYPE_UNKNOWN,
}ValueType;

typedef struct Symbol{
    char *name ;
    ValueType type;
    struct Symbol *next;
}Symbol;


typedef struct Scope{
    Symbol *symbols;
    struct Scope *parent;
}Scope;

Scope *createScope(Scope *parent);
void freeScope(Scope *scope);
void freeSymbolTable(Symbol *s);
bool addSymbol(Scope *scope, char *name, ValueType type);
Symbol *symbolLookup(Scope *scope, char *name);

ValueType checkExpression(Scope *scope, ParserNode *exp);
ValueType checkCondition(Scope *scope, ParserNode *cond);
ValueType checkExpression(Scope *scope, ParserNode *exp);
void checkStatement(Scope *scope, ParserNode *stmt);
ValueType checkTerm(Scope *scope, ParserNode *term);


Symbol *checkSemantic(ParserNode *root);






#endif