#include "lexer.h"


#include <string.h>
void getNextToken(FILE *file, Token *token) {
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (isSpace(c)) continue;
        if (isAlpha(c)) {
        int len = 0;
        token->value[len++] = c;
        while (isAlNum(c = fgetc(file))) {
            if (len < MAX_TOKEN_LEN - 1) token->value[len++] = c;
        }
        ungetc(c, file);
        token->value[len] = '\0';
        if (strcmp(token->value, "int") == 0) token->type = TOKEN_INT;
        else if (strcmp(token->value, "if") == 0) token->type = TOKEN_IF;
        else token->type = TOKEN_IDENTIFIER;
        return;
        }
        if (isDigit(c)) {
            int len = 0;
            token->value[len++] = c;
            while (isDigit(c = fgetc(file))) {
                if (len < MAX_TOKEN_LEN - 1) token->value[len++] = c;
            }
            ungetc(c, file);
            token->value[len] = '\0';
            token->type = TOKEN_NUMBER;
            return;
        }
        switch (c) {
            case '+': token->type = TOKEN_ADD; token->value[0] = '+'; token->value[1] = '\0'; return;
            case '-': token->type = TOKEN_SUB; token->value[0] = '-'; token->value[1] = '\0'; return;
            case '(': token->type = TOKEN_LPAREN; token->value[0] = '('; token->value[1] = '\0'; return;
            case ')': token->type = TOKEN_RPAREN; token->value[0] = ')'; token->value[1] = '\0'; return;
            case '{': token->type = TOKEN_LBRACE; token->value[0] = '{'; token->value[1] = '\0'; return;
            case '}': token->type = TOKEN_RBRACE; token->value[0] = '}'; token->value[1] = '\0'; return;
            case ';': token->type = TOKEN_SEMICOLON; token->value[0] = ';'; token->value[1] = '\0'; return;
            case '=':
                if((c= fgetc(file))=='='){
                    token->type = TOKEN_EQ;
                    strcpy(token->value,"=="); 
                    return;
                } 
                ungetc(c,file);
                token->type = TOKEN_ASSIGN;
                token->value[0] = '='; 
                token->value[1] = '\0'; 
                return;
                case '>': 
                if((c= fgetc(file))=='='){
                    token->type = TOKEN_GTE;
                    strcpy(token->value,">="); 
                    return;
                } 
                ungetc(c,file);
                token->type = TOKEN_GT;
                token->value[0] = '>'; 
                token->value[1] = '\0'; 
                return;
                case '<': 
                if((c= fgetc(file))=='='){
                    token->type = TOKEN_LTE;
                    strcpy(token->value,"<="); 
                    return;
                } 
                ungetc(c,file);
                token->type = TOKEN_LT;
                token->value[0] = '<'; 
                token->value[1] = '\0'; 
                return;
                case '!': 
                if((c= fgetc(file))=='='){
                    token->type = TOKEN_NEQ;
                    strcpy(token->value,"!="); 
                    return;
                } 
                ungetc(c,file);
                token->type = TOKEN_UNKNOWN;
                token->value[0] = '!'; 
                token->value[1] = '\0'; 
                return;
            
        }
    }
    token->type = TOKEN_EOF;
    token->value[0] = '\0';
    return;
}


bool isAlpha(char c){
    return ((c>='A' && c<='Z') || (c>='a' && c<='z'));
}
bool isAlNum(char c){
    return isAlpha(c) || isDigit(c);
}
bool isDigit(char c){
    return (c>= '0' && c<= '9');
}
bool isSpace(char c){
    return (c==' ' || c == '\t' );
}