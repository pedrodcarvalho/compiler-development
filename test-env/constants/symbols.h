#ifndef SYMBOLS_H
#define SYMBOLS_H

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
    SFALSO,
    SPOSITIVO,
    SNEGATIVO,
    SERRO
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
    "SFALSO",
    "SPOSITIVO",
    "SNEGATIVO",
    "SERRO"};

int is_operand(Symbols symbol)
{
    return symbol == SIDENTIFICADOR || symbol == SNUMERO || symbol == SVERDADEIRO || symbol == SFALSO;
}

int is_operator(Symbols symbol)
{
    return symbol == SOU || symbol == SE || symbol == SNAO || symbol == SMAIOR || symbol == SMAIORIG || symbol == SIG || symbol == SMENOR || symbol == SMENORIG || symbol == SDIF || symbol == SMAIS || symbol == SMENOS || symbol == SMULT || symbol == SDIV || symbol == SPOSITIVO || symbol == SNEGATIVO;
}

#endif // SYMBOLS_H
