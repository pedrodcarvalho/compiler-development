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
} Types;

const char *TypeNames[] = {
    "PROGRAM_NAME",
    "VARIABLE",
    "INTEGER",
    "BOOLEAN",
    "INTEGER_FUNCTION",
    "BOOLEAN_FUNCTION",
    "FUNCTION",
    "PROCEDURE"};

#endif // TYPES_H
