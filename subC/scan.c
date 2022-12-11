#include "scan.h"
#include "data.h"
#include "misc.h"

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

static int scanident(int c, char* buf, int lim) {
    int i = 0;

    while (isalpha(c) || isdigit(c) || '_' == c){
        if (lim  - 1 == i){
            fatal("Identifier too long");
        }
        else if (i < lim - 1){
            buf[i++] = c;
        }
        c = next();
    }

    putback(c);
    buf[i] = '\0';
    return i;
}

static int keyword(char* ident) {
    switch (*ident) {
        case 'e':
            if (!strcmp(ident, "else"))
                return T_ELSE;
        case 'f':
            if (!strcmp(ident, "for"))
                return T_FOR;
        case 'i':
            if (!strcmp(ident, "int"))
                return T_INT;
            else if (!strcmp(ident, "if"))
                return T_IF;
        case 'p':
            if(!strcmp(ident, "print"))
                return T_PRINT;
        case 'w':
            if (!strcmp(ident, "while"))
                return T_WHILE;
    }
    // Here, 0 just means false, not T_EOF
    return 0;
} 

int scan(struct token* t) {

    int tokentype;
    int c = skip();

    switch (c) {
        case EOF:
            t->token = T_EOF;
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
        case ';':
            t->token = T_SEMI;
            break;
        case '{':
            t->token = T_LBRACE;
            break;
        case '}':
            t->token = T_RBRACE;
            break;
        case '(':
            t->token = T_LPARENT;
            break;
        case ')':
            t->token = T_RPARENT;
            break;
        case '=':
            if ((c = next()) == '=') {
                t->token = T_EQ;
            }
            else {
                putback(c);
                t->token = T_ASSIGN;
            }
            break;
        case '!':
            if ((c = next()) == '=') {
                t->token = T_NE;
            }
            else {
                fatalc("Unrecognized character", c);                
            }
            break;
        case '<':
            if ((c = next()) == '=') {
                t->token = T_LE;
            }
            else {
                putback(c);
                t->token = T_LT;
            }
            break;
        case '>':
            if ((c = next()) == '=') {
                t->token = T_GE;
            }
            else {
                putback(c);
                t->token = T_GT;
            }
            break;
        default:
            if (isdigit(c)) {
                t->intvalue = scanint(c);
                t->token = T_INTLIT;
                break;
            }
            else if (isalpha(c) || '_' == c) {
                scanident(c, g_identtext, TEXTLEN);
                if (tokentype = keyword(g_identtext)){
                    t->token = tokentype;
                    break;
                }
                t->token = T_IDENT;
                break;
            }

            fatalc("Unrecognized character", c);
            return 0;
    }

    return 1;
}