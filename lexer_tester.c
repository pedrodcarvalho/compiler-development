#include "./lexer.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_error("Usage: ./lexer_tester <source_file>", line_number);
    }

    Lexer *lexer = lexer_init(argv[1]);
    Token *token = NULL;
    Token *last_token = NULL;
    FILE *file = fopen("lexer_output.txt", "w");

    while ((token = lexer_token(lexer)) != NULL) {
        if (last_token) {
            free_token(last_token);
        }
        last_token = token;

        fprintf(file, "Lexeme: %-10s | Symbol: %-10s\n", token->lexeme, SymbolNames[token->symbol]);
    }

    if (last_token && last_token->symbol != SPONTO) {
        exit_error("Syntax error: Program should end with a '.'", line_number);
    }

    if (last_token) {
        free_token(last_token);
    }

    lexer_destroy(lexer);
    fclose(file);
    return 0;
}
