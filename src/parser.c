#include "parser.h"
#include<stdlib.h>

Parser *createParser(Token *tokens, int total_count){
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    if(parser == NULL){
        printf("Memory Allocation failed\n");
        exit(1);
    }
    parser->tokens = tokens;
    parser->current = 0;
    parser->total_count = total_count;
    return parser;
}

ParserNode *createNode(NodeType type, Token token){
    ParserNode *node = malloc(sizeof(ParserNode));
    if(node == NULL){
        printf("Memory Allocation failed\n");
        exit(1);
    }
    node->type = type;
    node->token = token;
    node->child_count = 0;
    node->child_capacity = 2;
    node->children = malloc(sizeof(ParserNode *)*node->child_capacity);
    if(node->children == NULL){
        printf("Memory Allocation failed\n");
        exit(1);
    }
    return node;
}

void addChild(ParserNode *parent, ParserNode *child){
    if(parent->child_count >= parent->child_capacity){
        parent->child_capacity *=2;
        ParserNode **new_children = realloc(parent->children,sizeof(ParserNode *)*parent->child_capacity);
        if(new_children ==NULL){
            printf("ERROR: Failed to realloc children array\n");
            exit(1); 
        }
        parent->children = new_children;
    }
    parent->children[parent->child_count++] = child;
}

// Root-> Statement
ParserNode *parseProgram(Parser *parser){
    Token empty = {0};
    ParserNode *root = createNode(NODE_ROOT,empty);
    while(getCurrentToken(parser).type != TOKEN_EOF ){
        ParserNode *stmt = parseStatement(parser);
        addChild(root,stmt);
        // advanceToken(parser);
    }
    return root;
}

// statement -> Ifstatmemt | Declaration | Assignment
ParserNode *parseStatement(Parser *parser){
    Token empty = {0};
    ParserNode *stmt = createNode(NODE_BODY,empty);
    ParserNode *s1;
    Token t = getCurrentToken(parser);
        if(t.type == TOKEN_IF){
            s1 = parseIf(parser);
            addChild(stmt,s1);
        }
        else if(t.type == TOKEN_INT) {
            s1 =  parseDeclaration(parser);
            addChild(stmt,s1);
        }
        else if(t.type == TOKEN_IDENTIFIER){
            s1 = parseAssignment(parser);
            addChild(stmt,s1);
        } 
        // advanceToken(parser);
        // t = getCurrentToken(parser);
    // }
    // else{
        //     printf("Error near Token %s", t.value)
    //     advanceToken(parser);
    //     return NULL;
    // }
    return stmt;
}

// Declaration -> "int" Identifier ";"
ParserNode *parseDeclaration(Parser *parser){
    Token t = getCurrentToken(parser); 
    ParserNode *node = createNode(NODE_DECLARATION,t);
    advanceToken(parser);
    if(!isMatch(TOKEN_IDENTIFIER,parser)){
        printf("Identifier not found in declaration\n");
        return NULL;
    }
    Token i= parser->tokens[parser->current - 1];
    ParserNode *n2 = createNode(NODE_IDENTIFIER,i);
    addChild(node,n2);
    if(!isMatch(TOKEN_SEMICOLON, parser)){
        printf("Semicolon(;) Not present at end of Declaration\n");
        return NULL;
    }
    return node;
}

// Assignment-> Identifier "=" Expression ";"
ParserNode *parseAssignment(Parser *parser){
    Token t = getCurrentToken(parser);
    ParserNode *assignNode = createNode(NODE_ASSIGNMENT,t);
    if(isMatch(TOKEN_IDENTIFIER,parser)){
        ParserNode *id = createNode(NODE_IDENTIFIER,(parser->tokens[(parser->current-1)]));
        addChild(assignNode,id);
    }
    else{
        printf("Identifier is not available in assignment\n");
        return NULL;
    }
    if(!isMatch(TOKEN_ASSIGN,parser)){
        printf("Assignment operator is not present\n");
        return NULL;
    }
    ParserNode *exp = parseExpression(parser);
    addChild(assignNode,exp);
    if(!isMatch(TOKEN_SEMICOLON,parser)){
        printf("Semicolon not present at end of Assignment\n");
        return NULL;
    } 
    return assignNode;
}

// Ifstatement -> 'if' "(" condition ")" "{" statement "}"
ParserNode *parseIf(Parser *parser){
    ParserNode *ifNode = createNode(NODE_IF,(getCurrentToken(parser)));
    advanceToken(parser);
    if(!isMatch(TOKEN_LPAREN, parser)){
        printf("Left parenthesis not present in if statement \n");
        return NULL;
    }
    ParserNode *cond = parseCondition(parser);
    addChild(ifNode,cond);
    if(!isMatch(TOKEN_RPAREN, parser)){
        printf("Right parenthesis not present in if statement \n");
        return NULL;
    }
    if(!isMatch(TOKEN_LBRACE, parser)){
        printf("Left bracket not present in if statement \n");
        return NULL;
    }
    while(getCurrentToken(parser).type != TOKEN_RBRACE && getCurrentToken(parser).type != TOKEN_EOF ){
        ParserNode *stmt = parseStatement(parser);
        addChild(ifNode,stmt);
    }
    Token t1 = getCurrentToken(parser);
    if(!isMatch(TOKEN_RBRACE, parser)){
        printf("%s",t1.value);
        printf("Right bracket not present in if Statement\n");
        return NULL;
    }
    return ifNode;
}

