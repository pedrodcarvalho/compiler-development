#ifndef TOKEN_H
#define TOKEN_H

typedef struct
{
    char *lexeme;
    int symbol;
} Token_t;

Token_t *create_token(const char *lexeme, int symbol)
{
    Token_t *token = (Token_t *)malloc(sizeof(Token_t));
    token->lexeme = strdup(lexeme);
    token->symbol = symbol;
    return token;
}

#endif // TOKEN_H
