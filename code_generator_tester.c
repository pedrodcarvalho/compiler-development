#include "./code_generator.h"

int main()
{
    CodeGenerator generator;
    initializeCodeGenerator(&generator);

    generate(&generator, "L1", "LDC", "", "10");
    generate(&generator, "", "ADD", "", "");
    generate(&generator, "L2", "JMP", "", "L3");
    generate(&generator, "", "ALLOC", "", "0 2");
    generate(&generator, "", "HLT", "", "");

    saveCodeToFile(&generator, "output.obj");

    return 0;
}
