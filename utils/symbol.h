#ifndef SYMBOL_H
#define SYMBOL_H

typedef struct Symbol_t {
    int type;
    char *lexeme;
    int scope;
    int memory_address;
} Symbol_t;

Symbol_t *create_symbol(int type, const char *lexeme, int scope, const int memory_address)
{
    Symbol_t *symbol = (Symbol_t *)malloc(sizeof(Symbol_t));
    symbol->type = type;
    symbol->lexeme = strdup(lexeme);
    symbol->scope = scope;
    symbol->memory_address = memory_address;
    return symbol;
}

#endif // SYMBOL_H
