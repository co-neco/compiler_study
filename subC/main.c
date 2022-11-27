#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "scan.h"
#include <errno.h>
#include <string.h>

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
            printf(", value %d", t.intValue);
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

    scanfile();
    return 0;
}