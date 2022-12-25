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