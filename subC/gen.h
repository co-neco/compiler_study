#pragma once

int glabel();
int genglobstr(const char* str);

int genAST(struct ASTnode* n, int reg, int parentop);
void genpreamble();
void genglobsym(int id);
int gprimsize(int type);
