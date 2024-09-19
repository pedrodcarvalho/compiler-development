# Lexer

Lexical analyzer (lexer) written in C, designed to tokenize an input file based on the syntax and rules defined by a specific language. It reads the source code, processes characters, and groups them into tokens for further processing by the parser.

## Features

- **Token Recognition**: Handles identifiers, reserved words, numbers, arithmetic and relational operators, assignment operators, punctuation, and comments.
- **Error Handling**: Reports lexical errors such as unclosed comment blocks and invalid symbols with line numbers.
- **File Input**: Reads the source code from a file specified during lexer initialization.
- **Line Tracking**: Keeps track of the line number for accurate error reporting.
- **Token Types**: Supports a wide range of tokens, including:
  - Reserved words (`programa`, `se`, `entao`, etc.)
  - Identifiers and numbers
  - Arithmetic (`+`, `-`, `*`) and relational (`<`, `<=`, `>`, `>=`, `=`) operators
  - Assignment (`:=`) and punctuation (`;`, `,`, `(`, `)`)

## How It Works

1. **Initialization**: The lexer is initialized by providing the input file name, which is then opened for reading. The current character is fetched for processing.
2. **Tokenization**: The lexer reads characters one at a time and groups them into tokens based on defined rules, such as handling digits, identifiers, reserved words, and operators.
3. **Error Handling**: In case of lexical errors (e.g., unclosed comments, invalid symbols), the lexer halts and reports an error, specifying the line number where the error occurred.

## Usage

### Initializing the Lexer

```c
Lexer *lexer = lexer_init("source.txt");
```

This function initializes the lexer with the file `source.txt` and returns a pointer to the lexer instance.

### Fetching Tokens

Use `lexer_token` to fetch tokens sequentially from the file.

```c
Token *token = lexer_token(lexer);
```

### Cleaning Up

Once you are done processing, remember to free the allocated memory for the lexer and tokens:

```c
lexer_destroy(lexer);
free_token(token);
```

### Example Workflow

```c
Lexer *lexer = lexer_init("source.txt");
Token *token;

while ((token = lexer_token(lexer)) != NULL) {
    printf("Token: %s\n", token->lexeme);
    free_token(token);
}

lexer_destroy(lexer);
```

## Error Handling

Errors such as memory allocation failures or I/O errors are handled by the `exit_error` function, which terminates the program and provides an error message along with the line number.

## Dependencies

- `exception.h`: Handles errors and exceptions in the lexer.
- `symbols.h`: Contains symbol definitions for the tokens.
- `token.h`: Defines the `Token` structure and the `create_token` function.
