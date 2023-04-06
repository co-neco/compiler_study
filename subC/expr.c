#include "defs.h"
#include "data.h"
#include "misc.h"
#include "tree.h"
#include "sym.h"
#include "scan.h"
#include "types.h"
#include "gen.h"

#include "expr.h"

static struct ASTnode* postfix();
static struct ASTnode* primary();

struct ASTnode* funccall() {
    struct ASTnode* tree;

    int id = findglob(g_identtext);
    if (id == -1)
        fatals("undeclared variable", g_identtext);

    lparent();
    struct ASTnode* para = binexpr(0);
    rparent();

    tree = mkastunary(A_FUNCCALL, Gsym[id].type, para, id);
    return tree;
}

struct ASTnode* array_value(int symid) {
    struct ASTnode* tree;

    int id = findglob(g_identtext);
    if (id == -1 || Gsym[symid].stype != S_ARRAY)
        fatals("undeclared array", g_identtext);

    lbracket();
    struct ASTnode* index = binexpr(0);
    rbracket();

    tree = mkastleaf(A_ADDR, Gsym[symid].type, symid);

    index = modify_type(index, tree->type, A_ADD);
    if (!index)
        fatal("Incompatible type when index array");

    tree = mkastnode(A_ADD, tree->type, tree, NULL, index, 0);
    tree = mkastunary(A_DEREF, value_at(tree->type), tree, 0);
    return tree;
}

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
        // T_MINUS, T_LOGNOT, T_INVERT can be a lvalue or a rvalue
        case T_MINUS:
            scan(&g_token);
            tree = prefix();
            tree = mkastunary(A_NEGATE, tree->type, tree, 0);
            break;
        case T_LOGNOT:
            scan(&g_token);
            tree = prefix();
            tree = mkastunary(A_LOGNOT, tree->type, tree, 0);
            break;
        case T_INVERT:
            scan(&g_token);
            tree = prefix();
            tree = mkastunary(A_INVERT, tree->type, tree, 0);
            break;
        case T_DEC:
            scan(&g_token);
            tree = prefix();

            if (tree->op != A_IDENT) {
                fatal("pre-decrement must be followed by a lvalue");
            }

            tree = mkastunary(A_PREDEC, tree->type, tree, 0);
            break;
        case T_INC:
            scan(&g_token);
            tree = prefix();

            if (tree->op != A_IDENT) {
                fatal("pre-increment must be followed by a lvalue");
            }

            tree = mkastunary(A_PREINC, tree->type, tree, 0);
            break;
        default:
            tree = postfix();
    }

    return tree;
}

static struct ASTnode* postfix() {

    struct ASTnode* node;

    node = primary();
    if (node->type != T_IDENT) {
        scan(&g_token);
        return node;
    }

    // Do not support (expression) + ((expression), [expression], ++, --) now.
    switch (g_token.token) {
        case T_LPARENT:
            node = funccall();
            break;
        case T_LBRACKET:
            node = array_value(node->v.id);
            break;
        case T_INC:
            node = mkastunary(A_POSTINC, node->type, node, 0);
            break;
        case T_DEC:
            node = mkastunary(A_POSTDEC, node->type, node, 0);
            break;
    }

    return node;
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
        case T_STRLIT:
            id = genglobstr(g_strtext);
            node = mkastleaf(A_STRLIT, P_CHARPTR, id);
            break;
        case T_IDENT:
            id = findglob(g_identtext);
            if (id == -1)
                fatals("Unknown variable", g_identtext);

            node = mkastleaf(A_IDENT, Gsym[id].type, id);
            if (Gsym[id].stype == S_ARRAY) {
                node->op = A_ADDR;
            }
            break;
        case T_LPARENT:
            lparent();
            node = binexpr(0);
            if (g_token.token != T_RPARENT)
                fatal("No right parent in an expression");
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
  DEFAULT_PREC, 1,
  12, 11, 10, 9, 8,  // T_LOGOR, T_LOGAND, T_OR, T_XOR, T_AMPER,
  5, 5,                          // T_LSHIFT, T_RSHIFT,
  4, 4,                         // T_PLUS, T_MINUS
  3, 3,                       // T_STAR, T_SLASH
  7, 7,                         // T_EQ, T_NE
  6, 6, 6, 6          // T_LT, T_GT, T_LE, T_GE
};

static int op_precedence(int tokentype) {
	int prec = opprec[tokentype];
	if (prec == 0)
		fatald("Syntax error, token", tokentype);
	return -prec;
}

static int is_right_associative(int tokentype) {
    if (tokentype == T_ASSIGN)
        return 1;
    else
        return 0;
}

struct ASTnode* binexpr(int prevprec) {

	struct ASTnode* left, *right;
	int tokentype;

	left = prefix();
    left->rvalue = 0;

	tokentype = g_token.token;
	if (tokentype == T_SEMI || tokentype == T_RPARENT
        || tokentype == T_RBRACKET)
		return left;

	while ((op_precedence(tokentype) > prevprec)
            ||
            (is_right_associative(tokentype) && (op_precedence(tokentype) == prevprec))) {

        scan(&g_token);

		right = binexpr(opprec[tokentype]);

        int ASTop = arithop(tokentype);
        struct ASTnode* new_left = modify_type(left, right->type, ASTop);
        struct ASTnode* new_right = modify_type(right, left->type, ASTop);

        if ((new_left == NULL) && (new_right == NULL))
            fatal("Expression with incompatible types");

        if (new_left != NULL)
            left = new_left;
        if (new_right != NULL)
            right = new_right;

        if (ASTop == A_ASSIGN) {
            struct ASTnode* tmp = left;
            left = right;
            right = tmp;
            right->rvalue = 0;
        }

		left = mkastnode(arithop(tokentype), left->type, left, NULL, right, 0);

		tokentype = g_token.token;
		if (tokentype == T_SEMI || tokentype == T_RPARENT
            || tokentype == T_RBRACKET)
			return left;
	}

	return left;
}
