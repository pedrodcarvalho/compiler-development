#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./constants/commands.h"

typedef struct {
    FILE *out_file;
} CodeGenerator;

CodeGenerator *create_code_generator(const char *filename)
{
    CodeGenerator *code_generator = (CodeGenerator *)malloc(sizeof(CodeGenerator));
    if (!code_generator) {
        perror("Error creating code generator");
        exit(EXIT_FAILURE);
    }
    char *new_filename = (char *)malloc(sizeof(char) * 100);
    strncpy(new_filename, filename, strlen(filename) - 4);
    strcat(new_filename, ".vmobj");
    code_generator->out_file = fopen(new_filename, "w");
    if (!code_generator->out_file) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }
    return code_generator;
}

void generate_label(CodeGenerator *code_generator, int label)
{
    if (!code_generator->out_file) {
        create_code_generator("code_generator_output.vmobj");
    }
    fprintf(code_generator->out_file, "L%d:\n", label);
    printf("L%d:\n", label);
}

void generate(CodeGenerator *code_generator, const char *op)
{
    if (!code_generator->out_file) {
        create_code_generator("code_generator_output.vmobj");
    }
    fprintf(code_generator->out_file, "\t%s\n", op);
    printf("\t%s\n", op);
}

void generate_with_string(CodeGenerator *code_generator, const char *op, const char *arg1)
{
    if (!code_generator->out_file) {
        create_code_generator("code_generator_output.vmobj");
    }
    fprintf(code_generator->out_file, "\t%s\t%s\n", op, arg1);
    printf("\t%s\t%s\n", op, arg1);
}

void generate_with_int(CodeGenerator *code_generator, const char *op, int arg1)
{
    if (!code_generator->out_file) {
        create_code_generator("code_generator_output.vmobj");
    }
    fprintf(code_generator->out_file, "\t%s\t%d\n", op, arg1);
    printf("\t%s\t%d\n", op, arg1);
}

void generate_with_two_strings(CodeGenerator *code_generator, const char *op, const char *arg1, const char *arg2)
{
    if (!code_generator->out_file) {
        create_code_generator("code_generator_output.vmobj");
    }
    fprintf(code_generator->out_file, "\t%s\t%s\t%s\n", op, arg1, arg2);
    printf("\t%s\t%s\t%s\n", op, arg1, arg2);
}

void generate_with_two_ints(CodeGenerator *code_generator, const char *op, int arg1, int arg2)
{
    if (!code_generator->out_file) {
        create_code_generator("code_generator_output.vmobj");
    }
    fprintf(code_generator->out_file, "\t%s\t%d\t%d\n", op, arg1, arg2);
    printf("\t%s\t%d\t%d\n", op, arg1, arg2);
}

void close_file(CodeGenerator *code_generator)
{
    if (!code_generator->out_file) {
        create_code_generator("code_generator_output.vmobj");
    }
    fclose(code_generator->out_file);
}

#endif // CODE_GENERATOR_H
