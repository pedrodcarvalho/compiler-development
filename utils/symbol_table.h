#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../constants/types.h"
#include "./symbol.h"

#define MAX_SYMBOLS 100

Symbol symbols_table[MAX_SYMBOLS];
int symbols_count = 0;

void insere_tabela(char *lexeme, int type, int scope, int memory_address)
{
    if (symbols_count < MAX_SYMBOLS) {
        symbols_table[symbols_count].lexeme = strdup(lexeme);
        symbols_table[symbols_count].type = type;
        symbols_table[symbols_count].scope = scope;
        symbols_table[symbols_count].memory_address = memory_address;
        symbols_count++;
    }
    else {
        printf("Erro: Tabela de símbolos cheia!\n");
    }
}

int busca_tabela(char *lexeme)
{
    for (int i = symbols_count - 1; i >= 0; i--) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type != PROGRAM_NAME && symbols_table[i].type != PROCEDURE) {
            return i;
        }
    }
    return -1;
}

int verifica_duplicidade(char *lexeme, int scope)
{
    for (int i = symbols_count - 1; i >= 0; i--) {
        printf("Comparando %s com %s\n", symbols_table[i].lexeme, lexeme);
        printf("Comparando %d com %d\n", symbols_table[i].scope, scope);
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].scope == scope) {
            return 1; // Encontrou duplicidade
        }
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type == PROGRAM_NAME) {
            return 1; // Variável com mesmo nome do programa
        }
    }
    return 0; // Não encontrou duplicidade
}

void atualiza_tipo_tabela(char *lexeme, int type)
{
    int indice = busca_tabela(lexeme);
    if (indice != -1) {
        symbols_table[indice].type = type;
    }
}

int pesquisa_declproc_tabela(char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type == PROCEDURE) {
            return i;
        }
    }
    return -1;
}

int pesquisa_declfunc_tabela(char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && (symbols_table[i].type == INTEGER_FUNCTION || symbols_table[i].type == BOOLEAN_FUNCTION)) {
            return i;
        }
    }
    return -1;
}

int pesquisa_declvar_tabela(char *lexeme)
{
    for (int i = 0; i < symbols_count; i++) {
        if (strcmp(symbols_table[i].lexeme, lexeme) == 0 && symbols_table[i].type == VARIABLE) {
            return i;
        }
    }
    return -1;
}

void print_table()
{
    printf("\nTabela de Símbolos:\n");
    printf("Lexeme\t\tType\tScope\tMemory Address\n");
    for (int i = 0; i < symbols_count; i++) {
        printf("%s\t\t%d\t%d\t%d\n", symbols_table[i].lexeme, symbols_table[i].type, symbols_table[i].scope, symbols_table[i].memory_address);
    }
}

#endif // SYMBOL_TABLE_H
