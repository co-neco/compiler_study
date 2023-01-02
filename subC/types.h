//
// Created by coneco on 22-12-18.
//

#ifndef HOST_TYPES_H
#define HOST_TYPES_H

#include "tree.h"

struct ASTnode* modify_type(struct ASTnode* tree, int rtype, int ASTop);

int point_to(int type);
int value_at(int ptrtype);

#endif //HOST_TYPES_H
