#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    SPROGRAMA,
    SINICIO,
    SFIM,
    SPROCEDIMENTO,
    SFUNCAO,
    SSE,
    SENTAO,
    SSENAO,
    SENQUANTO,
    SFACA,
    SATRIBUICAO,
    SESCREVA,
    SLEIA,
    SVAR,
    SINTEIRO,
    SBOLEANO,
    SIDENTIFICADOR,
    SNUMERO,
    SPONTO,
    SPONTOEVIRGULA,
    SVIRGULA,
    SABREPARENTESES,
    SFECHAPARENTESES,
    SMAIOR,
    SMAIORIG,
    SIG,
    SMENOR,
    SMENORIG,
    SDIF,
    SMAIS,
    SMENOS,
    SMULT,
    SDIV,
    SE,
    SOU,
    SNAO,
    SDOISPONTOS,
    SVERDADEIRO,
    SFALSO
} Symbols;

const char *SymbolNames[] = {
    "SPROGRAMA",
    "SINICIO",
    "SFIM",
    "SPROCEDIMENTO",
    "SFUNCAO",
    "SSE",
    "SENTAO",
    "SSENAO",
    "SENQUANTO",
    "SFACA",
    "SATRIBUICAO",
    "SESCREVA",
    "SLEIA",
    "SVAR",
    "SINTEIRO",
    "SBOLEANO",
    "SIDENTIFICADOR",
    "SNUMERO",
    "SPONTO",
    "SPONTOEVIRGULA",
    "SVIRGULA",
    "SABREPARENTESES",
    "SFECHAPARENTESES",
    "SMAIOR",
    "SMAIORIG",
    "SIG",
    "SMENOR",
    "SMENORIG",
    "SDIF",
    "SMAIS",
    "SMENOS",
    "SMULT",
    "SDIV",
    "SE",
    "SOU",
    "SNAO",
    "SDOISPONTOS",
    "SVERDADEIRO",
    "SFALSO"};

typedef struct
{
    char *lexeme;
    Symbols symbol;
} Token;

typedef struct
{
    FILE *file;
    int line_number;
    char character;
} Lexer;

void exit_error(const char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

Lexer *lexer_init(const char *filename)
{
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) {
        exit_error("Memory allocation failed");
    }

    lexer->file = fopen(filename, "r");
    if (!lexer->file) {
        exit_error("I/O: Error opening the file.");
    }

    lexer->line_number = 0;
    lexer->character = fgetc(lexer->file);
    return lexer;
}

void lexer_destroy(Lexer *lexer)
{
    fclose(lexer->file);
    free(lexer);
}

Token *create_token(const char *lexeme, int symbol)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->lexeme = strdup(lexeme);
    token->symbol = symbol;
    return token;
}

void free_token(Token *token)
{
    free(token->lexeme);
    free(token);
}

Token *lexer_get_token(Lexer *lexer);

Token *lexer_token(Lexer *lexer)
{
    while (lexer->character == '\n' || lexer->character == '\r') {
        lexer->character = fgetc(lexer->file);
        lexer->line_number++;
    }

    if (lexer->character != EOF && lexer->character != '\xff') {
        while (lexer->character == '{' || lexer->character == ' ' || lexer->character == '\b' || lexer->character == '\t') {
            if (lexer->character == '{') {
                while (lexer->character != '}' && lexer->character != EOF && lexer->character != '\xff') {
                    if (lexer->character == '\n' || lexer->character == '\r') {
                        lexer->line_number++;
                    }
                    lexer->character = fgetc(lexer->file);
                }
                lexer->character = fgetc(lexer->file);
            }
            while (lexer->character == ' ' || lexer->character == '\b' || lexer->character == '\t' || lexer->character == '\n' || lexer->character == '\r') {
                if (lexer->character == '\n' || lexer->character == '\r') {
                    lexer->line_number++;
                }
                lexer->character = fgetc(lexer->file);
            }
        }

        if (lexer->character != EOF && lexer->character != '\xff') {
            return lexer_get_token(lexer);
        }
    }

    return NULL;
}

Token *lexer_handle_digit(Lexer *lexer)
{
    char num[32] = {0};
    int idx = 0;

    while (isdigit(lexer->character) && idx < sizeof(num) - 1) {
        num[idx++] = lexer->character;
        lexer->character = fgetc(lexer->file);
    }

    return create_token(num, SNUMERO);
}

