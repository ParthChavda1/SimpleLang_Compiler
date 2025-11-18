#include "codegen.h"

#include<stdarg.h>


int newLabel(CodeGen *cg){
    return cg->label_id++;
}

void emitLabel(CodeGen *cg, const char *fmt,...){
    va_list args;
    va_start(args,fmt);
    vfprintf(cg->output,fmt,args);
    fprintf(cg->output,":\n");
    va_end(args);
}

void emitText(CodeGen *cg, const char *fmt,...){
    va_list args;
    va_start(args,fmt);
    fprintf(cg->output,"\t");
    vfprintf(cg->output,fmt,args);
    fprintf(cg->output,"\n");
    va_end(args);
}

void emitData(CodeGen *cg, const char *fmt,...){
    va_list args;
    va_start(args,fmt);
    vfprintf(cg->output,fmt,args);
    fprintf(cg->output,"\n");
    va_end(args);
}


void generateCode(const char *filepath,ParserNode *root,Scope *s){
    CodeGen cg;
    cg.output = fopen(filepath,"w");
    cg.label_id = 0;
    if(!cg.output){
        printf("CodeGen Error: Cannot open %s \n",filepath);
        return;
    }

    fprintf(cg.output,".text\n\n");
    emitLabel(&cg,"start");
    genStatement(&cg,root);
    emitText(&cg,"hlt");

    fprintf(cg.output,"\n\n.data\n\n");
    // while(s){
        Symbol *symbol = s->symbols;
        while(symbol){
            
            emitData(&cg,"%s = 0",symbol->name);
            symbol = symbol->next;
        }
        // s = s->parent; 
    // }
// 
    fclose(cg.output);
}

void genStatement(CodeGen *cg,ParserNode *stmt){
    if(!stmt) return;
    switch(stmt->type){
        case NODE_ROOT:
        case NODE_BODY:
            for(int i=0 ; i< stmt->child_count; i++){
                genStatement(cg,stmt->children[i]);
            }
            break;
        case NODE_ASSIGNMENT:
            genAssignment(cg,stmt);
            break;
        case NODE_IF:
            genIf(cg,stmt);
            break;
        case NODE_DECLARATION:
            break;
        default:
            break;
    }
}

// a = expression
// TREE
// node->children[0] = identifier
// node->children[1] = Expression
void genAssignment(CodeGen *cg,ParserNode *node){
    const char *name = node->children[0]->token.value;
    genExpression(cg,node->children[1]);
    emitText(cg,"sta %%%s",name);

}

//Expression ->Number or Identifier or Expression
void genExpression(CodeGen *cg,ParserNode *exp){
    switch(exp->type){
        case NODE_EXPRESSION:
            if(exp->child_count == 1){
                genExpression(cg,exp->children[0]);
                return;
            }
            if(exp->child_count > 3){
                ParserNode *left = exp->children[0];
                genExpression(cg,left);
                emitText(cg,"push A");
                for(int i=1;i<exp->child_count;i+=2){
                    ParserNode *op = exp->children[i];
                    ParserNode *right = exp->children[i+1];
                    genExpression(cg,right);
                    emitText(cg,"pop B");
                    if(op->token.type == TOKEN_ADD){
                        emitText(cg,"add");
                    }
                    else if(op->token.type ==TOKEN_SUB){
                        emitText(cg,"sub");
                    }
                    else{
                        printf("Unsupported Operator\n");
                    }
                    emitText(cg,"push A");
                }
                return;
            }
            return;
        case NODE_NUMBER:
            emitText(cg,"ldi A %s",exp->token.value);
            return;
        case NODE_IDENTIFIER:
            emitText(cg,"lda %%%s",exp->token.value);
            return;
        default:
            printf("Invalid Expression Node");
            exit(1);
    }

}


void genIf(CodeGen *cg,ParserNode *nodeIf){
    ParserNode *cond = nodeIf->children[0];     
    char *jump1, *jump2;
    int isTwo = genCondition(cg,cond,&jump1,&jump2);
    int lTrue = newLabel(cg);
    int lFalse = newLabel(cg);
    emitText(cg,"%s %%L%d",jump1,lTrue);
    if(isTwo){
        emitText(cg,"%s %%L%d",jump2,lTrue);
    }
    emitText(cg,"jmp %%L%d",lFalse);
    emitLabel(cg,"L%d",lTrue);
    for(int i=1;i<nodeIf->child_count;i++){
        genStatement(cg,nodeIf->children[i]);
    }
    emitLabel(cg,"L%d",lFalse);

}

int genCondition(CodeGen *cg,ParserNode *cond,char **jump1,char **jump2){
    
    if(cond->child_count ==1){

        // if(cond->children[0]->token.value != '0'){
        //     *jump1 = "jmp"; 
        //     *jump2 = NULL; 
        // }
        *jump1 = NULL; 
        *jump2 = NULL; 
        return 0;
    }
    if(cond->child_count == 3)
    {
        ParserNode *left = cond->children[0]; 
        ParserNode *op = cond->children[1]; 
        ParserNode *right = cond->children[2];

        genExpression(cg,left);
        emitText(cg,"push A");

        genExpression(cg,right);
        emitText(cg, "pop B");
        
        emitText(cg, "cmp");
        switch(op->token.type){
            case TOKEN_EQ:  // ==
                *jump1 = "jz";
                *jump2 = NULL;
                return 0;
            case TOKEN_NEQ: //!=
                *jump1 = "jnz";
                *jump2 = NULL;
                return 0;
            case TOKEN_LT: //<
                *jump1 = "jc";
                *jump2 = NULL;
                return 0;
            case TOKEN_GTE: //>=
                *jump1 = "jnc";
                *jump2 = NULL;
                return 0;
            
            case TOKEN_LTE: //<=
                // CF = 1(<) or ZF = 1 (=)
                *jump1 = "jz";
                *jump2 = "jc";
                return 1;
            
            case TOKEN_GT: //>
                *jump1 = "jnc";
                *jump2 = "jnz";
                return 1;
            default:
                printf("Unknown Comparison Operator \n");
                exit(1);
        }
    }
    return 0;
}



