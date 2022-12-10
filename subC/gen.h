#pragma once

int genAST(struct ASTnode* n, int reg, int parentop);
void genpreamble();
void genpostamble();
void genprintint(int reg);
void genglobsym(char* s);
