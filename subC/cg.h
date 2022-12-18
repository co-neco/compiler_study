#pragma once

void freeall_registers(void);
void cgpreamble();
int cgloadint(int value);
int cgloadglob(int id);
int cgstoreglob(int r, int id);
int cgadd(int r1, int r2);
int cgsub(int r1, int r2);
int cgmul(int r1, int r2);
int cgdiv(int r1, int r2);
void cgprintint(int r);
void cgglobsym(int id);

void cgfuncpreamble(char* sym);
void cgfuncpostamble();

int cgcompare(int r1, int r2, int op);
void cgcondcompare(int r1, int r2, int op, int label);

int cglabel(int label);
int cgjump(int label);

int cgwiden(int r, int oldtype, int newtype);