// Condition-> Expression Comparator Expression 
ParserNode *parseCondition(Parser *parser){
    ParserNode *cond = createNode(NODE_CONDITION,getCurrentToken(parser));
    ParserNode *lExp = parseExpression(parser);
    addChild(cond,lExp);
    if(!isComparator(parser)){
        printf("Valid Comparator not present\n");
    }
    Token t = getCurrentToken(parser);
    ParserNode *comp = createNode(NODE_COMPARATOR,t); 
    advanceToken(parser);
    addChild(cond,comp);
    ParserNode *rExp = parseExpression(parser);
    addChild(cond,rExp);
    return cond;
}

// expression -> Term {('+' || '-')  Term }
ParserNode *parseExpression(Parser *parser){
    ParserNode *exp = createNode(NODE_EXPRESSION,getCurrentToken(parser));
    ParserNode *lTerm = parseTerm(parser);
    addChild(exp,lTerm);
    while(1){
        Token t = getCurrentToken(parser);
        if(t.type == TOKEN_ADD || t.type == TOKEN_SUB){
            ParserNode *oper = createNode(NODE_OPERATOR,(getCurrentToken(parser)));
            addChild(exp,oper);
            advanceToken(parser);
            ParserNode *RTerm = parseTerm(parser);
            addChild(exp,RTerm);
        }
        else{
            break;
        }
    }
    return exp;
}

// Term -> Identifier | Number | '(' Expression ')'
ParserNode *parseTerm(Parser *parser){
    Token t= getCurrentToken(parser);
    ParserNode *term;
    if(t.type == TOKEN_IDENTIFIER){
       ParserNode * id = createNode(NODE_IDENTIFIER,t);
       advanceToken(parser); 
       return id; 
    }
    
    if(t.type == TOKEN_NUMBER){
       ParserNode * num = createNode(NODE_NUMBER,t); 
       advanceToken(parser);
       return num; 
    }
    if(isMatch(TOKEN_LPAREN,parser)){
        Token empty = {0};
        term = createNode(NODE_TERM,empty);
        ParserNode *exp = parseExpression(parser);
        addChild(term,exp);
        if(!isMatch(TOKEN_RPAREN,parser)){
            printf("Right Parenthisis not Found\n");
            return NULL;
        }
        return term;
    }
    return NULL;
}






// helper Function

Token getCurrentToken(Parser *parser){
    return parser->tokens[parser->current];
}
Token advanceToken(Parser *parser){
    if (parser->current < parser->total_count){
        parser->current++;
    }
    return(getCurrentToken(parser));
}
bool isMatch(TokenType type, Parser *parser){
    if(getCurrentToken(parser).type == type){
        advanceToken(parser);
        return true;
    }
    return false;
}

bool isComparator(Parser *parser){
    Token t = getCurrentToken(parser);
    if(t.type == TOKEN_EQ || t.type == TOKEN_NEQ || t.type == TOKEN_LT || t.type == TOKEN_LTE || t.type == TOKEN_GT || t.type == TOKEN_GTE ){
        return true;
    }
    return false;
}

char * nodeTypeName(NodeType type){
    switch(type){
        case NODE_ROOT: return "ROOT";
        case NODE_BODY: return "BODY";
        case NODE_DECLARATION: return "DECLARATION";
        case NODE_ASSIGNMENT: return "ASSIGNMENT";
        case NODE_IF: return "IF";
        case NODE_CONDITION: return "CONDITION";
        case NODE_EXPRESSION: return "EXPRESSION";
        case NODE_TERM: return "TERM";
        case NODE_NUMBER: return "NUMBER";
        case NODE_IDENTIFIER: return "IDENTIFIER";
        case NODE_COMPARATOR: return "COMPARATOR";
        case NODE_OPERATOR: return "OPERATOR";
        default: return "UNKNOWN";
    }
}

void printParser(ParserNode *root, int depth){
    if(!root) return;
    for(int i=0;i<depth ; i++) printf(" ");
    printf("%s", nodeTypeName(root->type));
    if(root->token.value[0] != '\0'){
        printf("(%s)",root->token.value);
    }
    printf("\n");
    for(int i = 0; i<root->child_count; i++){
        printParser(root->children[i],depth+3);
    }
}

void freeParser(ParserNode *root){
    for(int i = 0;i< root->child_count;i++){
        freeParser(root->children[i]);
    }
    free(root->children);
    free(root);
}