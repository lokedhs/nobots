%{
/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-2002  Elias Martenson (elias-m@algonet.se)
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

%}

SYMBOL		[a-zA-Z_][a-zA-Z0-9_]*

%%

"#".*\n		{
			src_line++;
		}

0x[0-9A-Fa-f]+  {
			yylval.i = strtol( yytext, NULL, 16 );
			return INTEGER;
		}

0[0-7]+	{
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

array		{
			return ARRAY;
		}

func		{
			return FUNC;
		}

cfunc		{
			return CFUNC;
		}

safe		{
			return SAFE;
		}

callback	{
			return CALLBACK;
		}

getcallback	{
			return GETCALLBACK;
		}

print		{
			return PRINT;
		}

while		{
			return WHILE;
		}

if		{
			return IF;
		}

else		{
			return ELSE;
		}

random		{
			return RANDOM;
		}

getcb		{
			return GETCB;
		}

return		{
			return RETURN;
		}



getspeed|getheading|getxcoordinate|getycoordinate|getmaxspeed|uptime|c_to_a|c_to_r|ar_to_x|ar_to_y|scan|fire|getscannerstatus|getweaponstatus|send|addtimer	{
			strcpy( yylval.s, yytext );
			return ROBOTCOM_RETONE;
		}

setspeed|setheading|wallscan|receieve|discard|deltimer		{
			strcpy( yylval.s, yytext );
			return ROBOTCOM_RETZERO;
		}





{SYMBOL}	{
			strcpy( yylval.s, yytext );
			return SYMBOL;
		}

"++"		{
			return INC;
		}

"--"		{
			return DEC;
		}

"<="		{
			return LE;
		}

">="		{
			return GE;
		}

"=="		{
			return EQ;
		}

"!="		{
			return NE;
		}

"&&"		{
			return AND;
		}

"||"		{
			return OR;
		}

"+="		{
			return SETADD;
		}

"-="		{
			return SETSUB;
		}

"*="		{
			return SETMUL;
		}

"/="		{
			return SETDIV;
		}

[ \t]+		{ /* ignore whitespace */ }

"\n"		{
			src_line++;
		}

[][(){}!%&=,;:<>+*/-]	{
			return *yytext;
		}

.		{
			compiler_error( "illegal input character" );
		}
