#pragma once

#define TEXTLEN     512
#define NSYMBOLS    1024

// Token types
enum {
    T_EOF, 
    T_PLUS, T_MINUS, 
    T_STAR, T_SLASH, 
    T_EQ, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_INTLIT, T_SEMI, T_ASSIGN, T_IDENT,

    // random tokens
    T_LBRACE, T_RBRACE, T_LPARENT, T_RPARENT,

    // Keywords
    T_PRINT, T_INT, T_IF, T_ELSE
};

struct token {
    int token;
    int intvalue;
};

enum {
    // The order must be the same as token's enum
    A_ADD = 1, A_SUBSTRACT, A_MULTIPLY, A_DIVIDE, 
    A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE, 
    A_INTLIT, 
    A_IDENT, A_LVIDENT, A_ASSIGN, A_IF, A_GLUE 
};

struct ASTnode {
    int op;
    struct ASTnode* left;
    struct ASTnode* mid;
    struct ASTnode* right;
    union {
        int intvalue;
        int id;
    } v;
};

struct symtable {
    char* name;
};
