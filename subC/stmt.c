#include "defs.h"
#include "data.h"
#include "misc.h"
#include "gen.h"
#include "expr.h"
#include "tree.h"
#include "sym.h"
#include "scan.h"

#include "stmt.h"

struct ASTnode *print_statement() {
	struct ASTnode *tree;

	match(T_PRINT, "print");
	tree = binexpr(0);
	semi();
	return tree;
}

void var_declaration(void) {

	// Ensure we have an 'int' token followed by an identifier
	// and a semicolon. Text now has the identifier's name.
	// Add it as a known identifier
	match(T_INT, "int");
	ident();
	addglob(g_identtext);
	genglobsym(g_identtext);
	semi();
}

struct ASTnode *assignment_statement() {

	struct ASTnode *left, *right, *tree;
	int id;

	ident();

	if ((id = findglob(g_identtext)) == -1)
		fatals("Undeclared variable", g_identtext);

	right = mkastleaf(A_LVIDENT, id);
	match(T_ASSIGN, "=");
	left = binexpr(0);

	tree = mkastnode(A_ASSIGN, left, NULL, right, 0);

	semi();
	return tree;
}

struct ASTnode *if_statement() {
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
	
	return mkastnode(A_IF, cond, true, false, 0);
}

struct ASTnode *compound_statement() {
	struct ASTnode *left = NULL;
	struct ASTnode *tree;

	lbrace();

	while (1)
	{
		switch (g_token.token)
		{
		case T_PRINT:
			tree = print_statement();
			break;
		case T_INT:
			var_declaration();
			tree = NULL;
			break;
		case T_IDENT:
			tree = assignment_statment();
			break;
		case T_IF:
			tree = if_statement();
		case T_RBRACE:
			rbrace();
			return left;
		case T_EOF:
			return;
		default:
			fatald("Syntax error, token", g_token.token);
		}

		if (tree != NULL)
		{
			if (left == NULL)
				left = tree;
			else
				tree = mkastnode(A_GLUE, left, NULL, tree, 0);
		}
	}
	return tree;
}