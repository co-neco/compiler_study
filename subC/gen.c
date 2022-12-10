#include "defs.h"
#include "data.h"
#include "cg.h"
#include "misc.h"

#include "gen.h"

static int label() {
	static int label = 1;
	return label++;
}

void parse_file() {
    struct ASTnode* tree;
    
    scan(&g_token);
    genpreamble();
    tree = compound_statement();
    genAST(tree, -1, -1);
    genpostamble();
    fclose(g_outfile);
}

static void genifAST(struct ASTnode* n) {
	int condreg, truereg, falsereg;
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
		falsereg = genAST(n->right, -1, -1);
		freeall_registers();
	}
	else {
		lend = label();

		condreg = genAST(n->left, lend, A_IF);
		freeall_registers();

		truereg = genAST(n->mid, -1, -1);
		freeall_registers();
	}
	cglabel(lend);
}

static void genglueAST(struct ASTnode* n) {
	genAST(n->left, -1, -1);
	freeall_registers();
	genAST(n->right, -1, -1);
	freeall_registers();
}

int genAST(struct ASTnode* n, int reg, int parentop) {
	int leftreg, rightreg;

	if (n->op == A_IF) {
		genifAST(n);
		return -1;
	}
	else if (n->op == A_GLUE) {
		genglueAST(n);
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
		return cgloadglob(Gsym[n->v.id].name);
	case A_LVIDENT:
		return cgstoreglob(reg, Gsym[n->v.id].name);
	case A_ASSIGN:
		return rightreg;
	case A_EQ:
	case A_NE:
	case A_LT:
	case A_GT:
	case A_LE:
	case A_GE:
		if (parentop == A_IF) {
			cgifcompare(leftreg, rightreg, n->op, reg);
			return -1;
		}
		else
			return cgcompare(leftreg, rightreg, n->op);
		break;
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