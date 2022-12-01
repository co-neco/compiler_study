#include "defs.h"
#include "data.h"
#include "misc.h"
#include "gen.h"
#include "expr.h"
#include "tree.h"
#include "sym.h"

#include "stmt.h"

void print_statement() {

	struct ASTnode* tree;
	int reg;

	match(T_PRINT, "print");

	tree = binexpr(0);
	reg = genAST(tree, -1);
	genprintint(reg);
	genfreeregs();

	semi();
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


void assignment_statement() {

	struct ASTnode* left, * right, * tree;
	int id;

	ident();

	if ((id = findglob(g_identtext)) == -1)
		fatals("Undeclared variable", g_identtext);

	right = mkastleaf(A_LVIDENT, id);
	match(T_EQUALS, "=");
	left = binexpr(0);
	
	tree = mkastnode(A_ASSIGN, left, right, 0);
	
	genAST(tree, -1);
	genfreeregs();

	semi();
}

void statements() {

	while (1) {
		switch (g_token.token) {
		case T_PRINT:
			print_statement();
			break;
		case T_INT:
			var_declaration();
			break;
		case T_IDENT:
			assignment_statement();
			break;
		case T_EOF:
			return;
		default: 
			fatald("Syntax error, token", g_token.token);
		}
	}
}