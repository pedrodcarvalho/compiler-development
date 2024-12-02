#ifndef CODE_GENERATOR_C
#define CODE_GENERATOR_C

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./utils/symbol_table.h"
#include "./utils/token.h"

#define MAX_CODE_LENGTH 10000

#define LDC "LDC"
#define LDV "LDV"
#define ADD "ADD"
#define SUB "SUB"
#define MULT "MULT"
#define DIVI "DIVI"
#define INV "INV"
#define AND "AND"
#define OR "OR"
#define NEG "NEG"
#define CME "CME"
#define CMA "CMA"
#define CEQ "CEQ"
#define CDIF "CDIF"
#define CMEQ "CMEQ"
#define CMAQ "CMAQ"
#define START "START"
#define HLT "HLT"
#define STR "STR"
#define JMP "JMP"
#define JMPF "JMPF"
#define ALLOC "ALLOC"
#define DALLOC "DALLOC"
#define CALL "CALL"
#define RETURN "RETURN"
#define RETURNF "RETURNF"
#define PRN "PRN"
#define RD "RD"

typedef struct CodeGenerator {
    char code[MAX_CODE_LENGTH];
    int label_counter;
    int memory_pos;
    int variable_alloc[100];
    int alloc_idx;
} CodeGenerator;

void init_code_generator(CodeGenerator *generator)
{
    generator->code[0] = '\0';
    generator->label_counter = 1;
    generator->memory_pos = 0;
    generator->alloc_idx = 0;
}

void generate(CodeGenerator *generator, int *label_1, const char *command, int *label_2, int *memory_address)
{
    char label_1_stringfied[10] = {0};
    char label_2_stringfied[10] = {0};
    char memory_address_stringfied[10] = {0};

    if (label_1) {
        sprintf(label_1_stringfied, "%d", *label_1);
    }
    if (label_2) {
        sprintf(label_2_stringfied, "%d", *label_2);
    }
    if (memory_address) {
        sprintf(memory_address_stringfied, "%d", *memory_address);
    }

    char buffer[100] = {0};

    if (label_1) {
        sprintf(buffer + strlen(buffer), "%-4s", label_1_stringfied);
    }
    else {
        strcat(buffer, "    ");
    }

    if (command) {
        sprintf(buffer + strlen(buffer), "%-7s", command);
    }

    if (label_2) {
        sprintf(buffer + strlen(buffer), " %-3s", label_2_stringfied);
    }

    if (memory_address) {
        sprintf(buffer + strlen(buffer), " %-3s", memory_address_stringfied);
    }

    strcat(buffer, "\n");
    strcat(generator->code, buffer);
}

void generate_expression(CodeGenerator *generator, Token_t **tokens, int token_count, int current_scope)
{
    for (int i = 0; i < token_count; i++) {
        char *token = tokens[i]->lexeme;

        if (strcmp(token, "+") == 0) {
            if (tokens[i]->symbol != SPOSITIVO) {
                generate(generator, NULL, ADD, NULL, NULL);
            }
        }
        else if (strcmp(token, "-") == 0) {
            if (tokens[i]->symbol == SNEGATIVO) {
                generate(generator, NULL, INV, NULL, NULL);
            }
            else {
                generate(generator, NULL, SUB, NULL, NULL);
            }
        }
        else if (strcmp(token, "*") == 0) {
            generate(generator, NULL, MULT, NULL, NULL);
        }
        else if (strcmp(token, "div") == 0) {
            generate(generator, NULL, DIVI, NULL, NULL);
        }
        else if (strcmp(token, "<") == 0) {
            generate(generator, NULL, CME, NULL, NULL);
        }
        else if (strcmp(token, "<=") == 0) {
            generate(generator, NULL, CMEQ, NULL, NULL);
        }
        else if (strcmp(token, ">") == 0) {
            generate(generator, NULL, CMA, NULL, NULL);
        }
        else if (strcmp(token, ">=") == 0) {
            generate(generator, NULL, CMAQ, NULL, NULL);
        }
        else if (strcmp(token, "=") == 0) {
            generate(generator, NULL, CEQ, NULL, NULL);
        }
        else if (strcmp(token, "!=") == 0) {
            generate(generator, NULL, CDIF, NULL, NULL);
        }
        else if (isdigit(token[0])) {
            int value = atoi(token);
            generate(generator, NULL, LDC, NULL, &value);
        }
        else if (strcmp(token, "verdadeiro") == 0) {
            int value = 1;
            generate(generator, NULL, LDC, NULL, &value);
        }
        else if (strcmp(token, "falso") == 0) {
            int value = 0;
            generate(generator, NULL, LDC, NULL, &value);
        }
        else if (strcmp(token, "e") == 0) {
            generate(generator, NULL, AND, NULL, NULL);
        }
        else if (strcmp(token, "ou") == 0) {
            generate(generator, NULL, OR, NULL, NULL);
        }
        else if (strcmp(token, "nao") == 0) {
            generate(generator, NULL, NEG, NULL, NULL);
        }
        else {
            int memory_address = get_memory_address(token, current_scope);
            if (symbols_table[search_tabel(token)].type == INTEGER_FUNCTION || symbols_table[search_tabel(token)].type == BOOLEAN_FUNCTION) {
                int zero = 0;
                generate(generator, NULL, LDV, NULL, &zero);
            }
            else {
                generate(generator, NULL, LDV, NULL, &memory_address);
            }
        }
    }
}

void save_code_to_file(CodeGenerator *generator, const char *filePath)
{
    FILE *file = fopen(filePath, "w");
    if (file) {
        fprintf(file, "%s", generator->code);
        fclose(file);
    }
    else {
        perror("Error creating file");
    }
}

void print_generated_code_debug(CodeGenerator *generator)
{
    printf("%s", generator->code);
}

#endif // CODE_GENERATOR_C
