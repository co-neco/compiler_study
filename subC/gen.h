#pragma once

int genAST(struct ASTnode* n, int reg, int parentop);
void genpreamble();
void genprintint(int reg);
void genglobsym(int id);
