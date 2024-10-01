%{
#include "token.h"

void remove_quotes(char** str) {
	size_t len = strlen(*str);
	(*str)++;
	(*str)[len-2]=0;
}
%}

%%
"HOST"		return HOST;
"SECTION"	return SECTION;
"LSECTION"	return LSECTION;
"BR"		return BR;
[0-9]*\.[0-9]*\.[0-9]*\.[0-9]* return IPADDR;
\"[a-zA-Z0-9_ -]*\"	{remove_quotes(&yytext);return ID;}
[ \t\n]		;
.	dprintf(2, "\033[31mUnxpected character (%s)\033[0m\n", yytext);
%%

int yywrap(void)
{
	return 1;
}
