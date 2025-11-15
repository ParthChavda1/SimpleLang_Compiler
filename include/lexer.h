#ifndef LEXER
#define LEXER

#include<stdio.h>
#include<stdbool.h>

#define MAX_TOKEN_LEN 100

typedef enum {
    TOKEN_INT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_IF,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_ASSIGN,
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LTE,
    TOKEN_GTE,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_EOF,
    TOKEN_UNKNOWN,
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
}Token;

// get next token from file 
void getNextToken(FILE *file, Token *token);


// helper functions
bool isAlpha(char c);
bool isAlNum(char c);
bool isDigit(char c);
bool isSpace(char c);

#endif