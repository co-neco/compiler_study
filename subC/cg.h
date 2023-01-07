#pragma once

void freeall_registers(void);
void cgpreamble();
int cgloadint(int value, int type);
int cgloadglob(int id);
int cgstoreglob(int r, int id);
int cgstorederef(int r1, int r2, int type);
int cgadd(int r1, int r2);
int cgsub(int r1, int r2);
int cgmul(int r1, int r2);
int cgdiv(int r1, int r2);
void cgprintint(int r);
void cgglobsym(int id);

void cgfuncpreamble(char* sym);
void cgfuncpostamble(int id);
int cgfunccall(int r, int id);
void cgreturn(int r, int id);

int cgaddr(int id);
int cgderef(int r, int type);
int cgshlconst(int r, int num);

int cgcompare(int r1, int r2, int op);
void cgcondcompare(int r1, int r2, int op, int label);

int cglabel(int label);
int cgjump(int label);

int cgwiden(int r, int oldtype, int newtype);

int cgprimsize(int type);