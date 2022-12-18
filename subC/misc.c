#include "data.h"
#include "scan.h"

#include "misc.h"

#include <stdlib.h>

void match(int t, char* what) {
	if (g_token.token == t)
		scan(&g_token);
	else
		fatals("Expected", what);
}

void semi() {
	match(T_SEMI, ";");
}

void ident() {
	match(T_IDENT, "identifier");
}

void lbrace() {
	match(T_LBRACE, "{");
}

void rbrace() {
	match(T_RBRACE, "}");
}

void lparent() {
	match(T_LPARENT, "(");
}

void rparent() {
	match(T_RPARENT, ")");
}

int isarithop(int op) {
	if (op >= T_EQ && op <= T_GE)
		return 1;
	else
		return 0;
}

void fatal(char* s) {
	fprintf(stderr, "%s on line %d\n", s, g_line); exit(1);
}

void fatals(char* s1, char* s2) {
	fprintf(stderr, "%s:%s on line %d\n", s1, s2, g_line); exit(1);
}

void fatald(char* s, int d) {
	fprintf(stderr, "%s:%d on line %d\n", s, d, g_line); exit(1);
}

void fatalc(char* s, int c) {
	fprintf(stderr, "%s:%c on line %d\n", s, c, g_line); exit(1);
}

