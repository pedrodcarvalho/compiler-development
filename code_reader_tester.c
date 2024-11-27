#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char command[10];
    char attr1[10];
    char attr2[10];
} Instruction;

void safe_realloc(int **stack, int *stackSize, int newSize)
{
    if (newSize == 0) {
        free(*stack);
        *stack = NULL;
        *stackSize = 0;
        return;
    }

    int *temp = realloc(*stack, newSize * sizeof(int));
    if (temp == NULL) {
        perror("Realloc failed");
        free(*stack);
        exit(EXIT_FAILURE);
    }
    *stack = temp;
    if (newSize > *stackSize) {
        // Initialize new elements to zero
        memset(*stack + *stackSize, 0, (newSize - *stackSize) * sizeof(int));
    }
    *stackSize = newSize;
}

void execute(Instruction *instruction, int **stack, int *stackSize, int *i, FILE *output, int **jumps, int jumpsCount)
{
    if (strcmp(instruction->command, "LDC") == 0) {
        safe_realloc(stack, stackSize, *stackSize + 1);
        (*stack)[*stackSize - 1] = atoi(instruction->attr1);
    }
    else if (strcmp(instruction->command, "LDV") == 0) {
        int idx = atoi(instruction->attr1);
        safe_realloc(stack, stackSize, *stackSize + 1);
        (*stack)[*stackSize - 1] = (*stack)[idx];
    }
    else if (strcmp(instruction->command, "ADD") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a + b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "SUB") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a - b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "MULT") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a * b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "DIVI") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a / b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "INV") == 0) {
        int a = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 1] = -a;
    }
    else if (strcmp(instruction->command, "AND") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a && b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "OR") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a || b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "NEG") == 0) {
        int a = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 1] = 1 - a;
    }
    else if (strcmp(instruction->command, "CME") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a < b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "CMA") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a > b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "CEQ") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a == b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "CDIF") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a != b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "CMEQ") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a <= b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "CMAQ") == 0) {
        int a = (*stack)[*stackSize - 2];
        int b = (*stack)[*stackSize - 1];
        (*stack)[*stackSize - 2] = a >= b;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "STR") == 0) {
        int idx = atoi(instruction->attr1);
        (*stack)[idx] = (*stack)[*stackSize - 1];
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "JMP") == 0) {
        for (int j = 0; j < jumpsCount; j++) {
            if (jumps[j][0] == atoi(instruction->attr1)) {
                *i = jumps[j][1];
                printf("Jumping to: %d\n", atoi(instruction->attr1));
                break;
            }
        }
    }
    else if (strcmp(instruction->command, "JMPF") == 0) {
        if ((*stack)[*stackSize - 1] == 0) {
            for (int j = 0; j < jumpsCount; j++) {
                if (jumps[j][0] == atoi(instruction->attr1)) {
                    *i = jumps[j][1];
                    break;
                }
            }
        }
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "NULL") == 0) {
        // Do nothing
    }
    else if (strcmp(instruction->command, "RD") == 0) {
        int value;
        printf("Enter a value: ");
        scanf("%d", &value);
        safe_realloc(stack, stackSize, *stackSize + 1);
        (*stack)[*stackSize - 1] = value;
    }
    else if (strcmp(instruction->command, "PRN") == 0) {
        printf("Result: %d\n", (*stack)[*stackSize - 1]);
        fprintf(output, "%d\n", (*stack)[*stackSize - 1]);
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "CALL") == 0) {
        safe_realloc(stack, stackSize, *stackSize + 1);
        (*stack)[*stackSize - 1] = *i + 1;
        for (int j = 0; j < jumpsCount; j++) {
            if (jumps[j][0] == atoi(instruction->attr1)) {
                *i = jumps[j][1];
                break;
            }
        }
    }
    else if (strcmp(instruction->command, "RETURN") == 0 || strcmp(instruction->command, "RETURNF") == 0) {
        *i = (*stack)[*stackSize - 1] - 1;
        safe_realloc(stack, stackSize, *stackSize - 1);
    }
    else if (strcmp(instruction->command, "HLT") == 0) {
        // Do nothing
    }
    /*
    ALLOC m,n (Alocar memória):
    Para k:=0 até n-1 faça
    {s:=s+1;M[s]:=M[m+k]}
    DALLOC m,n (Desalocar memória):
    Para k:=n-1 até 0 faça
    {M[m+k]:=M[s];s:=s-1}
    */
    if (strcmp(instruction->command, "ALLOC") == 0) {
        for (int k = 0; k < atoi(instruction->attr2); k++) {
            safe_realloc(stack, stackSize, *stackSize + 1);
            (*stack)[*stackSize - 1] = (*stack)[atoi(instruction->attr1) + k];
        }
    }
    else if (strcmp(instruction->command, "DALLOC") == 0) {
        for (int k = atoi(instruction->attr2) - 1; k >= 0; k--) {
            (*stack)[atoi(instruction->attr1) + k] = (*stack)[*stackSize - 1];
            safe_realloc(stack, stackSize, *stackSize - 1);
        }
    }

    // Print stack for debugging
    printf("Stack: ");
    for (int k = 0; k < *stackSize; k++) {
        printf("%d ", (*stack)[k]);
    }
    printf("\n");
}

