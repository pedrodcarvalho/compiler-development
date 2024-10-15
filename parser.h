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
void replace_unary();

Lexer *lexer = NULL;
Token *token = NULL;
int current_scope = 0;
int scope_counter = 0;
int label = 0;
int memory_address = 0;
int expression_size = 0;

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
            insere_tabela(token->lexeme, PROGRAM_NAME, current_scope, memory_address++);
            token = lexer_token(lexer);
            if (token->symbol == SPONTOEVIRGULA) {
                analyze_code_block();
                if (token != NULL && token->symbol == SPONTO) {
                    token = lexer_token(lexer);
                    if (lexer->character == EOF || lexer->character == '}') {
                        // print_table(); // DEBUG
                        // print_expression(expression_size); // DEBUG
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
            if (!verifica_duplicidade(token->lexeme, scope_counter)) {
                insere_tabela(token->lexeme, VARIABLE, scope_counter, memory_address++);
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
    int is_boolean_expression = 0;
    if (token->symbol == SSE || token->symbol == SENQUANTO) {
        is_boolean_expression = 1;
    }

    Token **postfix_expression = infix_to_postfix(expression, expression_size, &expression_size);

    if (expression_size > 0) {
        semantic_analysis(expression[0]->lexeme, postfix_expression, expression_size, is_boolean_expression);
    }

    expression_size = 0;
    for (int i = 0; i < 100; i++) {
        expression[i] = NULL;
    }

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
    Token aux = *token;
    expression[expression_size++] = token;
    if (busca_tabela_sem_restricao(token->lexeme) == -1) {
        printf("%s\n", token->lexeme);
        exit_error("Syntax error. Identifier not found!", line_number); // Add check for var !IMPORTANT
    }
    token = lexer_token(lexer);
    if (token->symbol == SATRIBUICAO) {
        analyze_assignment();
    }
    else {
        expression[expression_size--] = NULL;
        analyze_procedure_call(&aux);
    }
}

void analyze_assignment()
{
    expression[expression_size++] = token;
    token = lexer_token(lexer);
    analyze_expression();
    replace_unary();
}

void analyze_function_call()
{
    token = lexer_token(lexer);
}

void analyze_procedure_call(Token *token)
{
    if (pesquisa_declproc_tabela(token->lexeme) == -1) {
        exit_error("Syntax error. Procedure not found", line_number);
    }
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
    expression[expression_size++] = token;
    token = lexer_token(lexer);
    analyze_expression();
    replace_unary();
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
    expression[expression_size++] = token;
    token = lexer_token(lexer);
    analyze_expression();
    replace_unary();
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
    // current_scope++;
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
    // current_scope--;
}

void analyze_procedure_declaration()
{
    token = lexer_token(lexer);
    current_scope++;
    scope_counter++;
    if (token->symbol == SIDENTIFICADOR) {
        if (pesquisa_declproc_tabela(token->lexeme) != -1) {
            exit_error("Syntax error. Duplicated procedure declaration", line_number);
        }
        insere_tabela(token->lexeme, PROCEDURE, scope_counter, label);
        label++;
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
    current_scope--;
}

void analyze_function_declaration()
{
    token = lexer_token(lexer);
    current_scope++;
    scope_counter++;
    if (token->symbol == SIDENTIFICADOR) {
        if (pesquisa_declfunc_tabela(token->lexeme) != -1) {
            exit_error("Syntax error. Duplicated function declaration", line_number);
        }
        insere_tabela(token->lexeme, FUNCTION, scope_counter, label);
        label++;
        token = lexer_token(lexer);
        if (token->symbol == SDOISPONTOS) {
            token = lexer_token(lexer);
            if ((token->symbol == SINTEIRO) || (token->symbol == SBOLEANO)) {
                if (token->symbol == SINTEIRO) {
                    symbols_table[symbols_count - 1].type = INTEGER_FUNCTION;
                }
                else {
                    symbols_table[symbols_count - 1].type = BOOLEAN_FUNCTION;
                }
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
    current_scope--;
}

void analyze_expression()
{
    analyze_simple_expression();
    if ((token->symbol == SMAIOR) || (token->symbol == SMAIORIG) || (token->symbol == SIG) || (token->symbol == SMENOR) || (token->symbol == SMENORIG) || (token->symbol == SDIF)) {
        expression[expression_size++] = token;
        token = lexer_token(lexer);
        analyze_simple_expression();
    }
}

void analyze_simple_expression()
{
    if ((token->symbol == SMAIS) || (token->symbol == SMENOS)) {
        expression[expression_size++] = token;
        token = lexer_token(lexer);
    }
    analyze_term();
    while ((token->symbol == SMAIS) || (token->symbol == SMENOS) || (token->symbol == SOU)) {
        expression[expression_size++] = token;
        token = lexer_token(lexer);
        analyze_term();
    }
}

void analyze_term()
{
    analyze_factor();
    while ((token->symbol == SMULT) || (token->symbol == SDIV) || (token->symbol == SE)) {
        expression[expression_size++] = token;
        token = lexer_token(lexer);
        analyze_factor();
    }
}

void analyze_factor()
{
    if (token->symbol == SIDENTIFICADOR) {
        int indice = busca_tabela(token->lexeme);
        if (indice == -1) {
            exit_error("Syntax error. Identifier not found", line_number);
        }
        if (symbols_table[indice].type == INTEGER_FUNCTION || symbols_table[indice].type == BOOLEAN_FUNCTION) {
            analyze_function_call();
        }
        else {
            expression[expression_size++] = token;
            token = lexer_token(lexer);
        }
    }
    else if (token->symbol == SNUMERO) {
        expression[expression_size++] = token;
        token = lexer_token(lexer);
    }
    else if (token->symbol == SNAO) {
        expression[expression_size++] = token;
        token = lexer_token(lexer);
        analyze_factor();
    }
    else if (token->symbol == SABREPARENTESES) {
        expression[expression_size++] = token;
        token = lexer_token(lexer);
        analyze_expression();
        if (token->symbol == SFECHAPARENTESES) {
            expression[expression_size++] = token;
            token = lexer_token(lexer);
        }
        else {
            exit_error("Syntax error. Expected ')' token", line_number);
        }
    }
    else if (token->symbol == SVERDADEIRO || token->symbol == SFALSO) {
        expression[expression_size++] = token;
        token = lexer_token(lexer);
    }
    else {
        exit_error("Syntax error. Expected an identifier, a number or 'nao', '(', 'verdadeiro', 'falso' token", line_number);
    }
}

void replace_unary()
{
    for (int i = 0; i < expression_size; i++) {
        if (expression[i]->symbol == SMAIS || expression[i]->symbol == SMENOS) {
            if (i == 0 || expression[i - 1]->symbol == SABREPARENTESES || is_operator(expression[i - 1]->symbol)) {
                if (expression[i]->symbol == SMAIS) {
                    expression[i]->symbol = SPOSITIVO;
                }
                else {
                    expression[i]->symbol = SNEGATIVO;
                }
            }
        }
    }
}

#endif /* PARSER_H */
