#include "defs.h"
#include "data.h"
#include "misc.h"
#include "tree.h"
#include "sym.h"
#include "scan.h"
#include "types.h"
#include "stmt.h"

#include "expr.h"

static struct ASTnode* primary();

static struct ASTnode* prefix() {

    struct ASTnode* tree;

    switch (g_token.token) {
        case T_AMPER:
            scan(&g_token);
            tree = prefix();

            if (tree->op != A_IDENT)
                fatal("\'&\' operator must be followed by a identifier");

            tree->op = A_ADDR;
            tree->type = point_to(tree->type);
            break;
        case T_STAR:
            scan(&g_token);
            tree = prefix();

            if (tree->op != A_IDENT && tree->op != A_DEREF)
                fatal("\'*\' operator must be followed by a identifier or one another \'*\' operator");

            tree = mkastunary(A_DEREF, value_at(tree->type), tree, 0);
            break;
        default:
            tree = primary();
    }

    return tree;
}

static struct ASTnode* primary() {

	struct ASTnode* node;
	int id;

	switch (g_token.token) {
	case T_INTLIT:
        if (g_token.intvalue >= 0 && g_token.intvalue < 256)
            node = mkastleaf(A_INTLIT, P_CHAR, g_token.intvalue);
        else
            node = mkastleaf(A_INTLIT, P_INT, g_token.intvalue);
		break;
	case T_IDENT:
		id = findglob(g_identtext);
		if (id == -1)
			fatals("Unknown variable", g_identtext);

        scan(&g_token);
        if (g_token.token == T_LPARENT) {
            return funccall();
        }

        restore_token(&g_token);
        node = mkastleaf(A_IDENT, Gsym[id].type, id);
		break;
	default:
		fatald("Syntax error, token", g_token.token);
	}

	scan(&g_token);
	return node;
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

	left = prefix();

	tokentype = g_token.token;
	if (tokentype == T_SEMI || tokentype == T_RPARENT)
		return left;

	while (op_precedence(tokentype) > prevprec) {
		scan(&g_token);

		right = binexpr(opprec[tokentype]);

        int ltype = left->type;
        int rtype = right->type;
        type_compatibility_check(&ltype, &rtype, 0);

        if (ltype)
            left = mkastunary(ltype, right->type, left, 0);
        if (rtype)
            right = mkastunary(rtype, left->type, right, 0);

		left = mkastnode(arithop(tokentype), left->type, left, NULL, right, 0);

		tokentype = g_token.token;
		if (tokentype == T_SEMI || tokentype == T_RPARENT)
			return left;
	}

	return left;
}
