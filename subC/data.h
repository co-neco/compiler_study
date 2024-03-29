#pragma once

#include "defs.h"

#include <stdio.h>

#ifndef extern_
#define extern_ extern
#endif

extern_ int g_line;
extern_ int g_putback;
extern_ int g_dumpAST;
extern_ FILE* g_infile;
extern_ FILE* g_outfile;
extern_ struct token g_token;
extern_ int g_functionid;
extern_ char g_identtext[TEXTLEN];
extern_ char g_strtext[TEXTLEN];
extern_ struct symtable Gsym[NSYMBOLS];