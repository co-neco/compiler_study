//
// Created by coneco on 22-12-11.
//
#include "defs.h"
#include "data.h"
#include "gen.h"
#include "stmt.h"
#include "scan.h"
#include "misc.h"

#include "parser.h"

void parse_file() {
    struct ASTnode* tree;

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