#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../constants/types.h"
#include "../constants/symbols.h"
#include "./symbol.h"
#include "./exception.h"
#include "./token.h"

#define MAX_SYMBOLS 100

Symbol symbols_table[MAX_SYMBOLS];
int symbols_count = 0;
int priorities[41] = {
    [SOU] = 1,
    [SE] = 2,
    [SMAIOR] = 3,
    [SMAIORIG] = 3,
    [SMENOR] = 3,
    [SMENORIG] = 3,
    [SIG] = 3,
    [SDIF] = 3,
    [SMAIS] = 4,
    [SMENOS] = 4,
    [SMULT] = 5,
    [SDIV] = 5,
    [SNAO] = 6,
    [SPOSITIVO] = 6,
    [SNEGATIVO] = 6,
    [SABREPARENTESES] = 0};

Token *expression[100];

void insere_tabela(char *lexeme, int type, int scope, int memory_address)
{
    if (symbols_count < MAX_SYMBOLS) {
        symbols_table[symbols_count].lexeme = strdup(lexeme);
        symbols_table[symbols_count].type = type;
        symbols_table[symbols_count].scope = scope;
        symbols_table[symbols_count].memory_address = memory_address;
        symbols_count++;
    }
    else {
        printf("Erro: Tabela de símbolos cheia!\n");
    }
}

int busca_tabela(char *lexeme)
{
    for (int i = symbols_count - 1; i >= 0; i--) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type != PROGRAM_NAME && symbols_table[i].type != PROCEDURE) {
            return i;
        }
    }
    return -1;
}

int busca_tabela_sem_restricao(char *lexeme)
{
    for (int i = symbols_count - 1; i >= 0; i--) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0) {
            return i;
        }
    }
    return -1;
}

int verifica_duplicidade(char *lexeme, int scope)
{
    for (int i = symbols_count - 1; i >= 0; i--) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].scope == scope) {
            return 1; // Encontrou duplicidade
        }
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type == PROGRAM_NAME && scope == 0) { // Add scope == 0 !IMPORTANT
            return 1;                                                                                              // Variável com mesmo nome do programa
        }
    }
    return 0; // Não encontrou duplicidade
}

void atualiza_tipo_tabela(char *lexeme, int type)
{
    int indice = busca_tabela(lexeme);
    if (indice != -1) {
        symbols_table[indice].type = type;
    }
}

int pesquisa_declproc_tabela(char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type == PROCEDURE) {
            return i;
        }
    }
    return -1;
}

int pesquisa_declfunc_tabela(char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && (symbols_table[i].type == INTEGER_FUNCTION || symbols_table[i].type == BOOLEAN_FUNCTION)) {
            return i;
        }
    }
    return -1;
}

int pesquisa_declvar_tabela(char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type == VARIABLE) {
            return i;
        }
    }
    return -1;
}

void print_table()
{
    printf("\nTabela de Símbolos:\n");
    printf("Lexeme\t\tType\tScope\tMemory Address\n");
    for (int i = 0; i < symbols_count; i++) {
        printf("%s\t\t%d\t%d\t%d\n", symbols_table[i].lexeme, symbols_table[i].type, symbols_table[i].scope, symbols_table[i].memory_address);
    }
}

Symbol *find_symbol(Symbol *table, char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(table[i].lexeme, lexeme) == 0) {
            return &table[i];
        }
    }
    return NULL;
}

