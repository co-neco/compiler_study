#pragma once

void parse_file();
int genAST(struct ASTnode* n, int reg, int parentop);
void genpreamble();
void genpostamble();
void genfreeregs();
void genprintint(int reg);
void genglobsym(char* s);
