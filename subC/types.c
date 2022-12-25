#include <stdio.h>

#include "defs.h"
#include "misc.h"
#include "gen.h"

#include "types.h"

void type_compatibility_check(int* left, int* right, int only_right) {
    char err_msg[20] = {0};

    if (left == NULL || right == NULL){
        fatal("Invalid type parameter");
    }

    if (*left == *right) {
        *left = 0;
        *right = 0;
        return;
    }

    int lsize = gprimsize(*left);
    int rsize = gprimsize(*right);

    if (lsize == 0 || rsize == 0)
        fatal("type parameter's size is 0");

    if (lsize < rsize){
        *left = A_WIDEN;
        *right = 0;
        return;
    }

    if (lsize > rsize) {
        if (only_right) {
            snprintf(err_msg, 20, "t1:%d->t2:%d", *left, *right);
            fatals("Type narrow error", err_msg);
        }

        *right = A_WIDEN;
        *left = 0;
        return;
    }

    *left = 0;
    *right = 0;
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

int value_at(int type) {

    int attype;

    switch (type) {
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
            fatald("Illegal pointer type", type);
    }

    return attype;
}