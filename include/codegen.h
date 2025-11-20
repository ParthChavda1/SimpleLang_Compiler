#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include "semantic.h"
#include <stdio.h>
#include<stdlib.h>

typedef struct {
    FILE *output;
    int label_id;
}CodeGen;

// Functions for generating Code according to node
void generateCode(const char *filepath,ParserNode *root,Symbol *s);

void genStatement(CodeGen *cg,ParserNode *stmt);
void genAssignment(CodeGen *cg,ParserNode *node);
void genIf(CodeGen *cg,ParserNode *nodeIf);
void genExpression(CodeGen *cg,ParserNode *exp);
int genCondition(CodeGen *cg,ParserNode *cond,char **jump1,char **jump2);

// Functions for creating and managing labels
int newLabel(CodeGen *cg);

// functions for writing in file
void emitLabel(CodeGen *cg, const char *fmt,...);
void emitText(CodeGen *cg, const char *fmt,...);
void emitData(CodeGen *cg, const char *fmt,...);



#endif