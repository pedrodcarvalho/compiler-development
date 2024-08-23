#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SYMBOLS 100
#define MAX_TOKENS 100

// Symbol Types
#define TYPE_INTEGER 1
#define TYPE_BOOLEAN 2

// Operators
#define OP_OR 1
#define OP_AND 2
#define OP_GREATER 3
#define OP_GREATER_EQUAL 4
#define OP_LESS 5
#define OP_LESS_EQUAL 6
#define OP_EQUAL 7
#define OP_NOT_EQUAL 8
#define OP_PLUS 9
#define OP_MINUS 10
#define OP_MULTIPLY 11
#define OP_DIVIDE 12
#define OP_NOT 13
#define OP_POSITIVE 14
#define OP_NEGATIVE 15
#define OP_OPEN_PARENTHESIS 16
#define OP_CLOSE_PARENTHESIS 17

typedef struct {
    char lexeme[32];
    int type;
    bool scope;
    int memory_address;
} Symbol;

typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    int count;
} SymbolTable;

typedef struct {
    int symbol;
    char lexeme[32];
} Token;

SymbolTable table;

// Function Prototypes
void initialize_table();
void insert_symbol(int type, const char *lexeme, bool scope, int memory_address);
Symbol *find_symbol(const char *lexeme);
void analyze_expression(Symbol *id, Token *expression, int expression_size, int line);
Token *to_postfix(Token *expression, int expression_size, int *new_size);
void error(int line, const char *msg);

// Initialize Symbol Table
void initialize_table()
{
    table.count = 0;
}

// Insert Symbol into the Symbol Table
void insert_symbol(int type, const char *lexeme, bool scope, int memory_address)
{
    Symbol *symbol = &table.symbols[table.count++];
    strcpy(symbol->lexeme, lexeme);
    symbol->type = type;
    symbol->scope = scope;
    symbol->memory_address = memory_address;
}

// Find Symbol in the Table
Symbol *find_symbol(const char *lexeme)
{
    for (int i = 0; i < table.count; i++) {
        if (strcmp(table.symbols[i].lexeme, lexeme) == 0) {
            return &table.symbols[i];
        }
    }
    return NULL;
}

// Analyze Expression Semantically
void analyze_expression(Symbol *id, Token *expression, int expression_size, int line)
{
    int types[MAX_TOKENS];
    int type_count = 0;

    for (int i = 0; i < expression_size; i++) {
        int symbol = expression[i].symbol;

        if (symbol == OP_NOT || symbol == OP_POSITIVE || symbol == OP_NEGATIVE) {
            continue;
        }

        if (symbol == OP_AND || symbol == OP_OR) {
            if (types[type_count - 1] != TYPE_BOOLEAN || types[type_count - 2] != TYPE_BOOLEAN) {
                error(line, "Operator must be applied to Boolean.");
            }
            type_count--; // Resulting type is Boolean
        }
        else if (symbol >= OP_GREATER && symbol <= OP_NOT_EQUAL) {
            if (types[type_count - 1] != TYPE_INTEGER || types[type_count - 2] != TYPE_INTEGER) {
                error(line, "Operator must be applied to Integer.");
            }
            type_count -= 2;
            types[type_count++] = TYPE_BOOLEAN; // Resulting type is Boolean
        }
        else if (symbol >= OP_PLUS && symbol <= OP_DIVIDE) {
            if (types[type_count - 1] != TYPE_INTEGER || types[type_count - 2] != TYPE_INTEGER) {
                error(line, "Operator must be applied to Integer.");
            }
            type_count--; // Resulting type is Integer
        }
        else {
            Symbol *s = find_symbol(expression[i].lexeme);
            if (s != NULL) {
                types[type_count++] = s->type;
            }
            else if (symbol == TYPE_INTEGER) {
                types[type_count++] = TYPE_INTEGER;
            }
            else if (symbol == TYPE_BOOLEAN) {
                types[type_count++] = TYPE_BOOLEAN;
            }
        }
    }

    if ((id->type == TYPE_INTEGER && types[0] != TYPE_INTEGER) ||
        (id->type == TYPE_BOOLEAN && types[0] != TYPE_BOOLEAN)) {
        error(line, "Incompatible expression type.");
    }
}

// Convert Infix Expression to Postfix
Token *to_postfix(Token *expression, int expression_size, int *new_size)
{
    Token *postfix = malloc(sizeof(Token) * MAX_TOKENS);
    Token stack[MAX_TOKENS];
    int postfix_index = 0;
    int stack_index = 0;

    for (int i = 0; i < expression_size; i++) {
        int symbol = expression[i].symbol;

        if (symbol == OP_PLUS || symbol == OP_MINUS || symbol == OP_MULTIPLY || symbol == OP_DIVIDE ||
            symbol == OP_AND || symbol == OP_OR || symbol == OP_GREATER || symbol == OP_LESS) {
            while (stack_index > 0 && stack[stack_index - 1].symbol >= symbol) {
                postfix[postfix_index++] = stack[--stack_index];
            }
            stack[stack_index++] = expression[i];
        }
        else if (symbol == OP_OPEN_PARENTHESIS) {
            stack[stack_index++] = expression[i];
        }
        else if (symbol == OP_CLOSE_PARENTHESIS) {
            while (stack[stack_index - 1].symbol != OP_OPEN_PARENTHESIS) {
                postfix[postfix_index++] = stack[--stack_index];
            }
            stack_index--; // Pop the '('
        }
        else {
            postfix[postfix_index++] = expression[i];
        }
    }

    while (stack_index > 0) {
        postfix[postfix_index++] = stack[--stack_index];
    }

    *new_size = postfix_index;
    return postfix;
}

// Handle Errors
void error(int line, const char *msg)
{
    printf("Error on line %d: %s\n", line, msg);
    exit(1);
}

// Example Usage
int main()
{
    initialize_table();

    // Example symbol insertion
    insert_symbol(TYPE_INTEGER, "x", true, 0);
    insert_symbol(TYPE_BOOLEAN, "flag", true, 0);

    // Example expression analysis
    Token expression[] = {
        {TYPE_INTEGER, "x"},
        {OP_PLUS, "+"},
        {TYPE_INTEGER, "10"}};

    Symbol *id = find_symbol("x");
    if (id != NULL) {
        int expression_size = sizeof(expression) / sizeof(expression[0]);
        analyze_expression(id, expression, expression_size, 1);

        int new_size;
        Token *postfix = to_postfix(expression, expression_size, &new_size);

        // Output the postfix expression
        for (int i = 0; i < new_size; i++) {
            printf("Token: %s\n", postfix[i].lexeme);
        }

        free(postfix);
    }

    return 0;
}
