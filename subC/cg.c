#include "defs.h"
#include "data.h"
#include "misc.h"

#include "cg.h"

static int freereg[4];
static char* reglist[4] = { "%r8", "%r9", "%r10", "%r11" };

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
		"\tret\n"
		"\n"
		"\t.globl\tmain\n"
		"\t.type\tmain, @function\n"
		"main:\n" "\tpushq\t%rbp\n" "\tmovq	%rsp, %rbp\n", g_outfile);
}

void cgpostamble() {
	fputs("\tmovl $0, %eax\n" "\tpopq %rbp\n" "\tret\n", g_outfile);
}

int cgloadint(int value) {
	int r = alloc_register();
	fprintf(g_outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
	return r;
}

int cgloadglob(char* identifier) {
	int r = alloc_register();
	fprintf(g_outfile, "\tmovq\t%s(%%rip), %s\n", identifier, reglist[r]);
	return r;
}

int cgstoreglob(int r, char* identifier) {
	fprintf(g_outfile, "\tmovq\t%s, %s(%%rip)\n", reglist[r], identifier);
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

void cgglobsym(char* sym) {
	fprintf(g_outfile, "\t.comm\t%s,8,8\n", sym);
}
