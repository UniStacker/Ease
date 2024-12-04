#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "Headers/parser.h"

#define NONE -7642563

// Helper: Create a new AST node
ASTNode* create_ast_node(ASTNodeType type, const char* value)
{
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->type = type;
    strncpy(node->value, value, MAX_TOKEN_LENGTH);
    node->left = node->right = node->next = NULL;
    return node;
}

// Helper: Trims a string
char* trim(const char* s)
{
    int out_i = 0;
    char* out_s = (char*)malloc(strlen(s+1));
    for (int i=0; s[i] != '\0'; i++) if (s[i] != ' ') out_s[out_i++] = s[i];
    out_s[out_i] = '\0';
    return out_s;
}

// Helper: Concatenates two strings
char* concat(char* s1, char* s2, int doTrim)
{
    if (doTrim)
    {
        s1 = trim(s1);
        s2 = trim(s2);
    }
    int len1 = strlen(s1), len2 = strlen(s2);
    char* out_s = (char*)malloc(len1 + len2 + 1);

    strncpy(out_s, s1, len1);
    strncpy(out_s+len1, s2, len2);

    out_s[len1+len2] = '\0';

    if (doTrim)
    {
        free(s1);
        free(s2);
    }
    return out_s;
}

// Create a new parser
Parser* create_parser(Token* tokens, int token_count)
{
    Parser* parser = (Parser*) malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->token_count = token_count;
    parser->current_index = 0;
    return parser;
}

// Get the current token
Token current_token(Parser* parser)
{
    if (parser->current_index < parser->token_count) return parser->tokens[parser->current_index];
    Token eof_token = {END_OF_FILE, ""};
    return eof_token;
}

Token get_token(Parser* parser, int idx)
{
    if (parser->current_index + idx < parser->token_count) return parser->tokens[parser->current_index+idx];
    Token eof_token = {END_OF_FILE, ""};
    return eof_token;
}

// Advance to the next token
void advance(Parser* parser)
{
    if (parser->current_index < parser->token_count) parser->current_index++;
}

// Match and consume a token of a specific type
int match(Parser* parser, TokenType type)
{
    if (current_token(parser).type == type) {advance(parser); return 1;}
    return 0;
}

// Parse an assignment statement: `<identifier> is <expression>`
cJSON* parse_assignment(Parser* parser)
{
    cJSON* object = cJSON_CreateObject();

    Token identifier = current_token(parser);
    if (!match(parser, IDENTIFIER))
    {
        printf("Parsing Error: Expected identifier for assignment.\n");
        return NULL;
    }

    if (strcmp(current_token(parser).value, "=") != 0)
    {
        printf("Parsing Error: expected \"=\" after identifier \"%s\".\n", identifier.value);
        return NULL;
    }
    advance(parser);

    if (current_token(parser).type == NUMBER_LITERAL && current_token(parser).type == STRING_LITERAL)
    {
        printf("Parsing Error: Expected a literal for assignment.\n");
        return NULL;
    }

    char* comp_expression = "";
    while
    (
        current_token(parser).type == NUMBER_LITERAL ||
        current_token(parser).type == STRING_LITERAL ||
        current_token(parser).type == OPERATOR
    )
    {
        if
        (
            current_token(parser).type == OPERATOR &&
            get_token(parser, parser->current_index+1).type != END_OF_FILE &&
            (
                get_token(parser, parser->current_index+1).type == NUMBER_LITERAL ||
                get_token(parser, parser->current_index+1).type == STRING_LITERAL
            )
        )
        {
            if (get_token(parser, parser->current_index+1).type == NUMBER_LITERAL)
            {
                comp_expression = concat(comp_expression, current_token(parser).value, 1);
                advance(parser);
                comp_expression = concat(comp_expression, current_token(parser).value, 1);
            }
            else if (get_token(parser, parser->current_index+1).type == STRING_LITERAL)
            {
                comp_expression = concat(comp_expression, current_token(parser).value, 0);
                advance(parser);
                comp_expression = concat(comp_expression, current_token(parser).value, 0);
            }
        }
        else comp_expression = concat(comp_expression, current_token(parser).value, 0);
        advance(parser);
    }

    cJSON_AddStringToObject(object, "type", "Assignment");
    cJSON_AddStringToObject(object, "variable", identifier.value);
    cJSON_AddStringToObject(object, "value", comp_expression);

    free(comp_expression);
    return object;
}

// Parse an output statement: `show <expression>`
cJSON* parse_output(Parser* parser)
{
    cJSON* object = cJSON_CreateObject();

    if (current_token(parser).type != KEYWORD || strcmp(current_token(parser).value, "say") != 0) 
    {
        printf("Parsing Error: Expected \"say\" statement.\n");
        return NULL;
    }
    advance(parser);

    Token expression = current_token(parser);
    if (expression.type != STRING_LITERAL && expression.type != NUMBER_LITERAL)
    {
        printf("Parsing Error: Expected a string literal for \"say\".\n");
        return NULL;
    }
    advance(parser);

    cJSON_AddStringToObject(object, "type", "Output");
    cJSON_AddStringToObject(object, "value", expression.value);

    return object;
}

cJSON* parse_keyword(Parser* parser)
{
    if (strcmp(current_token(parser).value, "say") == 0)
        return parse_output(parser);
    // else if (strcmp(current_token(parser).value, "true"|| strcmp(current_token(parser).value, "false") == 0))
    //     return parse_logical(parser);
    else
        return NULL;
}

// Parse a program (sequence of statements)
cJSON* parse_program(Parser* parser)
{
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "type", "program");
    cJSON* list = cJSON_CreateArray();


    while (current_token(parser).type != END_OF_FILE)
    {
        cJSON* statement = NULL;

        if (current_token(parser).type == IDENTIFIER)
            statement = parse_assignment(parser);
        else if (current_token(parser).type == KEYWORD)
            statement = parse_keyword(parser);
        else
        {
            printf("Parsing Error: Unexpected token \"%s\"\n", current_token(parser).value);
            return NULL;
        }

        if (!statement) return NULL;

        // Append the object to list
        cJSON_AddItemToArray(list, statement);
    }
    cJSON_AddItemToObject(json, "body", list);
    return json;
}