void semantic_analysis(char *lexeme, Token **expression, int size, int is_boolean_expression)
{
    int types[100];
    int types_counter = 0;
    int assignment_expression = 0;

    for (int i = 0; i < size; i++) {
        printf("%s ", expression[i]->lexeme);
    }
    printf("\n");

    for (int i = 0; i < size; i++) {
        Token *term = expression[i];
        if (term->symbol == SATRIBUICAO) {
            assignment_expression = 1;
        }
        if (is_operator(term->symbol)) {
            if (term->symbol == SNAO || term->symbol == SPOSITIVO || term->symbol == SNEGATIVO) {
                continue;
            }
            if (term->symbol == SE || term->symbol == SOU) {
                if (types[types_counter - 1] != BOOLEAN || types[types_counter - 2] != BOOLEAN) {
                    char *message = (char *)malloc(100 * sizeof(char));
                    sprintf(message, "Operator must be applied to Boolean. Found %s and %s.", TypeNames[types[types_counter - 2]], TypeNames[types[types_counter - 1]]);
                    exit_error(message, line_number);
                }
                types_counter--;
            }
            if (term->symbol == SMAIOR || term->symbol == SMAIORIG || term->symbol == SMENOR ||
                term->symbol == SMENORIG || term->symbol == SIG || term->symbol == SDIF) {
                if (types[types_counter - 1] != INTEGER || types[types_counter - 2] != INTEGER) {
                    char *message = (char *)malloc(100 * sizeof(char));
                    sprintf(message, "Operator must be applied to Integer. Found %s and %s.", TypeNames[types[types_counter - 2]], TypeNames[types[types_counter - 1]]);
                    exit_error(message, line_number);
                }
                types_counter -= 2;
                types[types_counter++] = BOOLEAN;
            }
            if (term->symbol == SMAIS || term->symbol == SMENOS || term->symbol == SMULT || term->symbol == SDIV) {
                if (types[types_counter - 1] != INTEGER || types[types_counter - 2] != INTEGER) {
                    char *message = (char *)malloc(100 * sizeof(char));
                    sprintf(message, "Operator must be applied to Integer. Found %s and %s.", TypeNames[types[types_counter - 2]], TypeNames[types[types_counter - 1]]);
                    exit_error(message, line_number);
                }
                types_counter--;
            }
        }
        else {
            if (term->symbol == SNUMERO) {
                types[types_counter++] = INTEGER;
            }
            else if (term->symbol == SIDENTIFICADOR) {
                Symbol *s = find_symbol(symbols_table, term->lexeme);
                if (s->type == INTEGER || s->type == INTEGER_FUNCTION) {
                    types[types_counter++] = INTEGER;
                }
                else if (s->type == BOOLEAN || s->type == BOOLEAN_FUNCTION) {
                    types[types_counter++] = BOOLEAN;
                }
            }
            else if (term->symbol == SVERDADEIRO || term->symbol == SFALSO) {
                types[types_counter++] = BOOLEAN;
            }
        }
    }

    if (types[0] != BOOLEAN && is_boolean_expression) {
        char *message = (char *)malloc(100 * sizeof(char));
        sprintf(message, "Incompatible type for expression. Found %s.", TypeNames[types[0]]);
        exit_error(message, line_number);
    }

    if (assignment_expression) {
        Symbol *id = find_symbol(symbols_table, lexeme);

        if (id->type == INTEGER_FUNCTION || id->type == INTEGER) {
            if (types[0] != INTEGER) {
                char *message = (char *)malloc(100 * sizeof(char));
                sprintf(message, "Incompatible type for expression. Found %s.", TypeNames[types[0]]);
                exit_error(message, line_number);
            }
        }
        else if (id->type == BOOLEAN_FUNCTION || id->type == BOOLEAN) {
            if (types[0] != BOOLEAN) {
                char *message = (char *)malloc(100 * sizeof(char));
                sprintf(message, "Incompatible type for expression. Found %s.", TypeNames[types[0]]);
                exit_error(message, line_number);
            }
        }
    }
}

Token **infix_to_postfix(Token **expression, int size, int *new_size)
{
    Token **new_expression = (Token **)malloc(size * sizeof(Token *));
    Token **stack = (Token **)malloc(size * sizeof(Token *));
    int stack_top = -1;
    int new_size_counter = 0;

    for (int i = 0; i < size; i++) {
        int term = expression[i]->symbol;

        if (is_operand(term)) {
            new_expression[new_size_counter++] = expression[i];
        }
        else if (is_operator(term)) {
            while (stack_top >= 0 && priorities[stack[stack_top]->symbol] >= priorities[term]) {
                new_expression[new_size_counter++] = stack[stack_top--];
            }
            stack[++stack_top] = expression[i];
        }
        else if (term == SABREPARENTESES) {
            stack[++stack_top] = expression[i];
        }
        else if (term == SFECHAPARENTESES) {
            while (stack[stack_top]->symbol != SABREPARENTESES) {
                new_expression[new_size_counter++] = stack[stack_top--];
            }
            stack_top--;
        }
    }

    while (stack_top >= 0) {
        new_expression[new_size_counter++] = stack[stack_top--];
    }

    *new_size = new_size_counter;
    free(stack);

    return new_expression;
}

void print_expression(int expression_size)
{
    printf("\nExpressão:\n");
    for (int i = 0; i < expression_size; i++) {
        printf("%s ", expression[i]->lexeme);
    }
    printf("\n");
}

#endif // SYMBOL_TABLE_H
