#include "defs.h"
#include "data.h"
#include "cg.h"
#include "misc.h"
#include "types.h"

#include "gen.h"

int glabel() {
	static int label = 1;
	return label++;
}

static void genIfAST(struct ASTnode* n) {
	int lend, lfalse;

	if (n->right != NULL) {
		lfalse = glabel();
		lend = glabel();

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
		lend = glabel();

		genAST(n->left, lend, A_IF);
		freeall_registers();

		genAST(n->mid, -1, -1);
		freeall_registers();
	}
	cglabel(lend);
}

static void genWhileAST(struct ASTnode* n) {
    int lbegin = glabel();
    int lend = glabel();

    cglabel(lbegin);
    genAST(n->left, lend, A_WHILE);
    freeall_registers();

    genAST(n->mid, -1, -1);
    freeall_registers();

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

    // Empty AST
    if (n == NULL)
        return -1;

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
            genAST(n->left, -1, n->op);
            freeall_registers();
            cgfuncpostamble(n->v.id);
            return -1;
    }

	if (n->left)
		leftreg = genAST(n->left, -1, -1);
	if (n->right)
		rightreg = genAST(n->right, leftreg, -1);

	switch (n->op) {
        case A_ADD:
            return cgadd(leftreg, rightreg);
        case A_SUBTRACT:
            return cgsub(leftreg, rightreg);
        case A_MULTIPLY:
            return cgmul(leftreg, rightreg);
        case A_DIVIDE:
            return cgdiv(leftreg, rightreg);
        case A_INTLIT:
            return cgloadint(n->v.intvalue, n->type);
        case A_IDENT:
            // A rvalue or a dereferenced value
            if (n->rvalue || parentop == A_DEREF)
                return cgloadglob(n->v.id);
            else
                // example: b = a + 1, b is a lvalue
                return -1;
        case A_ASSIGN:
            int result;
            switch (n->right->op) {
                case A_IDENT: result = cgstoreglob(leftreg, n->right->v.id); break;
                case A_DEREF: result = cgstorederef(leftreg, rightreg, value_at(n->right->type)); break;
                default: fatal("Wrong type of assignment right operator");
            }
            return result;
        case A_ADDR:
            return cgaddr(n->v.id);
        case A_DEREF:
            if (n->rvalue)
                return cgderef(leftreg, n->type);
            else
                // Leave it for A_ASSIGN to store through the pointer
                return leftreg;
        case A_WIDEN:
            return cgwiden(leftreg, n->left->type, n->type);
        case A_SCALE:
            int value_reg;
            int type_size = gprimsize(value_at(n->type));
            switch (type_size) {
                case 2: value_reg = cgshlconst(leftreg, 1); break;
                case 4: value_reg = cgshlconst(leftreg, 2); break;
                case 8: value_reg = cgshlconst(leftreg, 3); break;
                default:
                    value_reg = cgmul(leftreg, cgloadint(type_size, P_INT));
                    break;
            }
            return value_reg;
        case A_FUNCCALL:
            return cgfunccall(leftreg, n->v.id);
        case A_RETURN:
            cgreturn(leftreg, n->v.id);
            return -1;
        case A_PRINT:
            genprintint(leftreg);
            freeall_registers();
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

int gprimsize(int type) {
    return cgprimsize(type);
}