#include "data.h"
#include "misc.h"
#include "sym.h"
#include "gen.h"
#include "scan.h"
#include "stmt.h"
#include "tree.h"

#include "decl.h"

int parse_type(int token) {
    if (token == T_CHAR) return P_CHAR;
    if (token == T_INT) return P_INT;
    if (token == T_VOID) return P_VOID;
    fatald("Illegal type, token", token);
}

void var_declaration() {
    int type, symid;

    // Ensure we have an 'int' token followed by an identifier
    // and a semicolon. Text now has the identifier's name.
    // Add it as a known identifier
    type = parse_type(g_token.token);
    scan(&g_token);
    ident();

    symid = addglob(g_identtext, type, S_VARIABLE);
    genglobsym(symid);
    semi();
}

struct ASTnode* function_declaration() {
    struct ASTnode* tree;
    int nameslot;

    match(T_VOID, "void");
    ident();
    nameslot = addglob(g_identtext, P_VOID, S_FUNCTION);
    lparent();
    rparent();

    tree = compound_statement();
    return mkastunary(A_FUNCTION, P_VOID, tree, nameslot);
}