#!/usr/bin/perl -w

$basedir = 'src/processing/core';

@contents = ();

# next slurp methods from PGraphics
open(F, "$basedir/PGraphics.java") || die $!;
foreach $line (<F>) {
    push @contents, $line;
}
close(F);

# PGraphics subclasses PImage.. now get those methods
open(F, "$basedir/PImage.java") || die $!;
foreach $line (<F>) {
    push @contents, $line;
}
close(F);

#open(DEBUG, ">debug.java") || die $!;
#print DEBUG @contents;
#close(DEBUG);
#exit;


open(APPLET, "$basedir/PApplet.java") || die $!;
@applet = <APPLET>;
close(APPLET);


$insert = 'public functions for processing.core';

# an improved version of this would only rewrite if changes made
open(OUT, ">$basedir/PApplet.new") || die $!;
foreach $line (@applet) {
    print OUT $line;
    last if ($line =~ /$insert/);
}

$comments = 0;

while ($line = shift(@contents)) {
    $decl = "";

    if ($line =~ /\/\*/) {
        $comments++;
	#print "+[$comments] $line";
    }
    if ($line =~ /\*\//) {
        $comments--;
	#print "-[$comments] $line";
    }
    next if ($comments > 0);

    $got_something = 0;  # so it's ugly, back off
    $got_static = 0;
    $got_interface = 0;

    if ($line =~ /^\s*public ([\w\[\]]+) [a-zA-z_]+\(.*$/) {
        $got_something = 1;
	$got_interface = 1;
    } elsif ($line =~ /^\s*abstract public ([\w\[\]]+) [a-zA-z_]+\(.*$/) {
        $got_something = 1;
    } elsif ($line =~ /^\s*public final ([\w\[\]]+) [a-zA-z_]+\(.*$/) {
        $got_something = 1;
    } elsif ($line =~ /^\s*static public ([\w\[\]]+) [a-zA-z_]+\(.*$/) {
        $got_something = 1;
	$got_static = 1;
    }
    # if function is marked "// ignore" then, uh, ignore it.
    if (($got_something == 1) && ($line =~ /\/\/ ignore/)) {
	$got_something = 0;
    }
    #if ($line =~ /^\s*public (\w+) [a-zA-z_]+\(.*$/) {
    if ($got_something == 1) {
        if ($1 ne 'void') {
            $returns = 'return ';
        } else {
            $returns = '';
        }

#	if ($line =~ /^(\s+)abstract\s+([^;]+);/) {
#	    $line = $1 . $2 . " {\n";
#	    #print "found $1\n";
#	    # hrm
#	}
	# remove the 'abstract' modifier
	$line =~ s/\sabstract\s/ /;

	# replace semicolons with a start def
	$line =~ s/\;\s*$/ {\n/;

        print OUT "\n\n$line";

#	if ($got_interface == 1) {
#	    $iline = $line;
#	    $iline =~ s/ \{/\;/;
##	    print INTF "\n$iline";
#	}

        $decl .= $line;
        while (!($line =~ /\)/)) {
            $line = shift (@contents);
            $decl .= $line;
	    $line =~ s/\;\s*$/ {\n/;
            print OUT $line;

#	    if ($got_interface == 1) {
#		$iline = $line;
#		$iline =~ s/ \{/\;/;
##		print INTF $iline;
#	    }
        }
	
	#$g_line = '';
	#$r_line = '';

        $decl =~ /\s(\S+)\(/;
        $decl_name = $1;
	if ($got_static == 1) {
	    #print OUT "    ${returns}PGraphics.${decl_name}(";
	    $g_line = "    ${returns}PGraphics.${decl_name}(";
	} else {
	    #if ($returns eq '') {
	    #print OUT "    if (recorder != null) recorder.${decl_name}(";
	    $r_line = "    if (recorder != null) recorder.${decl_name}(";
	    #}
	    #print OUT "    ${returns}g.${decl_name}(";
	    $g_line = "    ${returns}g.${decl_name}(";
	}

        $decl =~ s/\s+/ /g; # smush onto a single line
        $decl =~ s/^.*\(//;
        $decl =~ s/\).*$//;

        $prev = 0;
        @parts = split(', ', $decl);
        foreach $part (@parts) {
            #($the_type, $the_arg) = split(' ', $part);
	    @blargh = split(' ', $part);
	    $the_arg = $blargh[1];
            $the_arg =~ s/[\[\]]//g;
            if ($prev != 0) {
                #print OUT ", ";
		$g_line .= ", ";
		$r_line .= ", ";
            }
            #print OUT "${the_arg}";
	    $g_line .= "${the_arg}";
	    $r_line .= "${the_arg}";
            $prev = 1;
        }
        #print OUT ");\n";
	$g_line .= ");\n";
	$r_line .= ");\n";

	if (($got_static != 1) && ($returns eq '')) {
	    print OUT $r_line;
	}
	print OUT $g_line;
        print OUT "  }\n";
    }
}
print OUT "}\n";
#print INTF "}\n";

close(OUT);
#close(INTF);

$oldguy = join(' ', @applet);

open(NEWGUY, "$basedir/PApplet.new") || die $!;
@newbie = <NEWGUY>;
$newguy = join(' ', @newbie);
close(NEWGUY);

if ($oldguy ne $newguy) {
    # replace him
    print "updating PApplet with PGraphics api changes\n";
    `mv $basedir/PApplet.new $basedir/PApplet.java`;
} else {
    # just kill the new guy
    #print "no changes to applet\n";
    `rm -f $basedir/PApplet.new`;
}
