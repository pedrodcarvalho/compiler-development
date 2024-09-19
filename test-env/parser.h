#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "./utils/exception.h"

void analisa_programa();
void analisa_bloco();
void analisa_et_variaveis();
void analisa_variaveis();
void analisa_tipo();
void analisa_comandos();
void analisa_comando_simples();
void analisa_atrib_chprocedimento();
void analisa_atribuicao();
void chamada_procedimento();
void analisa_leia();
void analisa_escreva();
void analisa_enquanto();
void analisa_se();
void analisa_subrotinas();
void analisa_declara_procedimento();
void analisa_declara_funcao();
void analisa_expressao();
void analisa_expressao_simples();
void analisa_termo();
void analisa_fator();
void analisa_chamada_funcao();

Lexer *lexer = NULL;
Token *token = NULL;

void parser_init(char *file)
{
    lexer = lexer_init(file);
}

void analisa_programa()
{
    token = lexer_token(lexer);
    if (token->symbol == SPROGRAMA) {
        token = lexer_token(lexer);
        if (token->symbol == SIDENTIFICADOR) {
            token = lexer_token(lexer);
            if (token->symbol == SPONTOEVIRGULA) {
                analisa_bloco();
                if (token->symbol == SPONTO) {
                    token = lexer_token(lexer);
                    if (lexer->character == EOF || lexer->character == '}') {
                        printf("Sucesso\n");
                    }
                    else {
                        exit_error("Syntax error, code after 'ponto' token", line_number);
                    }
                }
                else {
                    exit_error("Sytax error, expected 'ponto' token", line_number);
                }
            }
            else {
                exit_error("Syntax error, expected 'ponto e virgula' token 1", line_number);
            }
        }
        else {
            exit_error("Syntax error, expected 'identificador' token", line_number);
        }
    }
    else {
        exit_error("Syntax error, expected 'programa' token", line_number);
    }
}

void analisa_bloco()
{
    token = lexer_token(lexer);
    analisa_et_variaveis();
    analisa_subrotinas();
    analisa_comandos();
}

void analisa_et_variaveis()
{
    if (token->symbol == SVAR) {
        token = lexer_token(lexer);
        if (token->symbol == SIDENTIFICADOR) {
            while (token->symbol == SIDENTIFICADOR) {
                analisa_variaveis();
                if (token->symbol == SPONTOEVIRGULA) {
                    token = lexer_token(lexer);
                }
                else {
                    exit_error("Syntax error, expected 'ponto e virgula' token 2", line_number);
                }
            }
        }
        else {
            exit_error("Syntax error, expected 'identificador' token", line_number);
        }
    }
}

void analisa_variaveis()
{
    do {
        if (token->symbol == SIDENTIFICADOR) {
            token = lexer_token(lexer);
            if ((token->symbol == SVIRGULA) || (token->symbol == SDOISPONTOS)) {
                if (token->symbol == SVIRGULA) {
                    token = lexer_token(lexer);
                    if (token->symbol == SDOISPONTOS) {
                        exit_error("Syntax error, expected 'dois pontos' token", line_number);
                    }
                }
            }
            else {
                exit_error("Syntax error, expected 'virgula ou dois pontos' token", line_number);
            }
        }
        else {
            exit_error("Syntax error, expected 'identificador' token", line_number);
        }
    } while (token->symbol != SDOISPONTOS);
    token = lexer_token(lexer);
    analisa_tipo();
}

void analisa_tipo()
{
    if ((token->symbol != SINTEIRO) && (token->symbol != SBOLEANO)) {
        exit_error("Syntax error, expected 'inteiro ou boleano' token", line_number);
    }
    else {
        token = lexer_token(lexer);
    }
}

void analisa_comandos()
{
    if (token->symbol == SINICIO) {
        token = lexer_token(lexer);
        analisa_comando_simples();
        while (token->symbol != SFIM) {
            if (token->symbol == SPONTOEVIRGULA) {
                token = lexer_token(lexer);
                if (token->symbol != SFIM) {
                    analisa_comando_simples();
                }
                else {
                    break;
                }
            }
            else {
                printf("%d\n", token->symbol);
                printf("%s\n", token->lexeme);
                exit_error("Syntax error, expected 'ponto e virgula' token 3", line_number);
            }
        }
        token = lexer_token(lexer);
    }
    // ADICIONADO MANUALMENTE (PODE ESTAR ERRADO)
    else if (token->symbol == SSENAO) {
        token = lexer_token(lexer);
        analisa_comando_simples();
    }
    else {
        exit_error("Syntax error, expected 'inicio' token", line_number);
    }
}

