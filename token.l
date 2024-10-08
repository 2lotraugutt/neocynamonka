%{
#include "token.h"

void remove_quotes(char** str) {
	size_t len = strlen(*str);
	(*str)++;
	(*str)[len-2]=0;
}
%}

%%
#.*$		;
"HOST"		return HOST;
"API"		return PIPE;
"QUIET"		return QUIET;
"CHOST"		return CHOST;
"OHOST"		return OHOST;
"SECTION"	return SECTION;
"LSECTION"	return LSECTION;
"BR"		return BR;
([0-9]+\.){1,3}[0-9]* return IPADDR;
\"[\.\/a-zA-Z0-9()_ -]*\"	{remove_quotes(&yytext);return ID;}
[\.\/a-zA-Z0-9()_-]*	return ID;
[ \t\n]		;
.	return UNEXPECTED;
%%

int yywrap(void)
{
	return 1;
}
