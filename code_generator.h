#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./constants/commands.h"

FILE *out_file = NULL;

void create_file(const char *filename)
{
    out_file = fopen(filename, "w");
    if (!out_file) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }
}

void generate_label(int n)
{
    if (!out_file) {
        create_file("code_generator_output.vmobj");
    }
    fprintf(out_file, "L%d\tNULL\n", n);
    printf("L%d\tNULL\n", n);
}

void generate(const char *op)
{
    if (!out_file) {
        create_file("code_generator_output.vmobj");
    }
    fprintf(out_file, "\t%s\n", op);
    printf("\t%s\n", op);
}

void generate_with_string(const char *op, const char *arg1)
{
    if (!out_file) {
        create_file("code_generator_output.vmobj");
    }
    fprintf(out_file, "\t%s\t%s\n", op, arg1);
    printf("\t%s\t%s\n", op, arg1);
}

void generate_with_int(const char *op, int arg1)
{
    if (!out_file) {
        create_file("code_generator_output.vmobj");
    }
    fprintf(out_file, "\t%s\t%d\n", op, arg1);
    printf("\t%s\t%d\n", op, arg1);
}

void generate_with_two_strings(const char *op, const char *arg1, const char *arg2)
{
    if (!out_file) {
        create_file("code_generator_output.vmobj");
    }
    fprintf(out_file, "\t%s\t%s\t%s\n", op, arg1, arg2);
    printf("\t%s\t%s\t%s\n", op, arg1, arg2);
}

void generate_with_two_ints(const char *op, int arg1, int arg2)
{
    if (!out_file) {
        create_file("code_generator_output.vmobj");
    }
    fprintf(out_file, "\t%s\t%d\t%d\n", op, arg1, arg2);
    printf("\t%s\t%d\t%d\n", op, arg1, arg2);
}

void close_file()
{
    if (out_file) {
        fclose(out_file);
        out_file = NULL;
    }
}

#endif // CODE_GENERATOR_H
