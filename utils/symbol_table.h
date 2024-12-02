#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../constants/symbols.h"
#include "../constants/types.h"
#include "./exception.h"
#include "./symbol.h"
#include "./token.h"

#define MAX_SYMBOLS 100

// Symbol_t table definitions
Symbol_t symbols_table[MAX_SYMBOLS]; // Symbol_t table array
int symbols_count = 0;             // Counter for symbols in the table

// Operator precedence definitions
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

Token_t *expression[100];

/**
 * Inserts a new symbol into the symbol table.
 * @param lexeme The lexeme of the symbol.
 * @param type The type of the symbol.
 * @param scope The scope of the symbol.
 * @param memory_address The memory address of the symbol.
 */
void insert_tabel(char *lexeme, int type, int scope, int memory_address)
{
    if (symbols_count < MAX_SYMBOLS) {
        symbols_table[symbols_count].lexeme = strdup(lexeme);
        symbols_table[symbols_count].type = type;
        symbols_table[symbols_count].scope = scope;
        symbols_table[symbols_count].memory_address = memory_address;
        symbols_count++;
    }
    else {
        printf("Error! Symbol_t table is full.\n");
    }
}

/**
 * Searches for a lexeme in the symbol table, excluding PROGRAM_NAME and PROCEDURE types.
 * @param lexeme The lexeme to search for.
 * @return Index of the lexeme if found, or -1 if not found.
 */
int search_tabel(char *lexeme)
{
    for (int i = symbols_count - 1; i >= 0; i--) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type != PROGRAM_NAME && symbols_table[i].type != PROCEDURE) {
            return i;
        }
    }
    return -1;
}

/**
 * Searches for a lexeme in the symbol table without type restrictions.
 * @param lexeme The lexeme to search for.
 * @return Index of the lexeme if found, or -1 if not found.
 */
int search_tabel_no_restrictions(char *lexeme)
{
    for (int i = symbols_count - 1; i >= 0; i--) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Verifies if a lexeme already exists in the given scope or if it matches the program name.
 * @param lexeme The lexeme to check.
 * @param scope The scope in which to check for the lexeme.
 * @return 1 if a duplicate is found, or 0 otherwise.
 */
int is_duplicated(char *lexeme, int scope)
{
    for (int i = symbols_count - 1; i >= 0; i--) {
        if ((strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].scope == scope) || (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type == PROGRAM_NAME)) {
            return 1;
        }
    }
    return 0;
}

/**
 * Updates the type of a symbol in the table for a given lexeme.
 * @param lexeme The lexeme whose type needs to be updated.
 * @param type The new type to set.
 */
void update_type(char *lexeme, int type)
{
    int index = search_tabel(lexeme);
    if (index != -1) {
        symbols_table[index].type = type;
    }
}

/**
 * Searches for a procedure declaration by lexeme in the table.
 * @param lexeme The lexeme of the procedure to find.
 * @return Index of the procedure if found, or -1 otherwise.
 */
int search_procedure_declaration(char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type == PROCEDURE) {
            return i;
        }
    }
    return -1;
}

/**
 * Searches for a function declaration in the given scope by lexeme.
 * @param lexeme The lexeme of the function to search for.
 * @param scope The scope of the function.
 * @return Index of the function if found, or -1 otherwise.
 */
int search_function_declaration(char *lexeme, int scope)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && (symbols_table[i].type == INTEGER_FUNCTION || symbols_table[i].type == BOOLEAN_FUNCTION) && symbols_table[i].scope == scope) {
            return i;
        }
    }
    return -1;
}

/**
 * Searches for a variable declaration in the symbol table.
 * @param lexeme The lexeme of the variable to search for.
 * @return Index of the variable if found, or -1 otherwise.
 */
int search_variable_declaration(char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type == VARIABLE) {
            return i;
        }
    }
    return -1;
}

/**
 * Prints the entire symbol table to the console.
 */
void print_table_debug()
{
    printf("\nSymbol Table:\n");
    printf("Lexeme\t\tType\tScope\tMemory Address\n");
    for (int i = 0; i < symbols_count; i++) {
        printf("%s\t\t%d\t%d\t%d\n", symbols_table[i].lexeme, symbols_table[i].type, symbols_table[i].scope, symbols_table[i].memory_address);
    }
}

/**
 * Finds a symbol by lexeme in the provided symbol table.
 * @param table The table to search within.
 * @param lexeme The lexeme to find.
 * @return Pointer to the symbol if found, or NULL if not found.
 */
