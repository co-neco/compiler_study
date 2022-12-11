//
// Created by coneco on 22-12-11.
//
#include "defs.h"
#include "data.h"
#include "gen.h"
#include "stmt.h"
#include "scan.h"

#include "parser.h"

void parse_file() {
    struct ASTnode* tree;

    scan(&g_token);
    genpreamble();
    tree = compound_statement();
    genAST(tree, -1, -1);
    genpostamble();
    fclose(g_outfile);
}