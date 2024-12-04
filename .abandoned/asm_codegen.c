#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headers/inter_rep.h"
#include "Headers/codegen.h"

void generate_assembly(IRInstruction* instructions, int instruction_count, const char* output_f)
{
    FILE* asm_f = fopen(output_f, "w");
    if (!asm_f)
    {
        perror("CodeGeneration Error: Failed to open assembly file.");
        exit(EXIT_FAILURE);
    }

    // Writing assembly boilerplate
    fprintf(asm_f, "section .data\n");
    for (int i=0; i<instruction_count; i++)
    {
        IRInstruction* inst = get_instruction(instructions, i);
        if (inst->type == STORE_VAR && get_instruction(instructions, i-1))
            fprintf(asm_f, "    %s dq %d\n", inst->value, get_instruction(instructions, i-1)->value);
        else if (inst->type == STORE_STRING && get_instruction(instructions, i-1))
            fprintf(asm_f, "    %s db \"%s\", 0\n", inst->value, get_instruction(instructions, i-1)->value);
    }

    fprintf(asm_f, "\nsection .text\nglobal _start\n_start:\n");

    // Translate IR to assembly instructions
    for (int i=0; i<instruction_count; i++)
    {
        IRInstruction* inst = get_instruction(instructions, i);
        switch (inst->type)
        {
            case LOAD_CONST: fprintf(asm_f, "    mov rax, %s\n", inst->value); break;
            case STORE_VAR: fprintf(asm_f, "    mov [%s], rax\n", inst->value); break;
            case LOAD_VAR: fprintf(asm_f, "    mov rax, [%s]\n", inst->value); break;
            case PRINT: fprintf(asm_f, "    call print_number\n"); break;
            case PRINT_STRING: fprintf(asm_f, "    mov rsi, \"%s\"\n    mov rdx, %lu\n    mov rax, 1\n    mov rdi, 1\n    syscall\n", inst->value, strlen(inst->value)); break;
            default: break;
        }
    }

    // Writing the print helper function
    fprintf(asm_f, "\nprint_number:\n");
    fprintf(asm_f, "    mov rbx, 10\n    xor rcx, rcx\n    mov rdi, number_buffer + 11\n\n");
    fprintf(asm_f, "    convert_loop:\n        xor rdx, rdx\n        div rbx\n        add dl, '0'\n        dec rdi\n        mov [rdi], dl\n");
    fprintf(asm_f, "        inc rcx\n        test rax, rax\n        jnz convert_loop\n\n");
    fprintf(asm_f, "        mov rsi, rdi\n        mov rdx, rcx\n        mov rax, 1\n        mov rdi, 1\n        syscall\n        ret\n\n");
    fprintf(asm_f, "section .bss\n    number_buffer resb 12\n");

    // Exit program
    fprintf(asm_f, "    mov rax, 60\n    xor rdi, rdi\n    syscall\n");
    fclose(asm_f);
}
