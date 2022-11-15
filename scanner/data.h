#pragma once

#include <stdio.h>

#ifndef extern_
#define extern_ extern
#endif

extern_ int g_line;
extern_ int g_putback;
extern_ FILE *g_infile;