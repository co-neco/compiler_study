#include "defs.h"
#include "data.h"
#include "cg.h"
#include "misc.h"

#include "gen.h"

int genAST(struct ASTnode* n, int reg) {

	int leftreg, rightreg;

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
		return cggreatthan(leftreg, rightreg);
	case A_LE:
		return cglessequal(leftreg, rightreg);
	case A_GE:
		return cggreatequal(leftreg, rightreg);
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