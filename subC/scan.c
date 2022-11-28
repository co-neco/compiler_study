#include "scan.h"
#include "data.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char g_numberString[] = "0123456789";

/// @brief 
/// @param s 
/// @param c 
/// @return return non-negative position, or -1 if did not find c.
static int chrpos(char* s, int c) {
    char* p = strchr(s, c);
    return p ? p - s : -1;
}

static int next() {
    int c;

    if (g_putback) {
        c = g_putback;
        g_putback = 0;
        return c;
    }

    c = fgetc(g_infile);
    if ('\n' == c)
        g_line++;
    return c;
}

static void putback(int c){
    g_putback = c;
}

static int skip() {
    int c;

    c = next();
    while(' ' == c || '\t' == c || '\n' == c || '\r' == c || 'f' == c) {
        c = next();
    }
    return c;
}

static int scanint(int c) {
    int k, val = 0;

    while((k = chrpos(g_numberString, c)) >= 0){
        val = val * 10 + k;
        c = next();
    }

    putback(c);
    return val;
}

int scan(struct token* t) {
    int c = skip();

    switch (c) {
        case EOF:
            return 0;
        case '+':
            t->token = T_PLUS;
            break;
        case '-':
            t->token = T_MINUS;
            break;
        case '*':
            t->token = T_STAR;
            break;
        case '/':
            t->token = T_SLASH;
            break;
        default:
            if (isdigit(c)) {
                t->intvalue = scanint(c);
                t->token = T_INTLIT;
                break;
            }

            printf("Unrecognized character %c on line %d\n", c, g_line);
            return 0;
    }

    return 1;
}