#ifndef TYPES_H
#define TYPES_H

typedef enum {
    PROGRAM_NAME,
    VARIABLE,
    INTEGER,
    BOOLEAN,
    INTEGER_FUNCTION,
    BOOLEAN_FUNCTION,
    FUNCTION,
    PROCEDURE
} Type_t;

const char *type_names[] = {
    "PROGRAM_NAME",
    "VARIABLE",
    "INTEGER",
    "BOOLEAN",
    "INTEGER_FUNCTION",
    "BOOLEAN_FUNCTION",
    "FUNCTION",
    "PROCEDURE"};

#endif // TYPES_H
