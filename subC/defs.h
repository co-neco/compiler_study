#pragma once

#define TEXTLEN     512
#define NSYMBOLS    1024

// Token types
enum {
    // fixed order
    T_EOF, 
    T_PLUS, T_MINUS, 
    T_STAR, T_SLASH, 
    T_EQ, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_INTLIT, T_SEMI, T_ASSIGN, T_IDENT,

    // random order
    T_LBRACE, T_RBRACE, T_LPARENT, T_RPARENT,
    T_AMPER, T_COMMA,

    // Keywords
    T_PRINT, T_INT, T_IF, T_ELSE, T_WHILE, T_FOR,
    T_VOID, T_CHAR, T_LONG, T_RETURN
};

enum {
    P_NONE, P_VOID, P_CHAR, P_INT, P_LONG,
    P_VOIDPTR, P_CHARPTR, P_INTPTR, P_LONGPTR
};

struct token {
    int token;
    int intvalue;
};

enum {
    // The order must be the same as token's enum
    A_ADD = 1, A_SUBSTRACT, A_MULTIPLY, A_DIVIDE, 
    A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE, 
    A_INTLIT, A_IDENT, A_LVIDENT, A_ASSIGN,
    A_PRINT, A_IF, A_WHILE, A_GLUE, A_FUNCTION,
    A_WIDEN, A_FUNCCALL, A_RETURN, A_ADDR, A_DEREF
};

struct ASTnode {
    int op;
    int type;
    struct ASTnode* left;
    struct ASTnode* mid;
    struct ASTnode* right;
    union {
        int intvalue;
        int id;
    } v;
};

// Structural types
enum {
    S_VARIABLE, S_FUNCTION
};

struct symtable {
    char* name;
    int type;
    int stype;
    int endlabel;
};
