#define extern_
#include "data.h"
#undef extern_
#include <errno.h>
#include <string.h>

#include "parser.h"

static void init() {
    g_line = 1;
    g_putback = '\n';
}

static void usage(char* prog) {
    fprintf(stderr, "Usage: %s infile\n", prog);
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

    if ((g_outfile = fopen("out.s", "w")) == NULL) {
        fprintf(stderr, "Unable to create out.s: %s\n", strerror(errno));
        return -1;
    }

    parse_file();
    fclose(g_outfile);
    return 0;
}