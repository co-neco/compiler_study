#include <stdio.h>

#include "defs.h"
#include "misc.h"
#include "gen.h"

#include "types.h"

int is_inttype(int type) {
    if (type >= P_CHAR && type <= P_LONG)
        return 1;
    else
        return 0;
}

int is_ptrtype(int type) {
    if (type >= P_VOIDPTR && type <= P_LONGPTR)
        return 1;
    else
        return 0;
}

struct ASTnode* modify_type(struct ASTnode* tree, int rtype, int ASTop) {

    if (tree == NULL)
        return NULL;

    int ltype = tree->type;

    if (is_inttype(ltype) && is_inttype(rtype)) {

        if (ltype == rtype)
            return tree;

        int lsize = gprimsize(ltype);
        int rsize = gprimsize(rtype);

        if (lsize > rsize)
            // large type squeeze to smaller type
            return NULL;

        if (lsize == rsize)
            return tree;

        if (lsize < rsize)
            return mkastunary(A_WIDEN, rtype, tree, 0);
    }

    if (is_ptrtype(ltype) && ltype == rtype)
        return tree;

    if (ASTop == A_ADD || ASTop == A_SUBTRACT) {
        if (is_inttype(ltype) && is_ptrtype(rtype)) {
            int type_size = gprimsize(value_at(rtype));
            if (type_size > 1) {
                return mkastunary(A_SCALE, rtype, tree, type_size);
            }
            else
                return tree;
        }
    }

    return NULL;
}

int point_to(int type) {

    int totype;

    switch (type) {
        case P_VOID:
            totype = P_VOIDPTR;
            break;
        case P_CHAR:
            totype = P_CHARPTR;
            break;
        case P_INT:
            totype = P_INTPTR;
            break;
        case P_LONG:
            totype = P_LONGPTR;
            break;
        default:
            fatald("Illegal primitive type", type);
    }

    return totype;
}

int value_at(int ptrtype) {

    int attype;

    switch (ptrtype) {
        case P_VOIDPTR:
            attype = P_VOID;
            break;
        case P_CHARPTR:
            attype = P_CHAR;
            break;
        case P_INTPTR:
            attype = P_INT;
            break;
        case P_LONGPTR:
            attype = P_LONG;
            break;
        default:
            fatald("Illegal pointer type", ptrtype);
    }

    return attype;
}