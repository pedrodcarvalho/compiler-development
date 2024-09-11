#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../constants/types.h"
#include "./symbol.h"

typedef struct SymbolTable {
    Symbol **symbols;
    int size;
    int capacity;
} SymbolTable;

SymbolTable *create_symbol_table()
{
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    table->size = 0;
    table->capacity = 10;
    table->symbols = (Symbol **)malloc(table->capacity * sizeof(Symbol *));
    return table;
}

void free_symbol(Symbol *symbol)
{
    free(symbol->lexeme);
    if (symbol->memory_address != NULL) {
        free(symbol->memory_address);
    }
    free(symbol);
}

void free_symbol_table(SymbolTable *table)
{
    for (int i = 0; i < table->size; i++) {
        free_symbol(table->symbols[i]);
    }
    free(table->symbols);
    free(table);
}

void resize_symbol_table(SymbolTable *table)
{
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->symbols = (Symbol **)realloc(table->symbols, table->capacity * sizeof(Symbol *));
    }
}

void insert_symbol(SymbolTable *table, int type, const char *lexeme, int scope, const char *memory_address)
{
    if (table->size >= table->capacity) {
        resize_symbol_table(table);
    }

    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->type = type;
    symbol->lexeme = strdup(lexeme);
    symbol->scope = scope;
    memory_address ? symbol->memory_address = strdup(memory_address) : (symbol->memory_address = NULL);
    table->symbols[table->size++] = symbol;
}

int find_symbol_index(SymbolTable *table, const char *lexeme)
{
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i]->lexeme, lexeme) == 0) {
            return i;
        }
    }
    return -1;
}

Symbol *find_symbol(SymbolTable *table, const char *lexeme)
{
    int index = find_symbol_index(table, lexeme);
    if (index != -1) {
        return table->symbols[index];
    }
    return NULL;
}

void remove_symbol(SymbolTable *table, const char *lexeme)
{
    int index = find_symbol_index(table, lexeme);
    if (index != -1) {
        free(table->symbols[index]);
        for (int i = index; i < table->size - 1; i++) {
            table->symbols[i] = table->symbols[i + 1];
        }
        table->size--;
    }
}

void clear_scope(SymbolTable *table)
{
    int i = table->size;

    while (i > 0 && !(table->symbols[i - 1]->scope)) {
        i--;
    }
    if (i > 0) {
        table->symbols[i - 1]->scope = 0;
    }
}

void update_symbol_type(SymbolTable *table, int type)
{
    for (int i = 0; i < table->size; i++) {
        Symbol *symbol = table->symbols[i];
        if (symbol->type == VARIABLE && (type == INTEGER || type == BOOLEAN)) {
            symbol->type = type;
        }
        if (symbol->type == FUNCTION && (type == INTEGER_FUNCTION || type == BOOLEAN_FUNCTION)) {
            symbol->type = type;
        }
    }
}

Symbol **get_variables(SymbolTable *table, int *count)
{
    Symbol **variables = (Symbol **)malloc(table->size * sizeof(Symbol *));
    *count = 0;

    for (int i = table->size - 1; i >= 0 & !(table->symbols[i]->scope); i--) {
        if (table->symbols[i]->type == INTEGER || table->symbols[i]->type == BOOLEAN) {
            variables[(*count)++] = table->symbols[i];
            (*count)++;
        }
    }

    return variables;
}

int is_declared_in_scope(SymbolTable *table, const char *lexeme)
{
    for (int i = table->size; i > 0 && table->symbols[i - 1]->scope; i--) {
        if (strcmp(table->symbols[i - 1]->lexeme, lexeme) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_declared(SymbolTable *table, const char *lexeme)
{
    for (int i = table->size; i > 0 && !(table->symbols[i - 1]->scope); i--) {
        if (strcmp(table->symbols[i - 1]->lexeme, lexeme) == 0) {
            return 1;
        }
    }
    return 0;
}

#endif // SYMBOL_TABLE_H
