#pragma once

#include <stdio.h>

extern int yylex();
extern void yyset_in(FILE* fp);
extern int yylineno;
extern char* yytext;

enum token {
	ENDOFFILE = 0,
	HOST,
	CHOST,
	OHOST,
	SECTION,
	LSECTION,
	BR,
	IPADDR,
	ID,
	DISP,
	UNEXPECTED,
};
