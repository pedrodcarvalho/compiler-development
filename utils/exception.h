#ifndef EXCEPTION_H
#define EXCEPTION_H

void exit_error(const char *message, int line_number)
{
    FILE *file = fopen("stderr.log", "w");

    if (file) {
        fprintf(file, "Error on line %d: %s\n", line_number, message);
        fclose(file);
    }

    exit(EXIT_FAILURE);
}

#endif // EXCEPTION_H