Token *lexer_handle_identifier_reserved_word(Lexer *lexer)
{
    char id[64] = {0};
    int idx = 0;

    while ((isalnum(lexer->character) || lexer->character == '_') && idx < sizeof(id) - 1) {
        id[idx++] = lexer->character;
        lexer->character = fgetc(lexer->file);
    }

    if (strcmp(id, "programa") == 0)
        return create_token(id, SPROGRAMA);
    if (strcmp(id, "se") == 0)
        return create_token(id, SSE);
    if (strcmp(id, "entao") == 0)
        return create_token(id, SENTAO);
    if (strcmp(id, "senao") == 0)
        return create_token(id, SSENAO);
    if (strcmp(id, "enquanto") == 0)
        return create_token(id, SENQUANTO);
    if (strcmp(id, "faca") == 0)
        return create_token(id, SFACA);
    if (strcmp(id, "inicio") == 0)
        return create_token(id, SINICIO);
    if (strcmp(id, "fim") == 0)
        return create_token(id, SFIM);
    if (strcmp(id, "escreva") == 0)
        return create_token(id, SESCREVA);
    if (strcmp(id, "leia") == 0)
        return create_token(id, SLEIA);
    if (strcmp(id, "var") == 0)
        return create_token(id, SVAR);
    if (strcmp(id, "inteiro") == 0)
        return create_token(id, SINTEIRO);
    if (strcmp(id, "booleano") == 0)
        return create_token(id, SBOLEANO);
    if (strcmp(id, "verdadeiro") == 0)
        return create_token(id, SVERDADEIRO);
    if (strcmp(id, "falso") == 0)
        return create_token(id, SFALSO);
    if (strcmp(id, "procedimento") == 0)
        return create_token(id, SPROCEDIMENTO);
    if (strcmp(id, "funcao") == 0)
        return create_token(id, SFUNCAO);
    if (strcmp(id, "div") == 0)
        return create_token(id, SDIV);
    if (strcmp(id, "e") == 0)
        return create_token(id, SE);
    if (strcmp(id, "ou") == 0)
        return create_token(id, SOU);
    if (strcmp(id, "nao") == 0)
        return create_token(id, SNAO);

    return create_token(id, SIDENTIFICADOR);
}

Token *lexer_handle_assignment(Lexer *lexer)
{
    char assignment[3] = {0};
    assignment[0] = lexer->character;
    lexer->character = fgetc(lexer->file);

    if (lexer->character != '=') {
        return create_token(assignment, SDOISPONTOS);
    }

    assignment[1] = lexer->character;
    lexer->character = fgetc(lexer->file);
    return create_token(assignment, SATRIBUICAO);
}

Token *lexer_handle_arithmetic_op(Lexer *lexer)
{
    char operator= lexer->character;
    lexer->character = fgetc(lexer->file);

    if (operator== '+')
        return create_token("+", SMAIS);
    if (operator== '-')
        return create_token("-", SMENOS);
    return create_token("*", SMULT);
}

Token *lexer_handle_relational_op(Lexer *lexer)
{
    char relational_op[3] = {0};
    relational_op[0] = lexer->character;
    lexer->character = fgetc(lexer->file);

    if (relational_op[0] == '!') {
        if (lexer->character == '=') {
            relational_op[1] = lexer->character;
            lexer->character = fgetc(lexer->file);
            return create_token(relational_op, SDIF);
        }
        else {
            exit_error("Lexical error: Invalid character '!'.");
        }
    }
    else if (relational_op[0] == '=') {
        return create_token("=", SIG);
    }
    else if (relational_op[0] == '<') {
        if (lexer->character == '=') {
            relational_op[1] = lexer->character;
            lexer->character = fgetc(lexer->file);
            return create_token(relational_op, SMENORIG);
        }
        else {
            return create_token("<", SMENOR);
        }
    }
    else if (relational_op[0] == '>') {
        if (lexer->character == '=') {
            relational_op[1] = lexer->character;
            lexer->character = fgetc(lexer->file);
            return create_token(relational_op, SMAIORIG);
        }
        else {
            return create_token(">", SMAIOR);
        }
    }

    return NULL;
}

Token *lexer_get_token(Lexer *lexer)
{
    if (isdigit(lexer->character)) {
        return lexer_handle_digit(lexer);
    }
    else if (isalpha(lexer->character) || lexer->character == '_') {
        return lexer_handle_identifier_reserved_word(lexer);
    }
    else if (lexer->character == ':') {
        return lexer_handle_assignment(lexer);
    }
    else if (lexer->character == '+' || lexer->character == '-' || lexer->character == '*') {
        return lexer_handle_arithmetic_op(lexer);
    }
    else if (lexer->character == '=' || lexer->character == '!' || lexer->character == '<' || lexer->character == '>') {
        return lexer_handle_relational_op(lexer);
    }
    else if (lexer->character == ';') {
        lexer->character = fgetc(lexer->file);
        return create_token(";", SPONTOEVIRGULA);
    }
    else if (lexer->character == ',') {
        lexer->character = fgetc(lexer->file);
        return create_token(",", SVIRGULA);
    }
    else if (lexer->character == '(') {
        lexer->character = fgetc(lexer->file);
        return create_token("(", SABREPARENTESES);
    }
    else if (lexer->character == ')') {
        lexer->character = fgetc(lexer->file);
        return create_token(")", SFECHAPARENTESES);
    }
    else if (lexer->character == '.') {
        lexer->character = fgetc(lexer->file);
        return create_token(".", SPONTO);
    }
    else {
        exit_error("Lexical error: Unknown character.");
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_error("Usage: ./lexer <source_file>");
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

        fprintf(file, "Token: %-10s | Symbol: %-10s\n", token->lexeme, SymbolNames[token->symbol]);
    }

    if (last_token && last_token->symbol != SPONTO) {
        exit_error("Syntax error: Program should end with a '.'");
    }

    if (last_token) {
        free_token(last_token);
    }

    lexer_destroy(lexer);
    fclose(file);
    return 0;
}
