#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "scan.h"
#include <errno.h>
#include <string.h>

#include "gen.h"
#include "stmt.h"

static void init() {
    g_line = 1;
    g_putback = '\n';
}

static void usage(char* prog) {
    fprintf(stderr, "Usage: %s infile\n", prog);
}

char *tokenString[] = {"+", "-", "*", "/", "intlit"};

static void scanfile() {
    struct token t;

    while(scan(&t)) {
        printf("Token %s", tokenString[t.token]);
        if (t.token == T_INTLIT)
            printf(", value %d", t.intvalue);
        printf("\n");
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        usage(argv[0]);
        return -1;
    }

    init();

    if ((g_infile = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Unablee to open %s: %s\n", argv[1], strerror(errno));
        return -1;
    }

    if (g_outfile = fopen("out.s". "w") == NULL) {
        fprintf(stderr, "Unable to create out.s: %s\n", stderror(errno));
        exit(1);
    }

    scan(&g_outfile);
    genpreamble();
    statements();
    genpostamble();
    fclose(g_outfile);
    exit(0);
}