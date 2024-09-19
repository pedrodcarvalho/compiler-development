#ifndef EXCEPTION_H
#define EXCEPTION_H

void exit_error(const char *message, int line_number)
{
    fprintf(stderr, "Error on line %d: %s\n", line_number, message);
    exit(EXIT_FAILURE);
}

#endif // EXCEPTION_H
