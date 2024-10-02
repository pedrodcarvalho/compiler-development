#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "./utils/symbol_table.h"
#include "./utils/exception.h"

void analyze_program();
void analyze_code_block();
void analyze_variables_step();
void analyze_variables();
void analyze_types();
void analyze_commands();
void analyze_simple_command();
void analyze_procedure_call_assignment();
void analyze_assignment();
void analyze_function_call();
void analyze_procedure_call();
void analyze_read();
void analyze_write();
void analyze_while();
void analyze_if();
void analyze_subroutine();
void analyze_procedure_declaration();
void analyze_function_declaration();
void analyze_expression();
void analyze_simple_expression();
void analyze_term();
void analyze_factor();

Lexer *lexer = NULL;
Token *token = NULL;
int current_scope = 0;
int memory_address = 0;

void parser_init(char *file)
{
    lexer = lexer_init(file);
}

void analyze_program()
{
    token = lexer_token(lexer);
    if (token->symbol == SPROGRAMA) {
        token = lexer_token(lexer);
        if (token->symbol == SIDENTIFICADOR) {
            token = lexer_token(lexer);
            if (token->symbol == SPONTOEVIRGULA) {
                analyze_code_block();
                if (token != NULL && token->symbol == SPONTO) {
                    token = lexer_token(lexer);
                    if (lexer->character == EOF || lexer->character == '}') {
                        printf("Success. No syntax errors found.\n");
                    }
                    else {
                        exit_error("Syntax error. Expected EOF", line_number);
                    }
                }
                else {
                    exit_error("Syntax error. Expected '.' token", line_number);
                }
            }
            else {
                exit_error("Syntax error. Expected ';' token", line_number);
            }
        }
        else {
            exit_error("Syntax error. Expected an identifier", line_number);
        }
    }
    else {
        exit_error("Syntax error. Expected 'programa' token", line_number);
    }
}

void analyze_code_block()
{
    token = lexer_token(lexer);
    analyze_variables_step();
    analyze_subroutine();
    analyze_commands();
}

void analyze_variables_step()
{
    if (token->symbol == SVAR) {
        token = lexer_token(lexer);
        if (token->symbol == SIDENTIFICADOR) {
            while (token->symbol == SIDENTIFICADOR) {
                analyze_variables();
                if (token->symbol == SPONTOEVIRGULA) {
                    token = lexer_token(lexer);
                }
                else {
                    exit_error("Syntax error. Expected ';' token", line_number);
                }
            }
        }
        else {
            exit_error("Syntax error. Expected an identifier", line_number);
        }
    }
}

void analyze_variables()
{
    do {
        if (token->symbol == SIDENTIFICADOR) {
            if (!verifica_duplicidade(token->lexeme, current_scope)) {
                insere_tabela(token->lexeme, SVAR, current_scope, memory_address++);
            }
            else {
                exit_error("Syntax error. Duplicated identifier", line_number);
            }
            token = lexer_token(lexer);
            if ((token->symbol == SVIRGULA) || (token->symbol == SDOISPONTOS)) {
                if (token->symbol == SVIRGULA) {
                    token = lexer_token(lexer);
                    if (token->symbol == SDOISPONTOS) {
                        exit_error("Syntax error. Expected ':' token", line_number);
                    }
                }
            }
            else {
                exit_error("Syntax error. Expected ',' or ':' token", line_number);
            }
        }
        else {
            exit_error("Syntax error. Expected an identifier", line_number);
        }
    } while (token->symbol != SDOISPONTOS);
    token = lexer_token(lexer);
    analyze_types();
}

void analyze_types()
{
    if ((token->symbol != SINTEIRO) && (token->symbol != SBOLEANO)) {
        exit_error("Syntax error. Expected and integer or a boolean value", line_number);
    }
    else {
        for (int i = symbols_count - 1; i >= 0; i--) {
            if (symbols_table[i].type == VARIABLE) {
                atualiza_tipo_tabela(symbols_table[i].lexeme, (token->symbol == SINTEIRO) ? INTEGER : BOOLEAN);
            }
        }
        token = lexer_token(lexer);
    }
}

void analyze_commands()
{
    if (token->symbol == SINICIO) {
        token = lexer_token(lexer);
        analyze_simple_command();
        while (token->symbol != SFIM) {
            if (token->symbol == SPONTOEVIRGULA) {
                token = lexer_token(lexer);
                if (token->symbol != SFIM) {
                    analyze_simple_command();
                }
                else {
                    break;
                }
            }
            else {
                exit_error("Syntax error. Expected ';' token", line_number);
            }
        }
        token = lexer_token(lexer);
    }
    else {
        exit_error("Syntax error. Expected 'inicio' token", line_number);
    }
}

void analyze_simple_command()
{
    if (token->symbol == SIDENTIFICADOR) {
        analyze_procedure_call_assignment();
    }
    else if (token->symbol == SSE) {
        analyze_if();
    }
    else if (token->symbol == SENQUANTO) {
        analyze_while();
    }
    else if (token->symbol == SLEIA) {
        analyze_read();
    }
    else if (token->symbol == SESCREVA) {
        analyze_write();
    }
    else {
        analyze_commands();
    }
}

void analyze_procedure_call_assignment()
{
    token = lexer_token(lexer);
    if (token->symbol == SATRIBUICAO) {
        analyze_assignment();
    }
    else {
        analyze_procedure_call();
    }
}

void analyze_assignment()
{
    token = lexer_token(lexer);
    analyze_expression();
}

void analyze_function_call()
{
    token = lexer_token(lexer);
}

void analyze_procedure_call()
{
    // TODO: Code Generation
}

