#pragma once

int glabel();

int genAST(struct ASTnode* n, int reg, int parentop);
void genpreamble();
void genprintint(int reg);
void genglobsym(int id);
int gprimsize(int type);
