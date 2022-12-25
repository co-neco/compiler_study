//
// Created by coneco on 22-12-11.
//
#include "defs.h"
#include "data.h"
#include "gen.h"
#include "scan.h"
#include "misc.h"
#include "decl.h"

#include "parser.h"
#include "sym.h"

void parse_file() {
    struct ASTnode* tree;

    addglob("printint", P_CHAR, S_FUNCTION, 0);

    scan(&g_token);
    genpreamble();

    while(1) {
        tree = function_declaration();
        if (tree == NULL) {
            fatal("AST tree is NULL");
        }

        genAST(tree, -1, -1);
        if (g_token.token == T_EOF)
            break;
    }
}