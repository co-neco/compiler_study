#include "defs.h"
#include "data.h"
#include "misc.h"

#include "cg.h"

static int freereg[4];
static char* reglist[4] = { "%r8", "%r9", "%r10", "%r11" };
static char* breglist[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };

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
	fputs("\t.text\n"
		".LC0:\n"
		"\t.string\t\"%d\\n\"\n"
		"printint:\n"
		"\tpushq\t%rbp\n"
		"\tmovq\t%rsp, %rbp\n"
		"\tsubq\t$16, %rsp\n"
		"\tmovl\t%edi, -4(%rbp)\n"
		"\tmovl\t-4(%rbp), %eax\n"
		"\tmovl\t%eax, %esi\n"
		"\tleaq	.LC0(%rip), %rdi\n"
		"\tmovl	$0, %eax\n"
		"\tcall	printf@PLT\n"
		"\tnop\n"
		"\tleave\n"
		"\tret\n" "\n", g_outfile);
}

int cgloadint(int value) {
	int r = alloc_register();
	fprintf(g_outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
	return r;
}

int cgloadglob(int id) {
	int r = alloc_register();
    if (Gsym[id].type == P_INT)
	    fprintf(g_outfile, "\tmovq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
    else
        fprintf(g_outfile, "\tmovzbq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
	return r;
}

int cgstoreglob(int r, int id) {
    if (Gsym[id].type == P_INT)
	    fprintf(g_outfile, "\tmovq\t%s, %s(%%rip)\n", reglist[r], Gsym[id].name);
    else
        fprintf(g_outfile, "\tmovb\t%s, %s(%%rip)\n", breglist[r], Gsym[id].name);
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
    if (Gsym[id].type == P_INT)
	    fprintf(g_outfile, "\t.comm\t%s,8,8\n", Gsym[id].name);
    else if (Gsym[id].type == P_CHAR)
        fprintf(g_outfile, "\t.comm\t%s,1,1\n", Gsym[id].name);
}

void cgfuncpreamble(char* sym) {
    fprintf(g_outfile,
            "\t.text\n"
            "\t.globl\t%s\n"
            "\t.type\t%s, @function\n"
            "%s:\n\tpushq\t%%rbp\n"
            "\tmovq\t%%rsp, %%rbp\n", sym, sym, sym);
}

void cgfuncpostamble() {
    fputs("\tmovl $0, %eax\n" "\tpopq %rbp\n" "\tret\n\n", g_outfile);
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