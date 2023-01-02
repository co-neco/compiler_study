//
// Created by coneco on 22-12-17.
//

#ifndef HOST_DECL_H
#define HOST_DECL_H

#include "defs.h"

int parse_type(int token);
void var_declaration(int type);
struct ASTnode* function_declaration(int type);
void global_declaration();

#endif //HOST_DECL_H
