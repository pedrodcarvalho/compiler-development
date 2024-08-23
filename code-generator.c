#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LABEL "LABEL"
#define MAX_OP_SIZE 50
#define MAX_LINE_SIZE 100

typedef struct {
    FILE *file;
} CodeGenerator;

// Function to initialize the CodeGenerator struct and create a new file
CodeGenerator *init_generator(const char *source)
{
    CodeGenerator *cg = (CodeGenerator *)malloc(sizeof(CodeGenerator));
    char filename[MAX_OP_SIZE];

    snprintf(filename, sizeof(filename), "%s.vmobj", source);

    cg->file = fopen(filename, "w");
    if (!cg->file) {
        fprintf(stderr, "Error creating file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    return cg;
}

// Function to generate a label
void generate_label(CodeGenerator *cg, int n)
{
    char line[MAX_LINE_SIZE];
    snprintf(line, sizeof(line), "%s%d\tNULL\n", LABEL, n);
    fprintf(cg->file, "%s", line);
    printf("%s", line);
}

// Function to generate an operation without arguments
void generate_op(CodeGenerator *cg, const char *op)
{
    char line[MAX_LINE_SIZE];
    snprintf(line, sizeof(line), "\t%s\n", op);
    fprintf(cg->file, "%s", line);
    printf("%s", line);
}

// Function to generate an operation with one string argument
void generate_op1(CodeGenerator *cg, const char *op, const char *arg1)
{
    char line[MAX_LINE_SIZE];
    snprintf(line, sizeof(line), "\t%s\t%s\n", op, arg1);
    fprintf(cg->file, "%s", line);
    printf("%s", line);
}

// Function to generate an operation with one integer argument
void generate_op1_int(CodeGenerator *cg, const char *op, int arg1)
{
    char line[MAX_LINE_SIZE];
    snprintf(line, sizeof(line), "\t%s\t%d\n", op, arg1);
    fprintf(cg->file, "%s", line);
    printf("%s", line);
}

// Function to generate an operation with two string arguments
void generate_op2(CodeGenerator *cg, const char *op, const char *arg1, const char *arg2)
{
    char line[MAX_LINE_SIZE];
    snprintf(line, sizeof(line), "\t%s\t%s\t%s\n", op, arg1, arg2);
    fprintf(cg->file, "%s", line);
    printf("%s", line);
}

// Function to generate an operation with two integer arguments
void generate_op2_int(CodeGenerator *cg, const char *op, int arg1, int arg2)
{
    char line[MAX_LINE_SIZE];
    snprintf(line, sizeof(line), "\t%s\t%d\t%d\n", op, arg1, arg2);
    fprintf(cg->file, "%s", line);
    printf("%s", line);
}

// Function to close the file
void close_generator(CodeGenerator *cg)
{
    if (cg->file) {
        fclose(cg->file);
    }
    free(cg);
}

int main()
{
    // Example usage
    CodeGenerator *cg = init_generator("output");

    generate_label(cg, 1);
    generate_op(cg, "LOAD");
    generate_op1(cg, "STORE", "R1");
    generate_op1_int(cg, "LOAD_CONST", 42);
    generate_op2(cg, "ADD", "R1", "R2");
    generate_op2_int(cg, "MULTIPLY", 5, 10);

    close_generator(cg);

    return 0;
}
