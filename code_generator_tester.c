#include "./code_generator.h"

int main()
{
    CodeGenerator *code_generator = (CodeGenerator *)malloc(sizeof(CodeGenerator));
    code_generator = create_code_generator("code_generator_output.vmobj");
    generate_label(code_generator, 1);
    generate(code_generator, "LOADCONST");
    generate_with_string(code_generator, "STORE", "A");
    generate_with_int(code_generator, "PRINT", 10);
    generate_with_two_strings(code_generator, "ADD", "A", "B");
    generate_with_two_ints(code_generator, "MULT", 5, 10);
    close_file(code_generator);
    return 0;
}
