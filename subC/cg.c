#include "defs.h"
#include "data.h"
#include "misc.h"

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

void vgpostamble() {
	fputs("\tmovl $0, $eax\n" "\tpopq $rbp\n" "\tret\n", g_outfile);
}
