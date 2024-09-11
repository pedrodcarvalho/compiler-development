#ifndef SEMANTIC_C
#define SEMANTIC_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./constants/symbols.h"
#include "./constants/types.h"
#include "./utils/exception.h"
#include "./utils/symbol.h"
#include "./utils/symbol_table.h"
#include "./utils/token.h"

typedef struct Semantic {
    SymbolTable *table;
    int priorities[40];
} Semantic;

Semantic *create_semantic()
{
    Semantic *semantic = (Semantic *)malloc(sizeof(Semantic));
    semantic->table = create_symbol_table();
    semantic->priorities[SOU] = 1;
    semantic->priorities[SE] = 2;
    semantic->priorities[SMAIOR] = 3;
    semantic->priorities[SMAIORIG] = 3;
    semantic->priorities[SMENOR] = 3;
    semantic->priorities[SMENORIG] = 3;
    semantic->priorities[SIG] = 3;
    semantic->priorities[SDIF] = 3;
    semantic->priorities[SMAIS] = 4;
    semantic->priorities[SMENOS] = 4;
    semantic->priorities[SMULT] = 5;
    semantic->priorities[SDIV] = 5;
    semantic->priorities[SNAO] = 6;
    semantic->priorities[SPOSITIVO] = 6;
    semantic->priorities[SNEGATIVO] = 6;
    semantic->priorities[SABREPARENTESES] = 0;
    return semantic;
}

void semantic_insert_symbol(Semantic *semantic, int type, const char *lexeme, int scope, const char *memory_address)
{
    insert_symbol(semantic->table, type, lexeme, scope, NULL);
}

void semantic_insert_symbol_with_memory_address(Semantic *semantic, int type, const char *lexeme, int scope, const char *memory_address)
{
    insert_symbol(semantic->table, type, lexeme, scope, memory_address);
}

void semantic_update_symbol_type(Semantic *semantic, int type)
{
    update_symbol_type(semantic->table, type);
}

void semantic_clear_scope(Semantic *semantic)
{
    clear_scope(semantic->table);
}

void semantic_remove_symbols(Semantic *semantic)
{
    for (int i = 0; i < semantic->table->size; i++) {
        remove_symbol(semantic->table, semantic->table->symbols[i]->lexeme);
    }
}

Symbol *semantic_find_symbol(Semantic *semantic, const char *lexeme)
{
    return find_symbol(semantic->table, lexeme);
}

Symbol **semantic_get_variables(Semantic *semantic, int *count)
{
    return get_variables(semantic->table, count);
}

int semantic_is_declarated_in_scope(Semantic *semantic, const char *lexeme)
{
    return is_declared_in_scope(semantic->table, lexeme);
}

int semantic_is_declared(Semantic *semantic, const char *lexeme)
{
    return is_declared(semantic->table, lexeme);
}

void semantic_analysis(Semantic *semantic, Symbol *id, Token **expression, int size, int line_number)
{
    int types[100];
    int types_counter = 0;

    for (int i = 0; i < size; i++) {
        Token *term = expression[i];
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
                Symbol *s = find_symbol(semantic->table, term->lexeme);
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

Token **infix_to_postfix(Semantic *semantic, Token **expression, int size, int *new_size)
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
            while (stack_top >= 0 && semantic->priorities[stack[stack_top]->symbol] >= semantic->priorities[term]) {
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

#endif // SEMANTIC_C
