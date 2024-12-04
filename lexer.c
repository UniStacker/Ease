#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Headers/lexer.h"

// Size of one Token structure
const int TOKEN_SIZE = sizeof(Token);

// Lists of keywords & operators
const char* keywords[] = {"is", "say", "input", "and", "or", "not", "true", "false"};
const char* operators[] = {"+", "-", "/", "*", "<", ">", "==", "<=", ">=", "="};

// Helper: Handle new line characters
// void handle_newline(const char* source_code, int i, Token* tokens, int* token_index, int* tokens_size)
// {
//     if (source_code[i] == '\n')
//     {
//         Token eol_token;
//         eol_token.type = END_OF_LINE;
//         eol_token.value[0] = '\0';
//         tokens[*token_index++] = eol_token;

//         print_token(tokens[*token_index-1]);
//         printf("\n");

//         // tokens = (Token*) realloc(tokens, *tokens_size + TOKEN_SIZE);
//         // *tokens_size += TOKEN_SIZE;
//         // if (tokens == NULL) {printf("Lexing Error: Failed to allocate memmory for tokens array.\n"); exit(EXIT_FAILURE);}
//     }
// }

// Function to check if a string is a keyword
int is_keyword(const char* str)
{
    for (int i=0, size = sizeof(keywords)/sizeof(keywords[0]); i<size; i++) if (strcmp(str, keywords[i]) == 0) return 1;
    return 0;
}

// Function to check if a string is an operator
int is_operator(const char* str)
{
    for (int i=0; i<(int)(sizeof(operators)/sizeof(operators[0])); i++) if (strcmp(operators[i], str) == 0) return 1;
    return 0;
}

// Function to get the next token from the input string
Token* tokenize(const char* source_code, int* token_count)
{
    // Check for source_code being NULL
    if (source_code == NULL)
    {
        fprintf(stderr, "Lexing Error: Source code is NULL.\n");
        exit(EXIT_FAILURE);
    }

    Token* tokens = (Token*) malloc(TOKEN_SIZE);      // Allocate memory for tokens
    if (tokens == NULL) {printf("Lexing Error: Failed to allocate memmory for tokens array.\n"); exit(EXIT_FAILURE);}
    int tokens_size = (int)sizeof(*tokens);
    int i = 0;                                              // Current position in source_code
    int token_index = 0;                                    // Current position in tokens array

    while (source_code[i] != '\0' && token_index < MAX_TOKENS)
    {
        tokens = (Token*) realloc(tokens, tokens_size + TOKEN_SIZE);
        tokens_size += TOKEN_SIZE;
        if (tokens == NULL) {printf("Lexing Error: Failed to allocate memmory for tokens array.\n"); exit(EXIT_FAILURE);}

        // handle_newline(source_code, i, tokens, &token_index, &tokens_size);

        // Skip the white space
        while (isspace(source_code[i])) {i++; continue;}

        // handle_newline(source_code, i, tokens, &token_index, &tokens_size);

        // Handle keywords and identifiers
        if (isalpha(source_code[i]) || source_code[i] == '_')
        {
            int start = i;
            while (isalnum(source_code[i]) || source_code[i] == '_') i++;
            Token token;
            strncpy(token.value, &source_code[start], i-start);
            token.value[i-start] = '\0';

            if (is_keyword(token.value)) token.type = KEYWORD;
            else token.type = IDENTIFIER;

            tokens[token_index++] = token;
            continue;
        }

        // Handle string literal
        if (source_code[i] == '"')
        {
            int start = ++i; // Skip the opening quote
            while (source_code[i] != '"' && source_code[i] != '\0') i++;
            if (source_code[i] == '\0')
            {
                printf("Lexing Error: Unterminated string literal");
                free(tokens);
                return NULL;
            }

            Token token;
            strncpy(token.value, &source_code[start], i-start);
            token.value[i-start] = '\0';

            if (is_keyword(token.value)) token.type = KEYWORD;
            else token.type = STRING_LITERAL;

            tokens[token_index++] = token;
            i++; // Skip the closing quote
            continue;
        }

        // Handle number literal
        if (isdigit(source_code[i]))
        {
            int start = i;
            while (isdigit(source_code[i]) || (source_code[i] == '.' && isdigit(source_code[i+1]))) i++;
            Token token;
            strncpy(token.value, &source_code[start], i-start);
            token.value[i-start] = '\0';

            token.type = NUMBER_LITERAL;

            tokens[token_index++] = token;
            continue;
        }

        // Handle punctuation
        if (ispunct(source_code[i]))
        {
            int start = i;
            while (ispunct(source_code[i])) i++;

            char value[MAX_TOKEN_LENGTH];
            strncpy(value, &source_code[start], i-start);

            Token token;

            // Assignment operator
            if (is_operator(value)) token.type = OPERATOR;
            else
            {
                printf("Lexing Error: Unknown symbol \"%s\".\n", value);
                free(tokens);
                return NULL;
            }

            strcpy(token.value, value);

            tokens[token_index++] = token;
            continue;
        }


        // Handle invalid characters
        printf("Lexing Error: Unknown character '%c'\n", source_code[i]);
        free(tokens);
        return NULL;
    }

    // Add an EOF token
    Token eof_token;
    eof_token.type = END_OF_FILE;
    eof_token.value[0] = '\0';
    tokens[token_index] = eof_token;

    *token_count = token_index;
    return tokens;
}

// Function to print a token
void print_token(Token token)
{
    const char* type_str;
    switch (token.type)
    {
        case KEYWORD: type_str = "KEYWORD"; break;
        case IDENTIFIER: type_str = "IDENTIFIER"; break;
        case STRING_LITERAL: type_str = "STRING_LITERAL"; break;
        case NUMBER_LITERAL: type_str = "NUMBER_LITERAL"; break;
        case OPERATOR: type_str = "OPERATOR"; break;
        case END_OF_FILE: type_str = "END_OF_FILE"; break;
        case END_OF_LINE: type_str = "END_OF_LINE"; break;
        default: type_str = "UNKNOWN"; break;
    }

    printf("Token(type: %s, value: \"%s\")", type_str, token.value);
}
