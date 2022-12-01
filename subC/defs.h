#pragma once

#define TEXTLEN     512
#define NSYMBOLS    1024

// Token types
enum {
    T_EOF, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT, T_SEMI, T_EQUALS,
    T_IDENT,
    // Keywords
    T_PRINT, T_INT
};

struct token {
    int token;
    int intvalue;
};

enum {
    A_ADD, A_SUBSTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT, 
    A_IDENT, A_LVIDENT, A_ASSIGN
};

struct ASTnode {
    int op;
    struct ASTnode* left;
    struct ASTnode* right;
    union {
        int intvalue;
        int id;
    } v;
};

struct symtable {
    char* name;
};
