//
// Created by coneco on 22-12-11.
//
#include "defs.h"
#include "gen.h"
#include "decl.h"
#include "sym.h"

#include "parser.h"

void parse_file() {

    addglob("printint", P_CHAR, S_FUNCTION, 0);

    genpreamble();
    global_declaration();
}