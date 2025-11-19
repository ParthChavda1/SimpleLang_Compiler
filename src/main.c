#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include<stdlib.h>


int main(int argc, char **argv){
    if(argc <3){
        printf("Usage: make run INPUT=<input_file> OUTPUT=<output_file_path>\n");
        return 0;
    }
    const char *inputFile = argv[1];
    const char *outputFile = argv[2];
    FILE *file = fopen(inputFile,"r");
    printf("Processing Lexical Analysis...\n");
    Token token;
    int capacity = 100;
    Token *token_list = (Token *)malloc( sizeof(Token)*capacity);
    int count = 0;
    do{
        if(count >= capacity){
            capacity*=2;
            Token *new_list = (Token *)realloc(token_list, (sizeof(Token))*capacity);
            if(new_list == NULL){
                printf("Memmory Reallocation Failed");
                return 1;
            }
            token_list = new_list;
        }
        getNextToken(file,&token);
        token_list[count++] = token;

    }while(token.type != TOKEN_EOF);
    for(int i=0; i<count; i++){
        printf("%d) Token: %d, Value: %s\n",i,token_list[i].type,token_list[i].value);
    }
    printf("Lexical Analysis Complete\n");
    fclose(file);
    printf("Processing Syntax Analysis...\n");
    Parser *parser = createParser(token_list,count);
    ParserNode * root = parseProgram(parser);
    printf("Syntax Analysis complete\n");
    printParser(root,3);
    if(parser_err_count==0){
        printf("Processing Semantic Analysis... \n");
        
        Symbol *s = checkSemantic(root);
        
        printf("Semantic Analysis Complete\n");
        if(semantic_err_count==0){
            printf("Generating assembly Code... \n");
            generateCode(outputFile,root,s);
            printf("Code Generation Complete\n");
            freeSymbolTable(s);
        }

    }
    freeParser(root);
    free(token_list);
    return 1;
}





