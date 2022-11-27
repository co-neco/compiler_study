#include "data.h"

#include "misc.h"

void fatal(const char* s) {
	fprintf(stderr, "%s on line %d\n", s, g_line); exit(1);
}

void fatald(const char* s, int d) {
	fprintf(stderr, "%s:%d on line %d\n", s, d, g_line); exit(1);
}