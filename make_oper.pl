#! /usr/bin/perl
#    robot assembler - robot assembly language
#    Copyright (C) 1996  Elias Martenson (elias.martenson@sun.se)
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

open MAINFILE, "<operations.def" or die "Can't open operations.def";
open OUTFUNC, ">oper_functions.c" or die "Can't open oper_functions.c";
open OUTSYN, ">oper_syntax.c" or die "Can't open oper_syntax.c";

print OUTFUNC "/* Automatically generated from operations.def, DO NOT EDIT */\n\n";
print OUTFUNC "typedef int CodeOperation( Program *, int *, int * );\n\n";

print OUTSYN "/* Automatically generated from operations.def, DO NOT EDIT */\n\n";


undef @code_tab;
$func_tab = "";
$syntax_tab = "";
$first = 1;

@handles = (MAINFILE);

while( scalar(@handles) > 0 ) {
    $infile = pop(@handles);

    while( <$infile> ) {
	chomp;
	next if /^\!/;		# Lines beginning with a ! are comments
	next if $first && /^\s*$/; # Strip blank lines before the first function
	if( /^\s*OPER\s+([a-z0-9_]+)\s+([0-9]+)\s*$/ ) {
	    $funcname = $1;
	    print "adding function $funcname\n";
	    $first = 0;
	    push @code_tab, "oper_$funcname";
	    $syntax_tab .= "\t{ \"$funcname\", $2 },\n";
	    $func_tab .= "\nstatic int oper_$funcname( Program *program, int *arg0, int *arg1 )\n";
	} elsif( /^\s*INCLUDE\s+([^\s]+)\s*$/ ) {
	    push( @handles, $infile );
	    open FH, "<$1"
		or die "can't open $1\n";
	    $infile = FH;
	} else {
	    if( $first ) {
		die "illegal format";
	    }
	    $func_tab .= "$_\n";
	}
    }
    close $infile;
}


print OUTFUNC $func_tab;
print OUTFUNC "\nstatic CodeOperation *code_tab[] = {\n";
foreach $oper (@code_tab) {
    print OUTFUNC "\t$oper,\n";
}
print OUTFUNC "};\n";
close OUTFUNC;

print OUTSYN "static SyntaxTableEntry syntax_table[] = {\n";
print OUTSYN $syntax_tab;
print OUTSYN "};\n";
close OUTSYN;
