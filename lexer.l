%{
/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-98  Elias Martenson (elias.martenson@sun.se)
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

extern int src_line;

%}

SYMBOL		[a-zA-Z_][a-zA-Z0-9_]*

%s list

%%

<INITIAL>("#".*)?\n  {
			src_line++;
			return STMT_SEP;
		}

<list>("#".*)*\n  {
			src_line++;
		}

"$"array	{
			return ARRAY;
		}

"$"define	{
			return DEFINE;
		}

";"		{
			return STMT_SEP;
		}

0x[0-9A-Fa-f]+  {
			yylval.i = strtol( yytext, NULL, 16 );
			return INTEGER;
		}

0[0-7]+		{
			yylval.i = strtol( yytext, NULL, 8 );
			return INTEGER;
		}

[1-9][0-9]*	{
			yylval.i = atoi( yytext );
			return INTEGER;
		}

"0"		{
			yylval.i = 0;
			return INTEGER;
		}

"%"{SYMBOL}	{
			strcpy( yylval.s, yytext + 1 );
			return IMMSYMBOL;
		}

"&"{SYMBOL}	{
			strcpy( yylval.s, yytext + 1 );
			return INDSYMBOL;
		}

{SYMBOL}	{
			strcpy( yylval.s, yytext );
			return SYMBOL;
		}

[ \t]+		{ /* ignore whitespace */ }

"["		{
			BEGIN(list);
			return '[';
		}

<list>"]"	{
			BEGIN(0);
			return ']';
		}

[%&,()+*/:=-]	{
			return *yytext;
		}

.		{
			asm_error( "illegal input character" );
		}
