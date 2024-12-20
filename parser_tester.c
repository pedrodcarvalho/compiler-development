#include "parser.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    parser_init(argv[1]);
    analyze_program();
    return 0;
}
