#include "data.h"
#include "misc.h"
#include "sym.h"
#include "gen.h"
#include "scan.h"
#include "stmt.h"
#include "tree.h"
#include "gen.h"

#include "decl.h"

static int parse_type(int token) {
    if (token == T_VOID) return P_VOID;
    if (token == T_CHAR) return P_CHAR;
    if (token == T_INT) return P_INT;
    if (token == T_LONG) return P_LONG;
    fatald("Illegal type, token", token);
}

static int is_last_return_statement(struct ASTnode* tree) {

    if (tree == NULL)
        return 0;

    if ((tree->op == A_GLUE && tree->right->op == A_RETURN)
        ||
        tree->op == A_RETURN)
        return 1;
    else
        return 0;
}

void var_declaration() {
    int type, symid;

    // Ensure we have an 'int' token followed by an identifier
    // and a semicolon. Text now has the identifier's name.
    // Add it as a known identifier
    type = parse_type(g_token.token);
    scan(&g_token);
    ident();

    symid = addglob(g_identtext, type, S_VARIABLE, 0);
    genglobsym(symid);
    semi();
}

struct ASTnode* function_declaration() {
    struct ASTnode* tree;
    int id, type, endlabel;

    type = parse_type(g_token.token);

    scan(&g_token);
    ident();

    endlabel = glabel();
    id = addglob(g_identtext, type, S_FUNCTION, endlabel);

    lparent();
    rparent();

    g_functionid = id;

    tree = compound_statement();

    if (type == P_VOID && is_last_return_statement(tree)) {
        fatals("invalid return statement in function", Gsym[id].name);
    }
    else if (type != P_VOID && !is_last_return_statement(tree)) {
        fatals("There is no return statement in function", Gsym[id].name);
    }

    return mkastunary(A_FUNCTION, P_VOID, tree, id);
}