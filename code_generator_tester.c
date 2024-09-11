#include "./code_generator.h"

int main()
{
    generate_label(1);
    generate("LOADCONST");
    generate_with_string("STORE", "A");
    generate_with_int("PRINT", 10);
    generate_with_two_strings("ADD", "A", "B");
    generate_with_two_ints("MULT", 5, 10);
    close_file();
    return 0;
}