void analisa_comando_simples()
{
    if (token->symbol == SIDENTIFICADOR) {
        analisa_atrib_chprocedimento();
    }
    else if (token->symbol == SSE) {
        analisa_se();
    }
    else if (token->symbol == SENQUANTO) {
        analisa_enquanto();
    }
    else if (token->symbol == SLEIA) {
        analisa_leia();
    }
    else if (token->symbol == SESCREVA) {
        analisa_escreva();
    }
    else {
        analisa_comandos();
    }
}

void analisa_atrib_chprocedimento()
{
    token = lexer_token(lexer);
    if (token->symbol == SATRIBUICAO) {
        analisa_atribuicao();
    }
    else {
        chamada_procedimento();
    }
}

void analisa_atribuicao()
{
    token = lexer_token(lexer);
    analisa_expressao();
}

void chamada_procedimento()
{
    // token = lexer_token(lexer);
    // if (token->symbol == SABREPARENTESES) {
    //     token = lexer_token(lexer);
    //     if (token->symbol == SIDENTIFICADOR) {
    //         token = lexer_token(lexer);
    //         if (token->symbol == SFECHAPARENTESES) {
    //             token = lexer_token(lexer);
    //         }
    //         else {
    //             exit_error("Syntax error, expected 'fecha parenteses' token", line_number);
    //         }
    //     }
    //     else {
    //         exit_error("Syntax error, expected 'identificador' token", line_number);
    //     }
    // }
    // else {
    //     exit_error("Syntax error, expected 'abre parenteses' token 1", line_number);
    // }
}

void analisa_leia()
{
    token = lexer_token(lexer);
    if (token->symbol == SABREPARENTESES) {
        token = lexer_token(lexer);
        if (token->symbol == SIDENTIFICADOR) {
            token = lexer_token(lexer);
            if (token->symbol == SFECHAPARENTESES) {
                token = lexer_token(lexer);
            }
            else {
                exit_error("Syntax error, expected 'fecha parenteses' token", line_number);
            }
        }
        else {
            exit_error("Syntax error, expected 'identificador' token", line_number);
        }
    }
    else {
        exit_error("Syntax error, expected 'abre parenteses' token 2", line_number);
    }
}

void analisa_escreva()
{
    token = lexer_token(lexer);
    if (token->symbol == SABREPARENTESES) {
        token = lexer_token(lexer);
        if (token->symbol == SIDENTIFICADOR) {
            token = lexer_token(lexer);
            if (token->symbol == SFECHAPARENTESES) {
                token = lexer_token(lexer);
            }
            else {
                exit_error("Syntax error, expected 'fecha parenteses' token", line_number);
            }
        }
        else {
            exit_error("Syntax error, expected 'identificador' token", line_number);
        }
    }
    else {
        exit_error("Syntax error, expected 'abre parenteses' token 3", line_number);
    }
}

void analisa_enquanto()
{
    token = lexer_token(lexer);
    analisa_expressao();
    if (token->symbol == SFACA) {
        token = lexer_token(lexer);
        analisa_comando_simples();
    }
    else {
        exit_error("Syntax error, expected 'faca' token", line_number);
    }
}

void analisa_se()
{
    token = lexer_token(lexer);
    analisa_expressao();
    if (token->symbol == SENTAO) {
        token = lexer_token(lexer);
        analisa_comando_simples();
        if (token->symbol == SENTAO) {
            token = lexer_token(lexer);
            analisa_comando_simples();
        }
    }
    else {
        exit_error("Syntax error, expected 'entao' token", line_number);
    }
}

void analisa_subrotinas()
{
    while ((token->symbol == SPROCEDIMENTO) || (token->symbol == SFUNCAO)) {
        if (token->symbol == SPROCEDIMENTO) {
            analisa_declara_procedimento();
        }
        else {
            analisa_declara_funcao();
        }
        if (token->symbol == SPONTOEVIRGULA) {
            token = lexer_token(lexer);
        }
        else {
            exit_error("Syntax error, expected 'ponto e virgula' token 4", line_number);
        }
    }
}

