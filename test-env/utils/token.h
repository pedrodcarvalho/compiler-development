#ifndef TOKEN_H
#define TOKEN_H

typedef struct
{
    char *lexeme;
    int symbol;
} Token;

Token *create_token(const char *lexeme, int symbol)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->lexeme = strdup(lexeme);
    token->symbol = symbol;
    return token;
}

const char *token_get_lexeme(Token *token)
{
    return token->lexeme;
}

void token_set_lexeme(Token *token, const char *lexeme)
{
    token->lexeme = strdup(lexeme);
}

int token_get_symbol(Token *token)
{
    return token->symbol;
}

void token_set_symbol(Token *token, int symbol)
{
    token->symbol = symbol;
}

const char *token_to_string(Token *token)
{
    return SymbolNames[token->symbol];
}

#endif // TOKEN_H
