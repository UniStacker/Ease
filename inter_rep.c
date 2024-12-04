#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cjson/cJSON.h>
#include "Headers/inter_rep.h"


// Helper: Checks if two strings are equal
int is_equal(const char* s1, const char* s2)
{
    return strcmp(s1, s2) == 0 ? 1 : 0;
}

// Helper: Check if string is a number
int isnum(const char* str)
{
    int one_dot = 0;
    for (int i=0; str[i] != '\0'; i++)
    {
        if (str[i] == '.' && !one_dot) one_dot = 1;
        else if (str[i] == '.' && one_dot) {one_dot = 0; break;}
    }
    for (int i=0; str[i] != '\0'; i++) if (!isdigit(str[i]) && !(str[i] == '.' && one_dot && isdigit(str[i+1]))) return 0;
    return 1;
}

// Helper: Returns the data type of a string
DataType get_datatype(const char* str)
{
    // Check numerical data types
    if (isnum(str))
    {
        for (int i=0; str[i] != '\0'; i++) if (str[i] == '.') return FLOAT;
        return INT;
    }
    // Check Characters
    else
    {
        if (strlen(str) <= 1) return CHAR;
        else return STRING;
    }
    return NONE;
}

// Gets the instruction at idx
IRInstruction* get_instruction(IRInstruction* instructions, int idx)
{
    if (!instructions) return NULL;
    if (idx == 0) return instructions;
    return get_instruction((IRInstruction*) instructions->next, idx-1);
}

// Create an IRList
IRList* create_ir()
{
    IRList* ir = (IRList*)malloc(sizeof(IRList*));
    ir->instructions = ir->tail = NULL;
    ir->total_instructions = 0;
    return ir;
}

// Prints the IR for debugging
void print_ir(IRList* __ir)
{
    for (int i=0; i<__ir->total_instructions; i++) {
        char* type = "";
        IRInstruction* instruction = get_instruction(__ir->instructions, i);
        if (!instruction)
        {
            printf("IR Error: An error occured while reading instruction from IR at index: %d.\n", i);
            exit(EXIT_FAILURE);
        }
        switch (instruction->inst_type)
        {
            case STORE_VAR: type = "STORE_VAR"; break;
            case PRINT: type = "PRINT"; break;
            default: type = "Unknown"; break;
        }

        printf("%s %s %s\n", type, instruction->operand1, instruction->operand2);
    }
}

int append_ir(IRList* __ir, InstructionType __type, const char* __op1, const char* __op2)
{
    IRInstruction* instruction = (IRInstruction*)malloc(sizeof(IRInstruction));
    if (!instruction) return 1;
    instruction->next = NULL;

    instruction->inst_type = __type;
    instruction->operand1 = __op1;
    instruction->operand2 = __op2;
    instruction->data_type = get_datatype(__op2);

    if (!__ir->instructions)
    {
        __ir->instructions = instruction;
        __ir->tail = instruction;
    }
    else
    {
        __ir->tail->next = (struct IRInstruction*) instruction;
        __ir->tail = instruction;
    }
    __ir->total_instructions++;
    return 0;
}

void free_ir(IRList* __ir)
{
    // for (int i=0; i < __ir->total_instructions; i++) free(__ir->instructions[i]);
    free(__ir);
}

void generate_ir(cJSON* __json, IRList* __ir)
{
    cJSON* body = cJSON_GetObjectItemCaseSensitive(__json, "body");
    int length = cJSON_GetArraySize(body);
    for (int i=0; i<length; i++)
    {
        cJSON* object = cJSON_GetArrayItem(body, i);

        cJSON* type_obj = cJSON_GetObjectItemCaseSensitive(object, "type");
        const char* type;
        if (type_obj && cJSON_IsString(type_obj)) type = type_obj->valuestring;
        else {printf("IR Error: Type not in JSON or isn't a string.\n"); exit(EXIT_FAILURE);}
        cJSON* value_obj = cJSON_GetObjectItemCaseSensitive(object, "value");
        const char* value = "";
        if (value_obj && cJSON_IsString(value_obj)) value = value_obj->valuestring;
        else {printf("IR Error: Value not in JSON or isn't a string.\n"); exit(EXIT_FAILURE);}

        if (is_equal(type, "Assignment"))
        {
            cJSON* variable_obj = cJSON_GetObjectItemCaseSensitive(object, "variable");
            const char* variable = "";
            if (variable_obj && cJSON_IsString(variable_obj)) variable = variable_obj->valuestring;
            else {printf("IR Error: Variable not in JSON or isn't a string.\n"); exit(EXIT_FAILURE);}

            if (append_ir(__ir, STORE_VAR, variable, value))
            {
                printf("IR Error: An error occured while appending instruction to IR.\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (is_equal(type, "Output"))
            if (append_ir(__ir, PRINT, "say", value))
            {
                printf("IR Error: An error occured while appending instruction to IR.\n");
                exit(EXIT_FAILURE);
            }
        
        // print_ir(__ir);
        // printf("\n");
        // cJSON_Delete(type_obj);
        // cJSON_Delete(value_obj);
        // cJSON_Delete(variable_obj);
        // free(type);
        // free(value);
    }
}