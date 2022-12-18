#include <stdio.h>

#include "defs.h"
#include "misc.h"

#include "types.h"

void type_compatibility_check(int* left, int* right, int only_right) {
    char err_msg[20] = {0};

    if (left == NULL || right == NULL){
        fatal("Invalid type parameter");
    }

    if ((*left == P_VOID) || (*right == P_VOID)) {
        snprintf(err_msg, 20, "t1:%d, t2:%d", *left, *right);
        fatals("Incompatible types", err_msg);
    }

    if (*left == *right) {
        *left = 0;
        *right = 0;
        return;
    }

    if ((*left == P_CHAR) && (*right == P_INT)) {
        *left = A_WIDEN;
        *right = 0;
        return;
    }
    else if ((*left == P_INT) && (*right == P_CHAR)) {
        if (only_right) {
            snprintf(err_msg, 20, "t1:%d->t2:%d", *left, *right);
            fatals("Type narrow error", err_msg);
        }

        *right = A_WIDEN;
        *left = 0;
        return;
    }

    // lecture returns true ??
    // Other situations we totally return false
    snprintf(err_msg, 20, "t1:%d, t2:%d", *left, *right);
    fatals("Incompatible types", err_msg);
}