Symbol_t *find_symbol(Symbol_t *table, char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(table[i].lexeme, lexeme) == 0) {
            return &table[i];
        }
    }
    return NULL;
}

/**
 * Retrieves the memory address of a symbol given its lexeme and scope.
 * @param lexeme The lexeme of the symbol.
 * @param scope The scope of the symbol.
 * @return Memory address if found, or -1 otherwise.
 */
int get_memory_address(char *lexeme, int scope)
{
    Symbol_t *symbol_ptr = NULL;

    if (scope != 1) {
        for (int i = symbols_count - 1; i >= 0; i--) {
            if (strcmp(symbols_table[i].lexeme, lexeme) == 0) {
                symbol_ptr = &symbols_table[i];
                break;
            }
        }
    }
    else {
        for (int i = 0; i < symbols_count; i++) {
            if (strcmp(symbols_table[i].lexeme, lexeme) == 0) {
                symbol_ptr = &symbols_table[i];
                break;
            }
        }
    }

    return (symbol_ptr != NULL) ? symbol_ptr->memory_address : -1;
}

/**
 * Performs semantic analysis of an expression.
 * Validates types and ensures compatibility.
 */
void semantic_analysis(char *lexeme, Token_t **expression, int size, int is_boolean_expression)
{
    int types[100];
    int types_counter = 0;
    int assignment_expression = 0;

    for (int i = 0; i < size; i++) {
        Token_t *term = expression[i];

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
                    sprintf(message, "Operator must be applied to Boolean. Found %s and %s.", type_names[types[types_counter - 2]], type_names[types[types_counter - 1]]);
                    exit_error(message, line_number);
                }
                types_counter--;
            }
            if (term->symbol == SMAIOR || term->symbol == SMAIORIG || term->symbol == SMENOR || term->symbol == SMENORIG || term->symbol == SIG || term->symbol == SDIF) {
                if (types[types_counter - 1] != INTEGER || types[types_counter - 2] != INTEGER) {
                    char *message = (char *)malloc(100 * sizeof(char));
                    sprintf(message, "Operator must be applied to Integer. Found %s and %s.", type_names[types[types_counter - 2]], type_names[types[types_counter - 1]]);
                    exit_error(message, line_number);
                }
                types_counter -= 2;
                types[types_counter++] = BOOLEAN;
            }
            if (term->symbol == SMAIS || term->symbol == SMENOS || term->symbol == SMULT || term->symbol == SDIV) {
                if (types[types_counter - 1] != INTEGER || types[types_counter - 2] != INTEGER) {
                    char *message = (char *)malloc(100 * sizeof(char));
                    sprintf(message, "Operator must be applied to Integer. Found %s and %s.", type_names[types[types_counter - 2]], type_names[types[types_counter - 1]]);
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
                Symbol_t *s = find_symbol(symbols_table, term->lexeme);
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
        sprintf(message, "Incompatible type for expression. Found %s.", type_names[types[0]]);
        exit_error(message, line_number);
    }

    if (assignment_expression) {
        Symbol_t *id = find_symbol(symbols_table, lexeme);

        if (id->type == INTEGER_FUNCTION || id->type == INTEGER) {
            if (types[0] != INTEGER) {
                char *message = (char *)malloc(100 * sizeof(char));
                sprintf(message, "Incompatible type for expression. Found %s.", type_names[types[0]]);
                exit_error(message, line_number);
            }
        }
        else if (id->type == BOOLEAN_FUNCTION || id->type == BOOLEAN) {
            if (types[0] != BOOLEAN) {
                char *message = (char *)malloc(100 * sizeof(char));
                sprintf(message, "Incompatible type for expression. Found %s.", type_names[types[0]]);
                exit_error(message, line_number);
            }
        }
    }
}

/**
 * Converts an infix expression to postfix notation.
 */
Token_t **infix_to_postfix(Token_t **expression, int size, int *new_size)
{
    Token_t **new_expression = (Token_t **)malloc(size * sizeof(Token_t *));
    Token_t **stack = (Token_t **)malloc(size * sizeof(Token_t *));
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

/**
 * Prints the tokens in an expression.
 */
void print_expression_debug(int expression_size)
{
    printf("\nExpression: ");
    for (int i = 0; i < expression_size; i++) {
        printf("%s ", expression[i]->lexeme);
    }
    printf("\n");
}

#endif // SYMBOL_TABLE_H
