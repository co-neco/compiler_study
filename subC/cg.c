#include "defs.h"
#include "data.h"
#include "misc.h"

#include "cg.h"

static int freereg[4];
static char* reglist[4] = { "%r8", "%r9", "%r10", "%r11" };
static char* breglist[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };
static char* dreglist[4] = { "%r8d", "%r9d", "%r10d", "%r11d" };

static int primsize[] = {0, 0, 1, 4, 8, 8, 8, 8};

void freeall_registers() {
	freereg[0] = freereg[1] = freereg[2] = freereg[3] = 1;
}

static int alloc_register() {
	for (int i = 0; i < 4; i++) {
		if (freereg[i]) {
			freereg[i] = 0;
			return i;
		}
	}
	fatal("Out of registers");
}

static void free_register(int reg) {
	if (freereg[reg] != 0)
		fatald("Error trying to free register", reg);
	freereg[reg] = 1;
}

void cgpreamble() {
	freeall_registers();
}

// We leave the type cast to variable or return generation
int cgloadint(int value, int type) {
	int r = alloc_register();
	fprintf(g_outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
	return r;
}

int cgloadglob(int id) {
	int r = alloc_register();

    switch (Gsym[id].type) {
        case P_CHAR:
            fprintf(g_outfile, "\tmovzbq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
            break;
        case P_INT:
            fprintf(g_outfile, "\tmovl\t%s(%%rip), %s\n", Gsym[id].name, dreglist[r]);
            break;
        case P_LONG:
        case P_CHARPTR:
        case P_INTPTR:
        case P_LONGPTR:
            fprintf(g_outfile, "\tmovq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
            break;
        default:
            fatald("Bad type in cgloadglob", Gsym[id].type);
    }
	return r;
}

int cgstoreglob(int r, int id) {


    switch (Gsym[id].type) {
        case P_CHAR:
            fprintf(g_outfile, "\tmovb\t%s, %s(%%rip)\n", breglist[r], Gsym[id].name);
            break;
        case P_INT:
            fprintf(g_outfile, "\tmovl\t%s, %s(%%rip)\n", dreglist[r], Gsym[id].name);
            break;
        case P_LONG:
        case P_CHARPTR:
        case P_INTPTR:
        case P_LONGPTR:
            fprintf(g_outfile, "\tmovq\t%s, %s(%%rip)\n", reglist[r], Gsym[id].name);
            break;
        default:
            fatald("Bad type in cgstoreglob", Gsym[id].type);
    }
	return r;
}

int cgadd(int r1, int r2) {
	fprintf(g_outfile, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
	free_register(r1);
	return r2;
}

int cgsub(int r1, int r2) {
	fprintf(g_outfile, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
	free_register(r2);
	return (r1);
}

int cgmul(int r1, int r2) {
	fprintf(g_outfile, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
	free_register(r1);
	return r2;
}

int cgdiv(int r1, int r2) {
	fprintf(g_outfile, "\tmovq\t%s,%%rax\n", reglist[r1]);
	fprintf(g_outfile, "\tcqo\n");
	fprintf(g_outfile, "\tidivq\t%s\n", reglist[r2]);
	fprintf(g_outfile, "\tmovq\t%%rax, %s\n", reglist[r1]);
	free_register(r2);
	return r1;
}

void cgprintint(int r) {
	fprintf(g_outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
	fprintf(g_outfile, "\tcall\tprintint\n");
	free_register(r);
}

void cgglobsym(int id) {
    int typesize = cgprimsize(Gsym[id].type);
    fprintf(g_outfile, "\t.comm\t%s,%d,%d\n", Gsym[id].name, typesize, typesize);
}

void cgfuncpreamble(char* sym) {
    fprintf(g_outfile,
            "\t.text\n"
            "\t.globl\t%s\n"
            "\t.type\t%s, @function\n"
            "%s:\n\tpushq\t%%rbp\n"
            "\tmovq\t%%rsp, %%rbp\n", sym, sym, sym);
}

void cgfuncpostamble(int id) {
    if (Gsym[id].type != P_VOID)
        cglabel(Gsym[id].endlabel);
    fputs("\tpopq %rbp\n" "\tret\n\n", g_outfile);
}

int cgfunccall(int r, int id) {
    int r1 = alloc_register();

    fprintf(g_outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
    fprintf(g_outfile, "\tcall\t%s\n", Gsym[id].name);
    fprintf(g_outfile, "\tmovq\t%%rax, %s\n", reglist[r1]);
    free_register(r);
    return r1;
}

void cgreturn(int r, int id) {

    switch (Gsym[id].type) {
        case P_CHAR:
            fprintf(g_outfile, "\tmovzbl\t%s, %%eax\n", breglist[r]);
            break;
        case P_INT:
            // movl would set high 32bit of rax to 0
            fprintf(g_outfile, "\tmovl\t%s, %%eax\n", dreglist[r]);
            break;
        case P_LONG:
            fprintf(g_outfile, "\tmovq\t%s, %%rax\n", reglist[r]);
        default:
            fatald("bad type in cgreturn", Gsym[id].type);
    }
    cgjump(Gsym[id].endlabel);
    free_register(r);
}

int cgaddr(int id) {
    int r = alloc_register();
    fprintf(g_outfile, "\tleaq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
    return r;
}

int cgderef(int r, int type) {

    int r1 = alloc_register();

    switch (type) {
        case P_CHAR:
            fprintf(g_outfile, "\tmovzbq\t(%s), %s\n", reglist[r], reglist[r1]);
            break;
        case P_INT:
            fprintf(g_outfile, "\tmovl\t(%s), %s\n", reglist[r], dreglist[r1]);
            break;
        case P_LONG:
            fprintf(g_outfile, "\tmovq\t(%s), %s\n", reglist[r], reglist[r1]);
            break;
        default:
            fatald("Illegal derefernce type", type);
    }
    free_register(r);
    return r1;
}

static const char* setx[] = { "sete", "setne", "setl", "setg", "setle", "setge" };
static const char* jumpx[] = { "jne", "je", "jge", "jle", "jg", "jl" };

int cgcompare(int r1, int r2, int op) {
	fprintf(g_outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
	fprintf(g_outfile, "\t%s\t%s\n", setx[op - A_EQ], breglist[r2]);
	fprintf(g_outfile, "\tmovzbq\t%s, %s\n", breglist[r2], reglist[2]);
	free_register(r1);
	return r2;
}

void cgcondcompare(int r1, int r2, int op, int label) {
	fprintf(g_outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
	fprintf(g_outfile, "\t%s\tL%d\n", jumpx[op - A_EQ], label);
	free_register(r1);
	free_register(r2);
}

int cglabel(int label) {
	fprintf(g_outfile, "L%d:\n", label);
}

int cgjump(int label) {
	fprintf(g_outfile, "\tjmp\tL%d\n", label);
}

int cgwiden(int r, int oldtype, int newtype) {
    // Now no operation
    return r;
}

int cgprimsize(int type) {
    if (type < P_NONE || type > P_LONGPTR)
        fatald("Bad type", type);
    return primsize[type];
}