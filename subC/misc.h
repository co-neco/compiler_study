#pragma once

void match(int t, char* what);
void semi(void);
void ident(void);
void lbracket();
void rbracket();
void lbrace();
void rbrace();
void lparent();
void rparent();
int isarithop(int op);

void fatal(char* s);
void fatals(char* s1, char* s2);
void fatald(char* s, int d);
void fatalc(char* s, int c);