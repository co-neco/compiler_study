#pragma once

#include <stdio.h>

#ifndef extern_
#define extern_ extern
#endif

extern_ int g_line;
extern_ int g_putback;
extern_ FILE* g_infile;
extern_ FILE* g_outfile;
extern_ struct* token g_token;
extern_ char g_identText[TEXTLEN + 1];
extern_ struct symtable Gsym[NSYMBOLS];