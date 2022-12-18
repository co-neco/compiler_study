#include "defs.h"
#include "data.h"
#include "cg.h"
#include "misc.h"

#include "gen.h"

static int label() {
	static int label = 1;
	return label++;
}

static void genIfAST(struct ASTnode* n) {
	int lend, lfalse;

	if (n->right != NULL) {
		lfalse = label();
		lend = label();

		genAST(n->left, lfalse, A_IF);
		freeall_registers();
		
		genAST(n->mid, -1, -1);
		freeall_registers();
		cgjump(lend);

		cglabel(lfalse);
		genAST(n->right, -1, -1);
		freeall_registers();
	}
	else {
		lend = label();

		genAST(n->left, lend, A_IF);
		freeall_registers();

		genAST(n->mid, -1, -1);
		freeall_registers();
	}
	cglabel(lend);
}

static void genWhileAST(struct ASTnode* n) {
    int lbegin = label();
    int lend = label();

    cglabel(lbegin);
    genAST(n->left, lend, A_WHILE);

    genAST(n->mid, -1, -1);
    cgjump(lbegin);

    cglabel(lend);
}

static void genGlueAST(struct ASTnode* n) {
	genAST(n->left, -1, -1);
	freeall_registers();
	genAST(n->right, -1, -1);
	freeall_registers();
}

int genAST(struct ASTnode* n, int reg, int parentop) {
	int leftreg, rightreg;

    switch (n->op) {
        case A_IF:
            genIfAST(n);
            return -1;
        case A_WHILE:
            genWhileAST(n);
            return -1;
        case A_GLUE:
            genGlueAST(n);
            return -1;
        case A_FUNCTION:
            cgfuncpreamble(Gsym[n->v.id].name);
            genAST(n->left, -1, -1);
            cgfuncpostamble();
            return -1;
    }

	if (n->left)
		leftreg = genAST(n->left, -1, -1);
	if (n->right)
		rightreg = genAST(n->right, leftreg, -1);

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
            return cgloadglob(n->v.id);
        case A_LVIDENT:
            return cgstoreglob(reg, n->v.id);
        case A_ASSIGN:
            return rightreg;
        case A_WIDEN:
            return cgwiden(leftreg, n->left->type, n->type);
        case A_PRINT:
            genprintint(leftreg);
            return -1;
        case A_EQ:
        case A_NE:
        case A_LT:
        case A_GT:
        case A_LE:
        case A_GE:
            if (parentop == A_IF || parentop == A_WHILE) {
                cgcondcompare(leftreg, rightreg, n->op, reg);
                return -1;
            }
            else
                return cgcompare(leftreg, rightreg, n->op);
        default:
            fatald("Unknown AST operator", n->op);
	}

	// Never comes here.
	return -1;
}

void genpreamble() {
	cgpreamble();
}

void genprintint(int reg) {
	cgprintint(reg);
}

void genglobsym(int id) {
	cgglobsym(id);
}