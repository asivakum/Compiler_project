
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     END = 0,
     PROGRAM = 258,
     BEGINN = 259,
     FUNCTION = 260,
     READ = 261,
     WRITE = 262,
     IF = 263,
     ELSE = 264,
     ENDIF = 265,
     WHILE = 266,
     ENDWHILE = 267,
     CONTINUE = 268,
     BREAK = 269,
     RETURN = 270,
     INT = 271,
     VOID = 272,
     STRING = 273,
     FLOAT = 274,
     ASSIGN = 275,
     ADD = 276,
     MINUS = 277,
     MULTIPLY = 278,
     DIVIDE = 279,
     EQUAL = 280,
     INEQUAL = 281,
     LESSTHAN = 282,
     GREATERTHAN = 283,
     LEFTPAR = 284,
     RIGHTPAR = 285,
     SEMICOLON = 286,
     COMMA = 287,
     LESSEQUAL = 288,
     GREATEREQUAL = 289,
     INTLITERAL = 290,
     FLOATLITERAL = 291,
     STRINGLITERAL = 292,
     IDENTIFIER = 293
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 35 "src/Micro.y"

	int int_val;
	float float_val;
	char* str;



/* Line 1676 of yacc.c  */
#line 99 "generated/Micro.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


