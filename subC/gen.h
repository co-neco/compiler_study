#pragma once

int genAST(struct ASTnode* n, int reg);
void genpreamble();
void genpostamble();
void genfreeregs();
void genprintint(int reg);
void genglobsym(char* s);