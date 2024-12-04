#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// AST Node types
typedef enum
{
    AST_ASSIGNMENT,
    AST_INPUT,
    AST_OUTPUT,
    AST_ARITHMATIC_OP,
    AST_LOGICAL_OP,
    AST_COMPARISON_OP,
    AST_STR_LITERAL,
    AST_NUM_LITERAL,
    AST_IDENTIFIER,
    AST_FUNCTION_CALL
}
ASTNodeType;

// AST Node structure
typedef struct
{
    ASTNodeType type;
    char value[MAX_TOKEN_LENGTH];        // For literals, identifiers etc.
    struct ASTNode* left;                // For expressions or assignments.
    struct ASTNode* right;               // For expressions or assignments.
    struct ASTNode* next;                // For sequential statements
    struct ASTNode* args;                // For function arguments.
}
ASTNode;

// Parser structure to maintain state
typedef struct
{
    Token* tokens;
    int current_index;
    int token_count;
}
Parser;

// Function prototypes
Parser* create_parser(Token* tokens, int token_count);
cJSON* parse_program(Parser* parser);
char* concat(char* s1, char* s2, int doTrim);

#endif // PARSER_H