int main()
{
    int debug = 0;

    int *stack = NULL;
    int stackSize = 0;

    FILE *input = fopen("./output.obj", "r");
    if (!input) {
        printf("Erro ao abrir arquivo de entrada.\n");
        return 1;
    }
    FILE *output = fopen("output.txt", "w");
    if (!output) {
        printf("Erro ao abrir arquivo de saída.\n");
        return 1;
    }

    Instruction *instructions = malloc(100 * sizeof(Instruction));
    if (!instructions) {
        perror("Failed to allocate memory");
        fclose(input);
        return 1;
    }

    char line[50];
    int instructionCount = 0;
    int **jumps = malloc(100 * sizeof(int *));
    int jumpsCount = 0;

    while (fgets(line, sizeof(line), input)) {
        line[strcspn(line, "\n")] = 0;

        char command[10] = {0}, attr1[10] = {0}, attr2[10] = {0};
        int parsed = sscanf(line, "%s %s %s", command, attr1, attr2);

        if (parsed >= 1) {
            strcpy(instructions[instructionCount].command, command);
            if (parsed >= 2) {
                if (strcmp(attr1, "NULL") == 0) {
                    jumps[jumpsCount] = malloc(2 * sizeof(int));
                    jumps[jumpsCount][0] = atoi(command);
                    jumps[jumpsCount][1] = instructionCount;
                    jumpsCount++;
                }
                strcpy(instructions[instructionCount].attr1, attr1);
            }
            if (parsed == 3) {
                strcpy(instructions[instructionCount].attr2, attr2);
            }
            instructionCount++;
        }
    }

    // DEBUG
    printf("Jump table:\n");
    for (int i = 0; i < jumpsCount; i++) {
        printf("%d -> %d\n", jumps[i][0], jumps[i][1]);
    }

    int i = 0;

    while (strcmp(instructions[i].command, "HLT") != 0 /* && debug < 50 */) {
        // DEBUG
        printf("Executing (%d): %s %s %s\n", i, instructions[i].command, instructions[i].attr1, instructions[i].attr2);

        // if (strcmp(instructions[i].command, "CALL") == 0) {
        //     stack = realloc(stack, (stackSize + 1) * sizeof(int));
        //     stack[stackSize] = i + 1;
        //     stackSize++;
        //     printf("Storing next line from CALL: %d\n", i + 1);
        // }

        // if (strcmp(instructions[i].command, "JMP") == 0) {
        //     for (int j = 0; j < jumpsCount; j++) {
        //         if (jumps[j][0] == atoi(instructions[i].attr1)) {
        //             i = jumps[j][1];
        //             printf("Jumping to: %d\n", atoi(instructions[i].attr1));
        //             break;
        //         }
        //     }
        // }

        execute(&instructions[i], &stack, &stackSize, &i, output, jumps, jumpsCount);

        i++;
        debug++;
    }

    fclose(input);
    fclose(output);
    free(stack);
    free(instructions);
    return 0;
}
