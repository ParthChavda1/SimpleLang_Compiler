#ifndef PARSER_H
#define PARSER_H


#include "lexer.h"
#include<stdio.h>
#include <stdbool.h>
/*
Grammer rules for my complier

program -> statement | epsilon
statement -> Ifstatmemt | Declaration | Assignment

Ifstatement -> 'if' "(" condition ")" "{" statement "}"
Declaration -> "int" Identifier ";"
Assignment-> Identifier "=" Expression ";"

Condition-> Expression Comparator Expression 
comparator -> "==" | ">=" | "<=" | ">" | "<" | "!=" 

expression -> Term {('+' || '-')  Term }
Term -> Identifier | Number | '(' Expression ')'

*/

extern int parser_err_count;

typedef enum {
    NODE_ROOT,
    NODE_DECLARATION,
    NODE_INT,
    NODE_IF,
    NODE_ASSIGNMENT,
    NODE_CONDITION,
    NODE_IDENTIFIER,
    NODE_EXPRESSION,
    NODE_COMPARATOR,
    NODE_TERM,
    NODE_NUMBER,
    NODE_BODY,
    NODE_OPERATOR,
}NodeType;

typedef struct ParserNode ParserNode;
 
struct ParserNode{
    NodeType type;
    Token token;
    ParserNode **children;
    int child_count;
    int child_capacity;
};

typedef struct {
    Token *tokens;
    int current;
    int total_count;
}Parser;


// functions for parsing token
Parser *createParser(Token *tokens, int total_count);

ParserNode *createNode(NodeType type, Token token);
void addChild(ParserNode *parent, ParserNode *child);

ParserNode *parseProgram(Parser *parser);
ParserNode *parseStatement(Parser *parser);
ParserNode *parseDeclaration(Parser *parser);
ParserNode *parseAssignment(Parser *parser);
ParserNode *parseIf(Parser *parser);
ParserNode *parseCondition(Parser *parser);
ParserNode *parseExpression(Parser *parser);
ParserNode *parseTerm(Parser *parser);


// helper function 
// Function to get current token from list of token
Token getCurrentToken(Parser *parser);
// Increment current counter in Parser 
Token advanceToken(Parser *parser);
// Check If the given type is same as current token and increments the current counter
bool isMatch(TokenType type, Parser *parser);
// Check comparator is valid or not
bool isComparator(Parser *parser);

// Functions that helps in debugging
void printParser(ParserNode *root, int depth);
void freeParser(ParserNode *root);









#endif