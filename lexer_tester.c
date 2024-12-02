#include "./lexer.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_error("Usage: ./lexer_tester <source_file>", line_number);
    }

    Lexer *lexer = lexer_init(argv[1]);
    Token_t *token = NULL;
    Token_t *last_token = NULL;
    FILE *file = fopen("lexer_output.txt", "w");

    while ((token = lexer_token(lexer)) != NULL) {
        if (last_token) {
            free_token(last_token);
        }
        last_token = token;

        fprintf(file, "Lexeme: %-10s | Symbol_t: %-10s\n", token->lexeme, symbol_names[token->symbol]);
    }

    if (last_token) {
        free_token(last_token);
    }

    lexer_destroy(lexer);
    fclose(file);
    printf("Successfully wrote the output to lexer_output.txt\n");
    return 0;
}
