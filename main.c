#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "Headers/lexer.h"
#include "Headers/parser.h"
#include "Headers/inter_rep.h"
#include "Headers/codegen.h"
#include "Headers/error_handler.h"

char* read_file(const char* path)
{
    FILE* fptr = fopen(path, "r");
    if (!fptr)
    {
        printf("Error: No such file exists \"%s\".\n", path);
        return NULL;
    }

    fseek(fptr, 0, SEEK_END);
    long length = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    char* content = (char*)malloc(length+1);
    fread(content, 1, length, fptr);
    content[length] = '\0';
    fclose(fptr);

    return content;
}

int compile()
{
    // printf("CWD: %s\n", getcwd(NULL, 0));
    char command[120];
    snprintf(command, sizeof(command), "%s/compile.sh", getcwd(NULL, 0));
    return system(command);
}

void clean_up(const char* c_file)
{
    char error_msg[126];
    sprintf(error_msg, "Failed to delete file'%s'", c_file);
    if(remove(c_file) != 0) perror(error_msg);
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("Error: No file source file provided.\n");
        exit(EXIT_FAILURE);
    }
    
    // Reading code from provided file path
    char* source_code = read_file(argv[1]);
    int token_count;

    // Tokenizing the source code
    Token* tokens = tokenize(source_code, &token_count);
    if (tokens == NULL)
    {
        printf("Lexing failed.\n");
        exit(EXIT_FAILURE);
    }

    // for (int i=0; i<token_count; i++) {print_token(tokens[i]); printf("\n");}
    
    // Parsing the tokens
    Parser* parser = create_parser(tokens, token_count);
    cJSON* ast = parse_program(parser);
    if (ast == NULL)
    {
        printf("Parsing failed.\n");
        free(tokens);
        exit(EXIT_FAILURE);
    }

    // Saving AST in JSON file.
    char* json_str = cJSON_Print(ast);
    FILE* json_f = fopen("build/AST.json", "w");
    if (!json_f) system("mkdir build");
    json_f = fopen("build/AST.json", "w");
    if (!json_f) {printf("Saving Error: Couldn't save to file \"build/AST.json\"\n\n"); exit(EXIT_FAILURE);}
    fprintf(json_f, "%s", json_str);
    fclose(json_f);

    // Creating IR
    IRList* ir_list = create_ir();
    generate_ir(ast, ir_list);
    // print_ir(ir_list);

    // Converting IR to C Code
    const char* out_file = "";
    if (argc >= 4)
        out_file = argv[3];
    else
        out_file = "build/program";
    
    const char* c_file = concat((char*)out_file, ".c", 0);
    generate_c(ir_list->instructions, ir_list->total_instructions, c_file);

    // if (!compile()) printf("Compiling done\n");
    // else perror("Compiling failed");
    // clean_up(concat((char*)c_file, ".c", 0));

    // Freeing up everything
    free(json_str);
    free_ir(ir_list);
    cJSON_Delete(ast);
    free(tokens);
    free(source_code);
    return 0;
}