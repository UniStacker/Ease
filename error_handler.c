#include <stdio.h>
#include <stdlib.h>
#include "Headers/error_handler.h"

const char* error_array[] = {
    "Syntax", "Memory Allocation", "Lexing", "Parsing", "IR", "Code Generation", "Compiling", "File Not Found",
    "Name"
};


void throw_error(ErrorType type, const char* file_path, int* line_no, const char* hint)
{
    printf(BOLD RED "%s Error: " RESET, error_array[type]);
    printf("in ");
    printf(BOLD UNDERLINE BLUE "\"%s\" " RESET, file_path);
    if (line_no)
    {
        printf("at ");
        printf(BOLD UNDERLINE BLUE "line %d" RESET, *line_no);
    }
    printf(BOLD GREEN "\nHint: " RESET);
    printf("%s\n", hint);

    exit(EXIT_FAILURE);
}