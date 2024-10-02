#ifndef SYMBOL_H
#define SYMBOL_H

#include "../constants/commands.h"

typedef struct Symbol {
    int type;
    char *lexeme;
    int scope;
    int memory_address;
} Symbol;

Symbol *create_symbol(int type, const char *lexeme, int scope, const int memory_address)
{
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->type = type;
    symbol->lexeme = strdup(lexeme);
    symbol->scope = scope;
    symbol->memory_address = memory_address;
    return symbol;
}

void symbol_set_memory_address(Symbol *symbol, const int memory_address)
{
    symbol->memory_address = memory_address;
}

char *symbol_set_memory_address_with_label(Symbol *symbol, int label)
{
    char *memory_address = (char *)malloc(10);
    sprintf(memory_address, "%d", label);
    return memory_address;
}

void symbol_set_lexeme(Symbol *symbol, const char *lexeme)
{
    symbol->lexeme = strdup(lexeme);
}

void symbol_set_scope(Symbol *symbol, int scope)
{
    symbol->scope = scope;
}

void symbol_set_type(Symbol *symbol, int type)
{
    symbol->type = type;
}

int symbol_get_type(Symbol *symbol)
{
    return symbol->type;
}

int symbol_get_memory_address(Symbol *symbol)
{
    return symbol->memory_address;
}

const char *symbol_get_lexeme(Symbol *symbol)
{
    return symbol->lexeme;
}

int is_new_scope(Symbol *symbol)
{
    return symbol->scope;
}

#endif // SYMBOL_H
