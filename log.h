/*
Header only loging and basic escape codes
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define T_RED       "\033[31m"
#define T_GREEN     "\033[32m"
#define T_YELOW     "\033[33m"
#define T_BLUE      "\033[34m"
#define T_MAGENTA   "\033[35m"
#define T_CYAN      "\033[36m"

#define T_BOLD      "\033[1m"
#define T_ITALIC    "\033[3m"
#define T_UNDERLINE "\033[4m"
#define T_BLINKS    "\033[5m"
#define T_BLINKR    "\033[6m"
#define T_REVERSE   "\033[7m"
#define T_CROSSED   "\033[9m"
#define T_NONE      "\033[0m"
#define T_NL        "\n"
#define T_TB        "\t"
#define T_NONE_NL   "\033[0m\n"

#ifndef RELESE
  #define LOG(str)        dprintf(2, "%s" T_NONE " (At f: %s l: %d)" T_NONE_NL,str, __FILE__ , __LINE__ )
  #define LOGF(f, arg...) dprintf(2, f T_NONE " (At f: %s l: %d)" T_NONE_NL, arg , __FILE__ , __LINE__ )
#else
  #define LOG(arg...);
  #define LOGF(arg...);
#endif

#define ERROR(str)        dprintf(2, T_RED "%s" T_NONE " At f: %s l: %d)" T_NONE_NL,str, __FILE__ , __LINE__)
#define ERRORF(f, arg...) dprintf(2, T_RED f T_NONE " (At f: %s l: %d)" T_NONE_NL, arg , __FILE__ , __LINE__ )

#define CRITICAL_ERROR(str)        {dprintf(2, T_RED T_UNDERLINE  "%s (At f: %s l: %d errno %s)" T_NONE_NL,str, __FILE__ , __LINE__,strerror(errno));exit(-1);}
#define CRITICAL_ERRORF(f, arg...) {dprintf(2, T_RED T_UNDERLINE f " (At f: %s l: %d errno: %s)" T_NONE_NL, arg , __FILE__ , __LINE__,strerror(errno));exit(-1);}

#define LEX_SYNTAXERR(msg) CRITICAL_ERRORF("Syntax error %s line %d at \"%s\" (%s)", config_file, yylineno, yytext, msg)
#define CRIT_HANDLE(statement, msg) if(statement)CRITICAL_ERROR(msg);

