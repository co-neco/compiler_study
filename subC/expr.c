#include "defs.h"
#include "data.h"
#include "misc.h"

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

	switch (tokentype) {
	case T_PLUS:
		return A_ADD;
	case T_MINUS:
		return A_SUBSTRACT;
	case T_STAR:
		return A_MULTIPLY;
	case T_SLASH:
		return A_DIVIDE;
	default:
		fatald("Syntax error, token", tokentype);
	}
}

static int opprec[] = { 0, 10, 10, 20, 20, 0 };

static int op_precedence(int tokentype) {
	int prec = opprec[tokentype];
	if (prec == 0)
		fatald("Syntax error, token", tokentype);
	return prec;
}

static ASTnode* binexpr(int prevprec) {

	struct ASTnode* left, right;
	int tokentype;

	left = primary();

	tokentype = g_token.token;
	if (tokentype == T_SEMI)
		return left;

	while (op_precedence(tokentype) > prevprec) {
		scan(&g_token);

		right = binexpr(opprec[tokentype]);
		left = mkastnode(arithop(tokentype), left, right, 0);

		tokentype = g_token.token;
		if (tokentype == T_SEMI)
			return left;
	}

	return left;
}
