#include "defs.h"
#include "data.h"
#include "misc.h"
#include "tree.h"
#include "sym.h"
#include "scan.h"

#include "expr.h"

static struct ASTnode* primary() {

	struct ASTnode* n;
	int id;

	switch (g_token.token) {
	case T_INTLIT:
		n = mkastleaf(A_INTLIT, g_token.intvalue);
		break;
	case T_IDENT:
		id = findglob(g_identtext);
		if (id == -1)
			fatals("Unknown variable", g_identtext);

		n = mkastleaf(A_IDENT, id);
		break;
	default:
		fatald("Syntax error, token", g_token.token);
	}

	scan(&g_token);
	return n;
}

static int arithop(int tokentype) {
	if (tokentype > T_EOF && tokentype < T_INTLIT)
		return tokentype;
	else {
		fatald("Syntax error, token", tokentype);
		return -1;
	}
}

// Operator precedence for each token. Must
// match up with the order of tokens in defs.h
static int opprec[] = {
  0, 10, 10,                    // T_EOF, T_PLUS, T_MINUS
  20, 20,                       // T_STAR, T_SLASH
  5, 5,                         // T_EQ, T_NE ??????
  6, 6, 6, 6                    // T_LT, T_GT, T_LE, T_GE
};

static int op_precedence(int tokentype) {
	int prec = opprec[tokentype];
	if (prec == 0)
		fatald("Syntax error, token", tokentype);
	return prec;
}

struct ASTnode* binexpr(int prevprec) {

	struct ASTnode* left, *right;
	int tokentype;

	left = primary();

	tokentype = g_token.token;
	if (tokentype == T_SEMI || tokentype == T_RPARENT)
		return left;

	while (op_precedence(tokentype) > prevprec) {
		scan(&g_token);

		right = binexpr(opprec[tokentype]);
		left = mkastnode(arithop(tokentype), left, NULL, right, 0);

		tokentype = g_token.token;
		if (tokentype == T_SEMI || tokentype == T_RPARENT)
			return left;
	}

	return left;
}
