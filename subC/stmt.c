#include "defs.h"
#include "data.h"
#include "decl.h"
#include "misc.h"
#include "expr.h"
#include "tree.h"
#include "sym.h"
#include "scan.h"
#include "types.h"

#include "stmt.h"

static struct ASTnode* print_statement() {
    struct ASTnode *expr, *tree;

    match(T_PRINT, "print");
    expr = binexpr(0);

    int ltype = P_INT;
    int rtype = expr->type;
    type_compatibility_check(&ltype, &rtype, 0);

    if (rtype) expr = mkastunary(rtype, ltype, expr, 0);

    tree = mkastunary(A_PRINT, P_NONE, expr, 0);
    return tree;
}

static struct ASTnode* assignment_statement() {

    struct ASTnode *left, *right, *tree;
    int id;

    ident();

    if ((id = findglob(g_identtext)) == -1)
        fatals("Undeclared variable", g_identtext);

    right = mkastleaf(A_LVIDENT, Gsym[id].type, id);
    match(T_ASSIGN, "=");
    left = binexpr(0);

    int ltype = left->type;
    int rtype = right->type;
    type_compatibility_check(&ltype, &rtype, 1);

    if (ltype) left = mkastunary(ltype, rtype, left, 0);

    tree = mkastnode(A_ASSIGN, right->type, left, NULL, right, 0);

    return tree;
}

static struct ASTnode* if_statement() {
	struct ASTnode *tree, *cond, *true, *false = NULL;

	match(T_IF, "if");
	lparent();

	cond = binexpr(0);
	if (!isarithop(cond->op))
		fatald("If statement with wrong condition operator", cond->op);

	rparent();
	true = compound_statement();

	if (g_token.token == T_ELSE) {
		scan(&g_token);
		false = compound_statement();
	}

	return mkastnode(A_IF, P_NONE, cond, true, false, 0);
}

static struct ASTnode* while_statement() {
    struct ASTnode *tree, *cond;

    match(T_WHILE, "while");
    lparent();

    cond = binexpr(0);
    rparent();

    struct ASTnode* stmts = compound_statement();
    return mkastnode(A_WHILE, P_NONE, cond, stmts, NULL, 0);
}

static struct ASTnode* for_statement() {
    struct ASTnode* pre, *cond, *post;

    match(T_FOR, "for");
    lparent();

    pre = single_statement();
    semi();

    cond = binexpr(0);
    semi();

    post = single_statement();
    rparent();

    struct ASTnode* comp = compound_statement();
    if (comp == NULL)
        fatal("for loop with empty statement");

    struct ASTnode* tree = mkastnode(A_GLUE, P_NONE, comp, NULL, post, 0);
    tree = mkastnode(A_WHILE, P_NONE, cond, tree, NULL, 0);
    tree = mkastnode(A_GLUE, P_NONE, pre, NULL, tree, 0);
    return tree;
}

struct ASTnode* single_statement() {
    struct ASTnode* tree = NULL;

    switch (g_token.token) {
        case T_PRINT:
            tree = print_statement();
            break;
        case T_INT:
        case T_CHAR:
            var_declaration();
            tree = NULL;
            break;
        case T_IDENT:
            tree = assignment_statement();
            break;
        case T_IF:
            tree = if_statement();
            break;
        case T_WHILE:
            tree = while_statement();
            break;
        case T_FOR:
            tree = for_statement();
            break;
        case T_RBRACE:
            return NULL;
        case T_EOF:
            // exit current process
            fatal("Incomplete compound statement");
        default:
            fatald("Syntax error, token", g_token.token);
    }

    return tree;
}

struct ASTnode* compound_statement() {
	struct ASTnode *left = NULL;
	struct ASTnode *tree;

	lbrace();

	while (1) {
        if (g_token.token == T_RBRACE) {
            rbrace();
            return left;
        }

        tree = single_statement();
        if (tree == NULL)
            continue;

        if (tree->op == A_PRINT || tree->op == A_ASSIGN)
            semi();

		if (tree != NULL)
		{
			if (left == NULL)
				left = tree;
			else
				left = mkastnode(A_GLUE, P_NONE, left, NULL, tree, 0);
		}
	}
}