void analyze_read()
{
    token = lexer_token(lexer);
    if (token->symbol == SABREPARENTESES) {
        token = lexer_token(lexer);
        if (token->symbol == SIDENTIFICADOR) {
            if (busca_tabela(token->lexeme) == -1) {
                exit_error("Syntax error. Identifier not found", line_number);
            }
            token = lexer_token(lexer);
            if (token->symbol == SFECHAPARENTESES) {
                token = lexer_token(lexer);
            }
            else {
                exit_error("Syntax error. Expected ')' token", line_number);
            }
        }
        else {
            exit_error("Syntax error. Expected an identifier", line_number);
        }
    }
    else {
        exit_error("Syntax error. Expected '(' token", line_number);
    }
}

void analyze_write()
{
    token = lexer_token(lexer);
    if (token->symbol == SABREPARENTESES) {
        token = lexer_token(lexer);
        if (token->symbol == SIDENTIFICADOR) {
            if (busca_tabela(token->lexeme) == -1) {
                exit_error("Syntax error. Identifier not found", line_number);
            }
            token = lexer_token(lexer);
            if (token->symbol == SFECHAPARENTESES) {
                token = lexer_token(lexer);
            }
            else {
                exit_error("Syntax error. Expected ')' token", line_number);
            }
        }
        else {
            exit_error("Syntax error. Expected an identifier", line_number);
        }
    }
    else {
        exit_error("Syntax error. Expected '(' token", line_number);
    }
}

void analyze_while()
{
    token = lexer_token(lexer);
    analyze_expression();
    if (token->symbol == SFACA) {
        token = lexer_token(lexer);
        analyze_simple_command();
    }
    else {
        exit_error("Syntax error. Expected 'faca' token", line_number);
    }
}

void analyze_if()
{
    token = lexer_token(lexer);
    analyze_expression();
    if (token->symbol == SENTAO) {
        token = lexer_token(lexer);
        analyze_simple_command();
        if (token->symbol == SSENAO) {
            token = lexer_token(lexer);
            analyze_simple_command();
        }
    }
    else {
        exit_error("Syntax error. Expected 'entao' token", line_number);
    }
}

void analyze_subroutine()
{
    current_scope++;
    while ((token->symbol == SPROCEDIMENTO) || (token->symbol == SFUNCAO)) {
        if (token->symbol == SPROCEDIMENTO) {
            analyze_procedure_declaration();
        }
        else {
            analyze_function_declaration();
        }
        if (token->symbol == SPONTOEVIRGULA) {
            token = lexer_token(lexer);
        }
        else {
            exit_error("Syntax error. Expected ';' token", line_number);
        }
    }
    current_scope--;
}

void analyze_procedure_declaration()
{
    token = lexer_token(lexer);
    if (token->symbol == SIDENTIFICADOR) {
        token = lexer_token(lexer);
        if (token->symbol == SPONTOEVIRGULA) {
            analyze_code_block();
        }
        else {
            exit_error("Syntax error. Expected ';' token", line_number);
        }
    }
    else {
        exit_error("Syntax error. Expected an identifier", line_number);
    }
}

void analyze_function_declaration()
{
    token = lexer_token(lexer);
    if (token->symbol == SIDENTIFICADOR) {
        token = lexer_token(lexer);
        if (token->symbol == SDOISPONTOS) {
            token = lexer_token(lexer);
            if ((token->symbol == SINTEIRO) || (token->symbol == SBOLEANO)) {
                token = lexer_token(lexer);
                if (token->symbol == SPONTOEVIRGULA) {
                    analyze_code_block();
                }
                else {
                    exit_error("Syntax error. Expected ';' token", line_number);
                }
            }
            else {
                exit_error("Syntax error. Expected and integer or a boolean value", line_number);
            }
        }
        else {
            exit_error("Syntax error. Expected ':' token", line_number);
        }
    }
    else {
        exit_error("Syntax error. Expected an identifier", line_number);
    }
}

void analyze_expression()
{
    analyze_simple_expression();
    if ((token->symbol == SMAIOR) || (token->symbol == SMAIORIG) || (token->symbol == SIG) || (token->symbol == SMENOR) || (token->symbol == SMENORIG) || (token->symbol == SDIF)) {
        token = lexer_token(lexer);
        analyze_simple_expression();
    }
}

void analyze_simple_expression()
{
    if ((token->symbol == SMAIS) || (token->symbol == SMENOS)) {
        token = lexer_token(lexer);
    }
    analyze_term();
    while ((token->symbol == SMAIS) || (token->symbol == SMENOS) || (token->symbol == SOU)) {
        token = lexer_token(lexer);
        analyze_term();
    }
}

void analyze_term()
{
    analyze_factor();
    while ((token->symbol == SMULT) || (token->symbol == SDIV) || (token->symbol == SE)) {
        token = lexer_token(lexer);
        analyze_factor();
    }
}

void analyze_factor()
{
    if (token->symbol == SIDENTIFICADOR) {
        token = lexer_token(lexer);
    }
    else if (token->symbol == SNUMERO) {
        token = lexer_token(lexer);
    }
    else if (token->symbol == SNAO) {
        token = lexer_token(lexer);
        analyze_factor();
    }
    else if (token->symbol == SABREPARENTESES) {
        token = lexer_token(lexer);
        analyze_expression();
        if (token->symbol == SFECHAPARENTESES) {
            token = lexer_token(lexer);
        }
        else {
            exit_error("Syntax error. Expected ')' token", line_number);
        }
    }
    else if (token->symbol == SVERDADEIRO || token->symbol == SFALSO) {
        token = lexer_token(lexer);
    }
    else {
        exit_error("Syntax error. Expected an identifier, a number or 'nao', '(', 'verdadeiro', 'falso' token", line_number);
    }
}

#endif /* PARSER_H */
