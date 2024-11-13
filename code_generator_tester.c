#include "./code_generator.h"

int main()
{
    CodeGenerator generator;
    initializeCodeGenerator(&generator);

    // Generate code for a simple expression: a + (b div 9 - 3) * c (postfix: a b 9 div 3 - c * +)
    // const char *tokens[] = {"a", "b", "9", "div", "3", "-", "c", "*", "+"};
    // generateExpression(&generator, tokens, 9);
    generate(&generator, "L1", "LDC", "", "10");  // L1 LDC 10
    generate(&generator, "", "ADD", "", "");      // ADD
    generate(&generator, "L2", "JMP", "", "L3");  // L2 JMP L3
    generate(&generator, "", "ALLOC", "", "0 2"); // ALLOC 0 2
    generate(&generator, "", "HLT", "", "");      // HLT

    // Save code to a file
    saveCodeToFile(&generator, "output.obj");

    return 0;
}
