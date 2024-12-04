#ifndef ERROR_HANDLER
#define ERROR_HANDLER

#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[36m"

#define UNDERLINE "\033[4m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

typedef enum
{
    SYNTAX_ERROR,
    MEM_ALLOCATION_ERROR,
    LEXING_ERROR,
    PARSING_ERROR,
    IR_ERROR,
    CODE_GEN_ERROR,
    COMPILING_ERROR,
    FILE_NOT_FOUND_ERROR,
    NAME_ERROR
}
ErrorType;

// Function prototypes
void throw_error(ErrorType type, const char* file_path, int* line_no, const char* hint);

#endif // ERROR_HANDLER