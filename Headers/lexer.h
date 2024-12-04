#ifndef LEXER_H
#define LEXER_H

// Maximum length of a token value
#define MAX_TOKEN_LENGTH 100
#define MAX_TOKENS 1000
#define true 1
#define false 0

// Token types
typedef enum
{
    KEYWORD,
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    OPERATOR,
    END_OF_FILE,
    END_OF_LINE,
    INVALID
}
TokenType;

// Token structure
typedef struct
{
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
}
Token;

// Function prototypes
Token* tokenize(const char* source_code, int* token_count);
void print_token(Token token);
int is_operator(const char* str);

#endif // LEXER_H