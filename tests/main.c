#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {INT, FLOAT, CHAR, STR} VarType;

typedef struct
{
    VarType type;
    char* value;
}
Variable;

void freeVariable(Variable var)
{
    if (var.type == STR) free(var.value);
    var.value = NULL;
}

char* trim(const char* s)
{
    int out_i = 0;
    char* out_s = (char*)malloc(strlen(s+1));
    for (int i=0; s[i] != '\0'; i++) if (s[i] != ' ') out_s[out_i++] = s[i];
    out_s[out_i] = '\0';
    return out_s;
}

char* concat(char* s1, char* s2)
{
    s1 = trim(s1);
    s2 = trim(s2);
    int len1 = strlen(s1), len2 = strlen(s2);
    char* out_s = (char*)malloc(len1 + len2 + 1);

    strncpy(out_s, s1, len1);
    strncpy(out_s+len1, s2, len2);

    free(s1);
    free(s2);
    return out_s;
}

int main(void)
{
    void* var = "Shan";

    printf("Hello, %s! ", (char*)var);

    var = 17;

    printf("You are %d years old ", (int)var);

    // var = 5.9;

    // printf("and %f feet tall.\n", (float)var);
    return 0;
}