void analisa_declara_procedimento()
{
    token = lexer_token(lexer);
    if (token->symbol == SIDENTIFICADOR) {
        token = lexer_token(lexer);
        if (token->symbol == SPONTOEVIRGULA) {
            analisa_bloco();
        }
        else {
            exit_error("Syntax error, expected 'ponto e virgula' token 5", line_number);
        }
    }
    else {
        exit_error("Syntax error, expected 'identificador' token", line_number);
    }
}

void analisa_declara_funcao()
{
    token = lexer_token(lexer);
    if (token->symbol == SIDENTIFICADOR) {
        token = lexer_token(lexer);
        if (token->symbol == SDOISPONTOS) {
            token = lexer_token(lexer);
            if ((token->symbol == SINTEIRO) || (token->symbol == SBOLEANO)) {
                token = lexer_token(lexer);
                if (token->symbol == SPONTOEVIRGULA) {
                    analisa_bloco();
                }
                else {
                    exit_error("Syntax error, expected 'ponto e virgula' token 6", line_number);
                }
            }
            else {
                exit_error("Syntax error, expected 'inteiro ou boleano' token", line_number);
            }
        }
        else {
            exit_error("Syntax error, expected 'dois pontos' token", line_number);
        }
    }
    else {
        exit_error("Syntax error, expected 'identificador' token", line_number);
    }
}

void analisa_expressao()
{
    analisa_expressao_simples();
    if ((token->symbol == SMAIOR) || (token->symbol == SMAIORIG) || (token->symbol == SIG) || (token->symbol == SMENOR) || (token->symbol == SMENORIG) || (token->symbol == SDIF)) {
        token = lexer_token(lexer);
        analisa_expressao_simples();
    }
}

void analisa_expressao_simples()
{
    if ((token->symbol == SMAIS) || (token->symbol == SMENOS)) {
        token = lexer_token(lexer);
    }
    analisa_termo();
    while ((token->symbol == SMAIS) || (token->symbol == SMENOS) || (token->symbol == SOU)) {
        token = lexer_token(lexer);
        analisa_termo();
    }
}

void analisa_termo()
{
    analisa_fator();
    while ((token->symbol == SMULT) || (token->symbol == SDIV) || (token->symbol == SE)) {
        token = lexer_token(lexer);
        analisa_fator();
    }
}

void analisa_fator()
{
    if (token->symbol == SIDENTIFICADOR) {
        analisa_chamada_funcao();
    }
    else if (token->symbol == SNUMERO) {
        token = lexer_token(lexer);
    }
    else if (token->symbol == SNAO) {
        token = lexer_token(lexer);
        analisa_fator();
    }
    else if (token->symbol == SABREPARENTESES) {
        token = lexer_token(lexer);
        analisa_expressao();
        if (token->symbol == SFECHAPARENTESES) {
            token = lexer_token(lexer);
        }
        else {
            exit_error("Syntax error, expected 'fecha parenteses' token", line_number);
        }
    }
    else if (token->symbol == SVERDADEIRO || token->symbol == SFALSO) {
        token = lexer_token(lexer);
    }
    else {
        exit_error("Syntax error, expected 'identificador, numero, nao, abre parenteses, verdadeiro ou falso' token", line_number);
    }
}

void analisa_chamada_funcao()
{
    token = lexer_token(lexer);
    // if (token->symbol == SABREPARENTESES) {
    //     token = lexer_token(lexer);
    //     if (token->symbol == SIDENTIFICADOR) {
    //         token = lexer_token(lexer);
    //         if (token->symbol == SFECHAPARENTESES) {
    //             token = lexer_token(lexer);
    //         }
    //         else {
    //             exit_error("Syntax error, expected 'fecha parenteses' token", line_number);
    //         }
    //     }
    //     else {
    //         exit_error("Syntax error, expected 'identificador' token", line_number);
    //     }
    // }
    // else {
    //     exit_error("Syntax error, expected 'abre parenteses' token 4", line_number);
    // }
}

#endif /* PARSER_H */
