#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "code_generator.h"
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
Token **remove_assignment_from_expression();

Lexer *lexer = NULL;
Token *token = NULL;
CodeGenerator generator;
int current_scope = 1;
int scope_counter = 1;
int label = 1;
int memory_address = 0;
int expression_size = 0;
int parsed_expression_size = 0;
int variables_count = 0;
int aux_memory_address = 0;
int *aux_dealloc = NULL;
int aux_dealloc_current_size = 0;

int zero = 0;
int one = 1;
int if_while_labels = 0;

void parser_init(char *file)
{
    lexer = lexer_init(file);
    initializeCodeGenerator(&generator);
}

void analyze_program()
{
    token = lexer_token(lexer);
    if (token->symbol == SPROGRAMA) {
        generate(&generator, NULL, START, NULL, NULL);
        variables_count = 0;
        token = lexer_token(lexer);
        if (token->symbol == SIDENTIFICADOR) {
            insere_tabela(token->lexeme, PROGRAM_NAME, current_scope, memory_address++);
            token = lexer_token(lexer);
            if (token->symbol == SPONTOEVIRGULA) {
                generate(&generator, NULL, ALLOC, &zero, &one);
                analyze_code_block();
                for (int i = symbols_count; i > 0; i--) {
                    if (symbols_table[i].scope == 1) {
                        variables_count++;
                    }
                }
                generate(&generator, NULL, DALLOC, &one, &variables_count);
                if (token != NULL && token->symbol == SPONTO) {
                    token = lexer_token(lexer);
                    if (lexer->character == EOF || lexer->character == '}') {
                        generate(&generator, NULL, DALLOC, &zero, &one);
                        generate(&generator, NULL, HLT, NULL, NULL);
                        saveCodeToFile(&generator, "output.obj");
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
            aux_memory_address = memory_address;
            while (token->symbol == SIDENTIFICADOR) {
                analyze_variables();
                if (token->symbol == SPONTOEVIRGULA) {
                    token = lexer_token(lexer);
                }
                else {
                    exit_error("Syntax error. Expected ';' token", line_number);
                }
            }
            generate(&generator, NULL, ALLOC, &aux_memory_address, &variables_count);
            aux_dealloc = realloc(aux_dealloc, (aux_dealloc_current_size + 3) * sizeof(int));
            int new_elements[3] = {aux_memory_address, variables_count, current_scope};
            for (int i = 0; i < 3; i++) {
                aux_dealloc[aux_dealloc_current_size + i] = new_elements[i];
            }
            aux_dealloc_current_size += 3;

            variables_count = 0;
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

        variables_count++;
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

    if (expression[0] != NULL && (strcmp(expression[0]->lexeme, "se") == 0 || strcmp(expression[0]->lexeme, "enquanto") == 0)) {
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
        exit_error("Syntax error. Identifier not found!", line_number);
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

    Token **parsed_expression = remove_assignment_from_expression();
    parsed_expression = infix_to_postfix(parsed_expression, parsed_expression_size, &parsed_expression_size);
    generateExpression(&generator, parsed_expression, parsed_expression_size, current_scope);

    if (symbols_table[busca_tabela(expression[0]->lexeme)].type == INTEGER_FUNCTION || symbols_table[busca_tabela(expression[0]->lexeme)].type == BOOLEAN_FUNCTION) {
        generate(&generator, NULL, STR, NULL, &zero);
    }
    else {
        int memory_address = get_memory_address(expression[0]->lexeme, current_scope);
        generate(&generator, NULL, STR, NULL, &memory_address);
    }
}

void analyze_function_call()
{
    generate(&generator, NULL, CALL, NULL, &symbols_table[busca_tabela(token->lexeme)].memory_address);
    token = lexer_token(lexer);
}

void analyze_procedure_call(Token *token)
{
    if (pesquisa_declproc_tabela(token->lexeme) == -1) {
        exit_error("Syntax error. Procedure not found", line_number);
    }
    generate(&generator, NULL, CALL, NULL, &symbols_table[busca_tabela_sem_restricao(token->lexeme)].memory_address);
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

            generate(&generator, NULL, RD, NULL, NULL);
            int memory_address = get_memory_address(token->lexeme, current_scope);
            generate(&generator, NULL, STR, NULL, &memory_address);

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

            if (symbols_table[busca_tabela(token->lexeme)].type == INTEGER_FUNCTION || symbols_table[busca_tabela(token->lexeme)].type == BOOLEAN_FUNCTION) {
                generate(&generator, NULL, LDV, NULL, &zero);
            }
            else {
                int memory_address = get_memory_address(token->lexeme, current_scope);
                generate(&generator, NULL, LDV, NULL, &memory_address);
            }
            generate(&generator, NULL, PRN, NULL, NULL);

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
    int aux_label_1 = label;
    int aux_label_2 = 0;

    generate(&generator, &label, "NULL", NULL, NULL);
    label++;

    expression[expression_size++] = token;
    token = lexer_token(lexer);
    analyze_expression();
    replace_unary();

    Token **parsed_expression = remove_assignment_from_expression();
    parsed_expression = infix_to_postfix(parsed_expression, parsed_expression_size, &parsed_expression_size);
    generateExpression(&generator, parsed_expression, parsed_expression_size, current_scope);

    if (token->symbol == SFACA) {
        aux_label_2 = label;
        generate(&generator, NULL, JMPF, &label, NULL);
        label++;
        if_while_labels++;

        token = lexer_token(lexer);
        analyze_simple_command();

        generate(&generator, NULL, JMP, &aux_label_1, NULL);
        if_while_labels++;
        generate(&generator, &aux_label_2, "NULL", NULL, NULL);
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

    Token **parsed_expression = remove_assignment_from_expression();
    parsed_expression = infix_to_postfix(parsed_expression, parsed_expression_size, &parsed_expression_size);
    generateExpression(&generator, parsed_expression, parsed_expression_size, current_scope);

    generate(&generator, NULL, JMPF, &label, NULL);
    int aux_label = label;
    int aux_label_2 = label;
    label++;
    if_while_labels++;

    if (token->symbol == SENTAO) {
        token = lexer_token(lexer);
        analyze_simple_command();

        if (token->symbol == SSENAO) {
            generate(&generator, NULL, JMP, &label, NULL);
            aux_label_2 = label;
            label++;
            if_while_labels++;
            generate(&generator, &aux_label, "NULL", NULL, NULL);

            token = lexer_token(lexer);
            analyze_simple_command();
        }

        generate(&generator, &aux_label_2, "NULL", NULL, NULL);
    }
    else {
        exit_error("Syntax error. Expected 'entao' token", line_number);
    }
}

void analyze_subroutine()
{
    int aux_label = 0;
    int flag = 0;
    if (token->symbol == SPROCEDIMENTO || token->symbol == SFUNCAO) {
        aux_label = label;
        generate(&generator, NULL, JMP, &label, NULL);
        label++;
        flag = 1;
    }
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

    if (flag) {
        generate(&generator, &aux_label, "NULL", NULL, NULL);
    }
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
        generate(&generator, &label, "NULL", NULL, NULL);
        label++;
        token = lexer_token(lexer);
        if (token->symbol == SPONTOEVIRGULA) {
            analyze_code_block();
            if (aux_dealloc[aux_dealloc_current_size - 1] == current_scope) {
                generate(&generator, NULL, DALLOC, &aux_dealloc[aux_dealloc_current_size - 3], &aux_dealloc[aux_dealloc_current_size - 2]);
                for (int i = 0; i < 3; i++) {
                    int temp = aux_dealloc[aux_dealloc_current_size - 1];
                    for (int j = aux_dealloc_current_size - 1; j > 0; j--) {
                        aux_dealloc[j] = aux_dealloc[j - 1];
                    }
                    aux_dealloc[0] = temp;
                }
            }
            generate(&generator, NULL, RETURN, NULL, NULL);
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
        if (pesquisa_declfunc_tabela(token->lexeme, scope_counter) != -1) {
            exit_error("Syntax error. Duplicated function declaration", line_number);
        }
        insere_tabela(token->lexeme, FUNCTION, scope_counter, label);
        generate(&generator, &label, "NULL", NULL, NULL);
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
                    if (aux_dealloc[aux_dealloc_current_size - 1] == current_scope) {
                        generate(&generator, NULL, DALLOC, &aux_dealloc[aux_dealloc_current_size - 3], &aux_dealloc[aux_dealloc_current_size - 2]);
                        for (int i = 0; i < 3; i++) {
                            int temp = aux_dealloc[aux_dealloc_current_size - 1];
                            for (int j = aux_dealloc_current_size - 1; j > 0; j--) {
                                aux_dealloc[j] = aux_dealloc[j - 1];
                            }
                            aux_dealloc[0] = temp;
                        }
                    }
                    generate(&generator, NULL, RETURNF, NULL, NULL);
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
    if (token->symbol == SIDENTIFICADOR) {
        int indice = busca_tabela(token->lexeme);
        if (indice == -1) {
            exit_error("Syntax error. Identifier not found", line_number);
        }
        if (symbols_table[indice].type == INTEGER_FUNCTION || symbols_table[indice].type == BOOLEAN_FUNCTION) {
            expression[expression_size++] = token;
        }
    }
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
            if (i == 1 || i == 2 || expression[i - 1]->symbol == SABREPARENTESES || is_operator(expression[i - 1]->symbol)) {
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

Token **remove_assignment_from_expression()
{
    Token **new_expression = (Token **)malloc(expression_size * sizeof(Token *));
    parsed_expression_size = expression_size;
    if (expression_size == 1) {
        return expression;
    }
    if (expression[1]->symbol == SATRIBUICAO) {
        for (int i = 2; i < parsed_expression_size; i++) {
            new_expression[i - 2] = expression[i];
        }
        parsed_expression_size -= 2;
    }
    else {
        for (int i = 0; i < parsed_expression_size; i++) {
            new_expression[i] = expression[i];
        }
    }

    return new_expression;
}

#endif // PARSER_H
