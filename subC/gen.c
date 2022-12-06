#include "defs.h"
#include "data.h"
#include "cg.h"
#include "misc.h"

#include "gen.h"

static int label() {
	static int label = 0;
	return label++;
}

void parse_file() {
    struct ASTnode* tree;
    
    scan(&g_token);
    genpreamble();
    tree = compound_statement();
    genAST(tree, -1);
    genpostamble();
    fclose(g_outfile);
}

int genAST(struct ASTnode* n, int reg, int parentop) {
	int leftreg, rightreg;
	int condreg, truereg, falsereg;

	if (n->op == A_IF) {

		int lbegin = label();
		int lmid = label();
		int lend = label();

		condreg = genAST(n->left, -1, n->op, lmid);




		truereg = genAST(n->mid, -1, -1);
		falsereg = genAST(n->right, -1, -1);

		cglabel()
	}

	if (n->left)
		leftreg = genAST(n->left, -1);
	if (n->right)
		rightreg = genAST(n->right, leftreg);

	switch (n->op) {
	case A_ADD:
		return cgadd(leftreg, rightreg);
	case A_SUBSTRACT:
		return cgsub(leftreg, rightreg);
	case A_MULTIPLY:
		return cgmul(leftreg, rightreg);
	case A_DIVIDE:
		return cgdiv(leftreg, rightreg);
	case A_INTLIT:
		return cgloadint(n->v.intvalue);
	case A_IDENT:
		return cgloadglob(Gsym[n->v.id].name);
	case A_LVIDENT:
		return cgstoreglob(reg, Gsym[n->v.id].name);
	case A_ASSIGN:
		return rightreg;
	case A_EQ:
		return cgequal(leftreg, rightreg);
	case A_NE:
		return cgnotequal(leftreg, rightreg);
	case A_LT:
		return cglessthan(leftreg, rightreg);
	case A_GT:
		return cggreaterthan(leftreg, rightreg);
	case A_LE:
		return cglessequal(leftreg, rightreg);
	case A_GE:
		return cggreaterequal(leftreg, rightreg);
	default:
		fatald("Unknown AST operator", n->op);
	}

	// Never comes here.
	return -1;
}

void genpreamble() {
	cgpreamble();
}

void genpostamble() {
	cgpostamble();
}

void genfreeregs() {
	freeall_registers();
}

void genprintint(int reg) {
	cgprintint(reg);
}

void genglobsym(char* sym) {
	cgglobsym(sym);
}