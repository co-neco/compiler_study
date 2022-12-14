#include "defs.h"
#include "data.h"
#include "misc.h"

#include "sym.h"

#include <string.h>

static int g_globindex = 0;

int findglob(char* sym) {

	int i;

	for (i = 0; i < g_globindex; i++) {
		if (*sym == *Gsym[i].name && !strcmp(sym, Gsym[i].name))
			return i;
	}
	return -1;
}

static int newglob() {

	int p;

	if ((p = g_globindex++) >= NSYMBOLS)
		fatal("Too many global symbols");
	return p;
}

int addglob(char* name, int type, int stype, int length, int endlabel) {

	int y;

	if ((y = findglob(name)) != -1)
		return y;

	y = newglob();
	Gsym[y].name = strdup(name);
    Gsym[y].type = type;
    Gsym[y].stype = stype;
    Gsym[y].endlabel = endlabel;
    Gsym[y].length = length;
	return y;
}