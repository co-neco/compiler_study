#pragma once

void freeall_registers(void);
void cgpreamble();
void cgpostamble();
int cgloadint(int value);
int cgloadglob(char* identifier);
int cgadd(int r1, int r2);
int cgsub(int r1, int r2);
int cgmul(int r1, int r2);
int cgdiv(int r1, int r2);
void cgprintint(int r);
int cgstoreglob(int r, char* identifier);
void cgglobsym(char* sym);

void cgfuncpreamble(char* sym);
void cgfuncpostamble();

int cgcompare(int r1, int r2, int op);
void cgcondcompare(int r1, int r2, int op, int label);

int cglabel(int label);
int cgjump(int label);