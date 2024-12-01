#ifndef CODE_GENERATOR_C
#define CODE_GENERATOR_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./utils/token.h"
#include "./utils/symbol_table.h"

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
    int labelCounter;
    int memoryPosition;
    int variableAlloc[100];
    int allocIndex;
} CodeGenerator;

void initializeCodeGenerator(CodeGenerator *generator)
{
    generator->code[0] = '\0';
    generator->labelCounter = 1;
    generator->memoryPosition = 0;
    generator->allocIndex = 0;
}

void generate(CodeGenerator *generator, int *label1, const char *command, int *label2, int *memoryAddress)
{
    char label1Str[10] = {0};
    char label2Str[10] = {0};
    char memoryAddressStr[10] = {0};

    if (label1) {
        sprintf(label1Str, "%d", *label1);
    }
    if (label2) {
        sprintf(label2Str, "%d", *label2);
    }
    if (memoryAddress) {
        sprintf(memoryAddressStr, "%d", *memoryAddress);
    }

    char buffer[100] = {0};

    if (label1) {
        sprintf(buffer + strlen(buffer), "%-4s", label1Str);
    }
    else {
        strcat(buffer, "    ");
    }

    if (command) {
        sprintf(buffer + strlen(buffer), "%-7s", command);
    }

    if (label2) {
        sprintf(buffer + strlen(buffer), " %-3s", label2Str);
    }

    if (memoryAddress) {
        sprintf(buffer + strlen(buffer), " %-3s", memoryAddressStr);
    }

    strcat(buffer, "\n");

    strcat(generator->code, buffer);
}

void generateExpression(CodeGenerator *generator, Token **tokens, int tokenCount, int current_scope)
{
    for (int i = 0; i < tokenCount; i++) {
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
        else if (strcmp(token, "==") == 0) {
            generate(generator, NULL, CEQ, NULL, NULL);
        }
        else if (strcmp(token, "!=") == 0) {
            generate(generator, NULL, CDIF, NULL, NULL);
        }
        else if (strcmp(token, "!") == 0) {
            generate(generator, NULL, NEG, NULL, NULL);
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
            int memoryAddress = get_memory_address(token, current_scope);
            if (symbols_table[busca_tabela(token)].type == INTEGER_FUNCTION || symbols_table[busca_tabela(token)].type == BOOLEAN_FUNCTION) {
                int zero = 0;
                generate(generator, NULL, LDV, NULL, &zero);
            }
            else {
                generate(generator, NULL, LDV, NULL, &memoryAddress);
            }
        }
    }
}

void allocateMemory(CodeGenerator *generator, int start, int size)
{
    generate(generator, NULL, ALLOC, NULL, NULL);
    generator->variableAlloc[generator->allocIndex++] = size;
    generator->memoryPosition += size;
}

void deallocateMemory(CodeGenerator *generator)
{
    if (generator->allocIndex > 0) {
        int size = generator->variableAlloc[--generator->allocIndex];
        generator->memoryPosition -= size;
        generate(generator, NULL, DALLOC, NULL, &size);
    }
}

void saveCodeToFile(CodeGenerator *generator, const char *filePath)
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

void debugCode(CodeGenerator *generator)
{
    printf("%s", generator->code);
}

#endif // CODE_GENERATOR_C
