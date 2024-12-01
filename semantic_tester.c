#include "./lexer.h"
#include "./semantic.h"

int main()
{
    Semantic *semantic = create_semantic();
    semantic_insert_symbol(semantic, INTEGER, "x", 0, "0x0001");
    semantic_insert_symbol(semantic, INTEGER, "a", 0, "0x0002");
    semantic_insert_symbol(semantic, INTEGER, "b", 0, "0x0003");
    semantic_insert_symbol(semantic, INTEGER, "c", 0, "0x0004");
    semantic_insert_symbol(semantic, INTEGER, "z", 0, "0x0005");

    Token **expression = (Token **)malloc(17 * sizeof(Token));
    expression[0] = create_token("(", SABREPARENTESES);
    expression[1] = create_token("x", SIDENTIFICADOR);
    expression[2] = create_token("+", SMAIS);
    expression[3] = create_token("a", SIDENTIFICADOR);
    expression[4] = create_token("*", SMULT);
    expression[5] = create_token("b", SIDENTIFICADOR);
    expression[6] = create_token(")", SFECHAPARENTESES);
    expression[7] = create_token("e", SE);
    expression[8] = create_token("(", SABREPARENTESES);
    expression[9] = create_token("(", SABREPARENTESES);
    expression[10] = create_token("b", SIDENTIFICADOR);
    expression[11] = create_token("+", SMAIS);
    expression[12] = create_token("c", SIDENTIFICADOR);
    expression[13] = create_token(")", SFECHAPARENTESES);
    expression[14] = create_token("div", SDIV);
    expression[15] = create_token("z", SIDENTIFICADOR);
    expression[16] = create_token(")", SFECHAPARENTESES);

    printf("Infix expression: ");
    for (int i = 0; i < 17; i++) {
        printf("%s ", expression[i]->lexeme);
    }
    printf("\n");

    int new_expression_size = 0;
    Token **new_expression = infix_to_postfix(semantic, expression, 17, &new_expression_size);

    printf("Postfix expression: ");
    for (int i = 0; i < new_expression_size; i++) {
        printf("%s ", new_expression[i]->lexeme);
    }
    printf("\n");

    printf("Semantic analysis:\n");
    int line_number = get_line_number();
    semantic_analysis(semantic, semantic_find_symbol(semantic, "x"), new_expression, new_expression_size, line_number);
    semantic_analysis(semantic, semantic_find_symbol(semantic, "a"), new_expression, new_expression_size, line_number);
    semantic_analysis(semantic, semantic_find_symbol(semantic, "b"), new_expression, new_expression_size, line_number);
    semantic_analysis(semantic, semantic_find_symbol(semantic, "c"), new_expression, new_expression_size, line_number);
    semantic_analysis(semantic, semantic_find_symbol(semantic, "z"), new_expression, new_expression_size, line_number);
    printf("Semantic analysis finished. No errors found.\n");

    free(expression);
    free(new_expression);
    semantic_remove_symbols(semantic);

    return 0;
}
