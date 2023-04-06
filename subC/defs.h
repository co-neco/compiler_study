#pragma once

#define TEXTLEN     512
#define NSYMBOLS    1024

#define NOLABEL	 0		// Use NOLABEL when we have no label to

// Token types
enum {
    // fixed order
    T_EOF, T_ASSIGN,
    T_LOGOR, T_LOGAND, T_OR, T_XOR, T_AMPER,
    T_LSHIFT, T_RSHIFT,
    T_PLUS, T_MINUS, 
    T_STAR, T_SLASH, 
    T_EQ, T_NE, T_LT, T_GT, T_LE, T_GE,

    // random order
    T_INTLIT, T_SEMI, T_IDENT, T_STRLIT,
    T_LBRACE, T_RBRACE, T_LPARENT, T_RPARENT,
    T_LBRACKET, T_RBRACKET, T_COMMA,
    T_INC, T_DEC, T_INVERT, T_LOGNOT,

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
    A_ASSIGN = 1,
    A_LOGOR, A_LOGAND, A_OR, A_XOR, A_AND,
    A_LSHIFT, A_RSHIFT,
    A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE,
    A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE,

    A_INTLIT, A_IDENT, A_PRINT, A_STRLIT,
    A_IF, A_WHILE, A_GLUE, A_FUNCTION,
    A_WIDEN, A_FUNCCALL, A_RETURN, A_ADDR, A_DEREF,
    A_SCALE, A_PREINC, A_PREDEC, A_POSTINC, A_POSTDEC,
    A_NEGATE, A_INVERT, A_LOGNOT, A_TOBOOL
};

struct ASTnode {
    int op;
    int type;
    int rvalue;
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
    S_VARIABLE, S_FUNCTION, S_ARRAY
};

struct symtable {
    char* name;
    int type;
    int stype;
    int endlabel;
    int length;
};
