#ifndef INTER_REP_H
#define INTER_REP_H

#include<cjson/cJSON.h>

typedef enum
{
    STORE_VAR,
    PRINT
}
InstructionType;

typedef  enum
{
    NONE,
    INT,
    FLOAT,
    CHAR,
    STRING
}
DataType;

typedef struct
{
    DataType data_type;
    InstructionType inst_type;
    const char* operand1;
    const char* operand2;
    struct IRInstruction* next;
}
IRInstruction;

typedef struct
{
    IRInstruction* instructions;
    IRInstruction* tail;
    int total_instructions;
}
IRList;

// Function prototypes
IRList* create_ir();
int isnum(const char* str);
IRInstruction* get_instruction(IRInstruction* instructions, int idx);
int append_ir(IRList* __ir, InstructionType __type, const char* __op1, const char* __op2);  // Appends the instruction to __ir
void free_ir(IRList* __ir);  // Frees the memory taken by __ir
void print_ir(IRList* __ir);  // Prints the IR for debugging
void generate_ir(cJSON* __json, IRList* __ir);

#endif // INTER_REP_H