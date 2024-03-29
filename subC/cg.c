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

int cgloadglob(int id, int op) {
    int r = alloc_register();

    // Print out the code to initialise it
    switch (Gsym[id].type) {
        case P_CHAR:
            if (op == A_PREINC)
                fprintf(g_outfile, "\tincb\t%s(%%rip)\n", Gsym[id].name);
            if (op == A_PREDEC)
                fprintf(g_outfile, "\tdecb\t%s(%%rip)\n", Gsym[id].name);
            fprintf(g_outfile, "\tmovzbq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
            if (op == A_POSTINC)
                fprintf(g_outfile, "\tincb\t%s(%%rip)\n", Gsym[id].name);
            if (op == A_POSTDEC)
                fprintf(g_outfile, "\tdecb\t%s(%%rip)\n", Gsym[id].name);
            break;
        case P_INT:
            if (op == A_PREINC)
                fprintf(g_outfile, "\tincl\t%s(%%rip)\n", Gsym[id].name);
            if (op == A_PREDEC)
                fprintf(g_outfile, "\tdecl\t%s(%%rip)\n", Gsym[id].name);
            fprintf(g_outfile, "\tmovl\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
            if (op == A_POSTINC)
                fprintf(g_outfile, "\tincl\t%s(%%rip)\n", Gsym[id].name);
            if (op == A_POSTDEC)
                fprintf(g_outfile, "\tdecl\t%s(%%rip)\n", Gsym[id].name);
            break;
        case P_LONG:
        case P_CHARPTR:
        case P_INTPTR:
        case P_LONGPTR:
            if (op == A_PREINC)
                fprintf(g_outfile, "\tincq\t%s(%%rip)\n", Gsym[id].name);
            if (op == A_PREDEC)
                fprintf(g_outfile, "\tdecq\t%s(%%rip)\n", Gsym[id].name);
            fprintf(g_outfile, "\tmovq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
            if (op == A_POSTINC)
                fprintf(g_outfile, "\tincq\t%s(%%rip)\n", Gsym[id].name);
            if (op == A_POSTDEC)
                fprintf(g_outfile, "\tdecq\t%s(%%rip)\n", Gsym[id].name);
            break;
        default:
            fatald("Bad type in cgloadglob:", Gsym[id].type);
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

int cgstorederef(int r1, int r2, int type) {

    switch (type) {
        case P_CHAR:
            fprintf(g_outfile, "\tmovb\t%s, (%s)\n", breglist[r1], reglist[r2]);
            break;
        case P_INT:
            fprintf(g_outfile, "\tmovl\t%s, (%s)\n", dreglist[r1], reglist[r2]);
            break;
        case P_LONG:
            fprintf(g_outfile, "\tmovq\t%s, (%s)\n", reglist[r1], reglist[r2]);
            break;
        default:
            fatald("Bad type in cgstorederef", type);
    }
    free_register(r2);
    return r1;
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

int cgand(int r1, int r2) {
    fprintf(g_outfile, "\tandq\t%s, %s\n", reglist[r1], reglist[r2]);
    free_register(r1); return (r2);
}

int cgor(int r1, int r2) {
    fprintf(g_outfile, "\torq\t%s, %s\n", reglist[r1], reglist[r2]);
    free_register(r1); return (r2);
}

int cgxor(int r1, int r2) {
    fprintf(g_outfile, "\txorq\t%s, %s\n", reglist[r1], reglist[r2]);
    free_register(r1); return (r2);
}

// Negate a register's value
int cgnegate(int r) {
    fprintf(g_outfile, "\tnegq\t%s\n", reglist[r]); return (r);
}

// Invert a register's value
int cginvert(int r) {
    fprintf(g_outfile, "\tnotq\t%s\n", reglist[r]); return (r);
}

int cgshl(int r1, int r2) {
    fprintf(g_outfile, "\tmovb\t%s, %%cl\n", breglist[r2]);
    fprintf(g_outfile, "\tshlq\t%%cl, %s\n", reglist[r1]);
    free_register(r2); return r1;
}

int cgshr(int r1, int r2) {
    fprintf(g_outfile, "\tmovb\t%s, %%cl\n", breglist[r2]);
    fprintf(g_outfile, "\tshrq\t%%cl, %s\n", reglist[r1]);
    free_register(r2); return (r1);
}

int cglognot(int r) {
    fprintf(g_outfile, "\ttest\t%s, %s\n", reglist[r]);
    fprintf(g_outfile, "\tsete\t%s\n", breglist[r]);
    fprintf(g_outfile, "\tmovzbq\t%s, %s\n", breglist[r], reglist[r]);
    return r;
}

int cgtobool(int id, int op, int label) {
    fprintf(g_outfile, "\ttest\t%s, %s\n", reglist[r]);
    if (op == A_IF || op == A_WHILE) {
        fprintf(g_outfile, "\tje\tL%d\n", label);
    }
    else {
        fprintf(g_outfile, "\tsetne\t%s\n", breglist[r]);
        fprintf(g_outfile, "\tmovzbq\t%s, %s\n", breglist[r], reglist[r]);
    }
    return r;
}


void cgglobsym(int id) {
    int typesize;
    char* type = NULL;

    // Get the size of the type
    typesize = cgprimsize(Gsym[id].type);

    fprintf(g_outfile, "\t.data\n" "\t.globl\t%s\n", Gsym[id].name);
    fprintf(g_outfile, "%s:", Gsym[id].name);

    switch(typesize) {
        case 1: type = ".byte"; break;
        case 4: type = ".long"; break;
        case 8: type = ".quad"; break;
        default: fatald("Unknown typesize in cgglobsym: ", typesize); break;
    }

    for (int i = 0; i < Gsym[id].length; ++i) {
        fprintf(g_outfile, "\t%s\t0\n", type); break;
    }
}

void cgglobstr(int label, const char* str) {
    const char* cur;

    cglabel(label);
    for (cur = str; *cur; cur++) {
        fprintf(g_outfile, "\t.byte\t%d\n", *cur);
    }
    fprintf(g_outfile, "\t.byte\t0\n");
}

int cgloadglobstr(int id) {
    int r = alloc_register();
    fprintf(g_outfile, "\tleaq\tL%d(%%rip), %s\n", id, reglist[r]);
    return r;
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

int cgshlconst(int r, int num) {
    fprintf(g_outfile, "\tshlq\t$%d, %s\n", num, reglist[r]);
    return r;
}

static const char* setx[] = { "sete", "setne", "setl", "setg", "setle", "setge" };
static const char* jumpx[] = { "jne", "je", "jge", "jle", "jg", "jl" };

int cgcompare(int r1, int r2, int op) {
	fprintf(g_outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
	fprintf(g_outfile, "\t%s\t%s\n", setx[op - A_EQ], breglist[r2]);
	fprintf(g_outfile, "\tmovzbq\t%s, %s\n", breglist[r2], reglist[r2]);
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