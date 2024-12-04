#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headers/inter_rep.h"
#include "Headers/codegen.h"
#include "Headers/parser.h"
#include "Headers/error_handler.h"

// Structure for a node that will hold a variables data
typedef struct
{
    DataType type;
    const char* variable;
    const char* value;
    struct VarNode* next;
}
VarNode;

typedef struct
{
    VarNode* head;
    VarNode* tail;
}
VarList;

// Helper: Creates a VarList
VarList* create_var_list()
{
    VarList* list = (VarList*)malloc(sizeof(VarList));
    list->head = list->tail = NULL;

    return list;
}

// Helper: Creates a node for VarList
VarNode* create_var_node(DataType type, const char* variable, const char* value)
{
    VarNode* node = (VarNode*)malloc(sizeof(VarNode));
    node->type = type;
    node->variable = variable;
    node->value = value;
    node->next = NULL;

    return node;
}

// Helper: Frees the nodes
void free_nodes(VarNode* head)
{
    if (!head) return;
    free_nodes((VarNode*) head->next);
    free(head);
}

// Helper: Finds and returns the node from head
VarNode* find_node(VarNode* head, const char* var)
{
    if (!head) return NULL;
    if (strcmp(head->variable, var) == 0) return head;
    return find_node((VarNode*) head->next, var);
}

// Appends the VarNode to the end of the list;
void append_var(VarList* list, IRInstruction* instr)
{
    VarNode* var =  find_node(list->head, instr->operand1);

    if (!var)
    {
        VarNode* node = create_var_node(instr->data_type, instr->operand1, instr->operand2);
        if (!list->head)
        {
            list->head = node;
            list->tail = node;
        }
        else
        {
            list->tail->next = (struct VarNode*)node;
            list->tail = node;
        }
    }
    else {
        var->value = instr->operand2;
        var->type = NONE;
    }
}

// Frees the variable list
void free_var_list(VarList* list)
{
    free_nodes(list->head);
    free(list);
}

// Finds and returns the variable from list
VarNode* find_var(VarList* list, const char* var)
{
    VarNode* node = find_node(list->head, var);
    if (!node)
        throw_error(NAME_ERROR, "EaseLang/codegen.c", NULL, concat(concat("Name \"", (char*)var, 0), "\" is not defined", 0));
    return node;
}

// Helper: Extracts an equation from string
char* extract_equation(const char* str, int start_idx, int i)
{
    char* out = (char*)malloc(i-start_idx);
    for (int j=0; j<i-start_idx; j++) out[j] = str[start_idx+j];
    out[i-start_idx] = '\0';
    return out;
}

// Parses a string for print 
char* parse_str_for_print(const char* str, VarList* list)
{
    char* str_f = (char*)malloc(5);
    char* str_v = (char*)malloc(1);
    if (!str_f || !str_v) throw_error(MEM_ALLOCATION_ERROR, "EaseLang/codegen.c", NULL, "Maybe you don't have enough memory");
    str_f[0] = '\"';
    int len_f = 2, len_v = 1;
    int idx_f = 1, idx_v = 0;

    for (int i=0; i< (int)strlen(str); i++)
    {
        str_f = (char*)realloc(str_f, ++len_f);
        if (!str_f) throw_error(MEM_ALLOCATION_ERROR, "EaseLang/codegen.c", NULL, "Maybe you don't have enough memory");
        if (str[i] == '{')
        {
            int start_idx = ++i;
            while (str[i] != '}' && str[i] != '\0') i++;
            if (str[i] != '\0')
            {
                // Getting the variable from VarList
                char* var_name = extract_equation(str, start_idx, i);
                VarNode* var = find_var(list, var_name);

                // Getting a format for that variable and appending it to format string str_f
                str_f = (char*)realloc(str_f, ++len_f);
                if (!str_f) throw_error(MEM_ALLOCATION_ERROR, "EaseLang/codegen.c", NULL, "Maybe you don't have enough memory");
                const char* format = "";
                switch (var->type)
                {
                    case INT: format = "%d"; break;
                    case FLOAT: format = "%f"; break;
                    case CHAR: format = "%c"; break;
                    case STRING: format = "%s"; break;
                    case NONE: break;
                }
                strncpy(str_f+idx_f, format, 2);
                idx_f += 2;

                // Appending variable name to variables string str_v
                len_v += strlen(var_name)+1;
                str_v = (char*)realloc(str_v, len_v);
                if (!str_v) throw_error(MEM_ALLOCATION_ERROR, "EaseLang/codegen.c", NULL, "Maybe you don't have enough memory");
                str_v[idx_v++] = ',';
                strncpy(str_v+idx_v, var_name, strlen(var_name));
                idx_v += strlen(var_name);

                free(var_name);
            }
            else
            {
                i = start_idx-1;
                continue;
            }
        }
        else str_f[idx_f++] = str[i];
    }

    str_f[idx_f++] = '\\';
    str_f[idx_f++] = 'n';
    str_f[idx_f++] = '\"';
    str_f[idx_f] = '\0';
    str_v[idx_v] = '\0';

    return concat(str_f, str_v, 0);
}

void generate_c(IRInstruction* instructions, int instruction_count, const char* output_f)
{
    FILE* c_f = fopen(output_f, "w");
    if (!c_f)
        throw_error(FILE_NOT_FOUND_ERROR, "EaseLang/codegen.c", NULL, "File may not exist");

    // A list to hold all the variables
    VarList* list = create_var_list();

    // Writing C boilerplate
    fprintf(c_f, "#include <stdio.h>\n\n");
    fprintf(c_f, "int main()\n{\n");

    // Translate IR to C instructions
    for (int i=0; i<instruction_count; i++)
    {
        IRInstruction* instruction = get_instruction(instructions, i);
        // printf("%d %d %s %s\n", instruction->inst_type, instruction->data_type, instruction->operand1, instruction->operand2);
        switch (instruction->inst_type)
        {
            case STORE_VAR: {
                append_var(list, instruction);
                const char* datatype = "";
                switch (instruction->data_type)
                {
                    case INT: datatype = "int"; break;
                    case FLOAT: datatype = "float"; break;
                    case CHAR: datatype = "char"; break;
                    case STRING: datatype = "char*"; break;
                    case NONE: break;
                }
                if (instruction->data_type != NONE)
                    fprintf(c_f, "    %s %s = ", datatype, instruction->operand1);
                else  fprintf(c_f, "    %s = ", instruction->operand1);
                if (instruction->data_type == STRING) fprintf(c_f, "\"%s\";\n", instruction->operand2);
                else if (instruction->data_type == CHAR) fprintf(c_f, "'%s';\n", instruction->operand2);
                else fprintf(c_f, "%s;\n", instruction->operand2);

                break;
            }
            case PRINT: {
                fprintf(c_f, "    printf(%s);\n", parse_str_for_print(instruction->operand2, list));
            }
        }
    }
    
    // Exit program
    fprintf(c_f, "    return 0;\n}");
    
    free_var_list(list);
}
