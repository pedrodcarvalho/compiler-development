#ifndef CODE_READER_H
#define CODE_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

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
        memset(*stack + *stackSize, 0, (newSize - *stackSize) * sizeof(int));
    }
    *stackSize = newSize;
}

void execute(Instruction *instruction, int **stack, int *stackSize, int *i, FILE *output, int **jumps, int jumpsCount, GtkWidget *widget)
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
    else if (strcmp(instruction->command, "RD") == 0) {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Input Required", GTK_WINDOW(gtk_widget_get_toplevel(widget)), GTK_DIALOG_MODAL, "_OK", GTK_RESPONSE_OK, NULL);
        gtk_window_set_default_size(GTK_WINDOW(dialog), 300, -1);
        GtkWidget *entry = gtk_entry_new();
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry, TRUE, TRUE, 0);
        gtk_widget_show_all(dialog);

        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
            const char *value = gtk_entry_get_text(GTK_ENTRY(entry));
            int int_value = atoi(value);
            safe_realloc(stack, stackSize, *stackSize + 1);
            (*stack)[*stackSize - 1] = int_value;
        }
        gtk_widget_destroy(dialog);
    }
    else if (strcmp(instruction->command, "PRN") == 0) {
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
    else if (strcmp(instruction->command, "ALLOC") == 0) {
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
}

int read_code(GtkWidget *widget, char *filename)
{
    int *stack = NULL;
    int stackSize = 0;

    FILE *input = fopen(filename, "r");
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
            else {
                strcpy(instructions[instructionCount].attr1, "");
            }
            if (parsed == 3) {
                strcpy(instructions[instructionCount].attr2, attr2);
            }
            else {
                strcpy(instructions[instructionCount].attr2, "");
            }
            instructionCount++;
        }
        else {
            strcpy(instructions[instructionCount].command, "");
            strcpy(instructions[instructionCount].attr1, "");
            strcpy(instructions[instructionCount].attr2, "");
        }
    }

    int i = 0;

    FILE *stack_log = fopen("stack.log", "w");
    if (!stack_log) {
        perror("Failed to open stack.log");
        fclose(input);
        fclose(output);
        return 1;
    }

    while (strcmp(instructions[i].command, "HLT") != 0) {
        execute(&instructions[i], &stack, &stackSize, &i, output, jumps, jumpsCount, widget);

        if (stackSize == 0) {
            fprintf(stack_log, "Empty");
        }
        for (int j = 0; j < stackSize; j++) {
            fprintf(stack_log, "%d ", stack[j]);
        }
        fprintf(stack_log, "| %s %s %s\n", instructions[i].command, instructions[i].attr1, instructions[i].attr2);

        i++;
    }

    fclose(input);
    fclose(output);
    fclose(stack_log);
    free(stack);
    free(instructions);
    return 0;
}

#endif // CODE_READER_H
