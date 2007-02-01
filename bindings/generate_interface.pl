#!/usr/bin/perl -w

#
#  Copyright(C) 2006 Cameron Rich
#
#  This library is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

#===============================================================
# This application transforms ssl.h into interfaces that can be used by 
# other language bindings. It is "SWIG"-like in nature in that various 
# files are generated based on the axTLS API.
#
# The file produced is axInterface.? (depending on the file extension).
#
#===============================================================

use strict;

my $CSHARP = 0;
my $VBNET = 1;

my $binding;
my $skip = 0;
my $signature_ret_type;

# Transforms function signature into an Interface format
sub transformSignature
{
    my $item;
    my ($line) = @_;

    foreach $item ($line)
    { 
        # our very basic preprocessor
        if ($binding == $CSHARP)
        {
            $line =~ s/STDCALL //;
            $line =~ s/EXP_FUNC/        [DllImport ("axtls")]\n        public static extern/;
            $line =~ s/uint32_t/uint/g;
            $line =~ s/uint8_t \*\*/ref IntPtr /g;
            $line =~ s/const uint8_t \* /IntPtr /g;
            $line =~ s/const uint8_t \*/byte[] /g;    # note: subtle diff 
            $line =~ s/uint8_t \* ?/byte[] /g;
            $line =~ s/uint8_t ?/byte /g;
            $line =~ s/const char \* ?/string /g;
            $line =~ s/SSLCTX \* ?/IntPtr /g;
            $line =~ s/SSLObjLoader \* ?/IntPtr /g;
            $line =~ s/SSL \* ?/IntPtr /g;
            $line =~ s/\(void\)/()/g;
        }
        elsif ($binding == $VBNET)
        {
            if ($line =~ /EXP_FUNC/)
            {
                # Procedure or function?
                my $invariant = $line =~ /void /;

                my $proc = $invariant ? "Sub" : "Function";
                ($signature_ret_type) = $line =~ /EXP_FUNC (.*) STDCALL/;
                $line =~ s/EXP_FUNC .* STDCALL /        <DllImport("axtls")> Public Shared $proc _\n            /;

                $signature_ret_type =~ s/const uint8_t \*/As IntPtr/;
                $signature_ret_type =~ s/const char \*/As String/;
                $signature_ret_type =~ s/SSLCTX \*/As IntPtr/;
                $signature_ret_type =~ s/SSLObjLoader \*/As IntPtr/;
                $signature_ret_type =~ s/SSL \*/As IntPtr/;
                $signature_ret_type =~ s/uint8_t/As Byte/;
                $signature_ret_type =~ s/int/As Integer/;
                $signature_ret_type =~ s/void//;
                $signature_ret_type .= "\n        End $proc\n\n";
            }

            $line =~ s/uint32_t (\w+)/ByVal $1 As Integer/g;
            $line =~ s/int (\w+)/ByVal $1 As Integer/g;
            $line =~ s/uint8_t \*\* ?(\w+)/ByRef $1 As IntPtr/g;
            $line =~ s/const uint8_t \* ?(\w+)/ByVal $1() As Byte/g;
            $line =~ s/uint8_t \* ?(\w+)/ByVal $1() As Byte/g;
            $line =~ s/const char \* ?(\w+)/ByVal $1 As String/g;
            $line =~ s/SSLCTX \* ?(\w+)/ByVal $1 As IntPtr/g;
            $line =~ s/SSLObjLoader \* ?(\w+)/ByVal $1 As IntPtr/g;
            $line =~ s/SSL \* ?(\w+)/ByVal $1 As IntPtr/g;
            $line =~ s/void \* ?(\w+)/Byval $1 As IntPtr/g;
            $line =~ s/\(void\)/()/g;
            $line =~ s/void//g;
            $line =~ s/;\n/ $signature_ret_type;/;
        }
    }

    return $line;
}

# Parse input file
sub parseFile
{
    my (@file) = @_;
    my $line;
    my $splitDefine = 0;
    my $splitFunctionDeclaration;
    my $vb_hack = " ";
    my $vb_line_hack = 0;

    $skip = 0;

    foreach $line (@file)
    {
        # test for a #define
        if (!$skip && $line =~ m/^#define/)
        {
            $splitDefine = 1 if $line =~ m/\\$/;

            if ($binding == $VBNET)
            {
                $line =~ s/\|/Or/g;
                $line =~ s/ 0x/ &H/;
            }

            my ($name, $value) = $line =~ /#define (\w+) +([^\\]*)[\\]?\n/;

            if (defined $name && defined $value)
            {
                # C# constant translation
                if ($binding == $CSHARP)
                {
                    $line = "        public const int $name = $value";
                }
                # VB.NET constant translation
                elsif ($binding == $VBNET)
                {
                    $line = "        Public Const $name As Integer = $value";
                }
            }

            next if $line =~ /#define/;  # ignore any other defines
               
            print DATA_OUT $line;

            # check line is not split
            next if $splitDefine == 1;
            print DATA_OUT ";" if $binding == $CSHARP;
            print DATA_OUT "\n";
        }

        # pick up second line of #define statement
        if ($splitDefine) 
        {
            if ($line !~ /\\$/)
            {
                $line =~ s/$/;/ if $binding == $CSHARP;        # add the ";"
            }

            $line =~ s/ ?\| ?/ Or /g 
                                if ($binding == $VBNET);

            # check line is not split
            $splitDefine = ($line =~ m/\\$/);

            # ignore trailing "\"
            $line =~ s/\\$// if $binding == $CSHARP;
            $line =~ s/\\$/_/ if $binding == $VBNET;
            print DATA_OUT $line;
            next;
        } 

        # test for function declaration
        if (!$skip && $line =~ /EXP_FUNC/ && $line !~ /\/\*/)
        {
            $line = transformSignature($line);
            $splitFunctionDeclaration = $line !~ /;/;
            $line =~ s/;// if ($binding == $VBNET);
            $line =~ s/\n$/ _\n/ if ($binding == $VBNET) && 
                                                $splitFunctionDeclaration;
            print DATA_OUT $line;
            next;
        }

        if ($splitFunctionDeclaration) 
        {
            $line = transformSignature($line);
            $splitFunctionDeclaration = $line !~ /;/;
            $line =~ s/;// if ($binding == $VBNET);
            $line =~ s/\n/ _\n/ if ($binding == $VBNET) && 
                                                $splitFunctionDeclaration == 1;
            print DATA_OUT $line;
            next;
        }
    }
}

#===============================================================

# Determine which module to build from command-line options
use strict;
use Getopt::Std;

my $binding_prefix;
my $binding_suffix;
my $data_file;
my @raw_data;

if (not defined  $ARGV[0])
{
    goto ouch;
}

if ($ARGV[0] eq "-csharp")
{
    print "Generating C# interface file\n";
    $binding_prefix = "csharp";
    $binding_suffix = "cs";
    $binding = $CSHARP;
}
elsif ($ARGV[0] eq "-vbnet")
{
    print "Generating VB.NET interface file\n";
    $binding_prefix = "vbnet";
    $binding_suffix = "vb";
    $binding = $VBNET;
}
else
{
ouch:
    die "Usage: $0 [-csharp | -vbnet]\n";
}

my $interfaceFile = "$binding_prefix/axInterface.$binding_suffix";

# Input file required to generate interface file.
$data_file = "../ssl/ssl.h";

# Open input files
open(DATA_IN, $data_file) || die("Could not open file ($data_file)!");
@raw_data = <DATA_IN>;


# Open output file
if ($binding == $CSHARP || $binding == $VBNET)
{
    open(DATA_OUT, ">$interfaceFile") || die("Cannot Open File");
}

# SPEC interface file header
if ($binding == $CSHARP)
{
    # generate the C#/C interface file
    print DATA_OUT << "END";
// The C# to C interface definition file for the axTLS project
// Do not modify - this file is generated

using System;
using System.Runtime.InteropServices;

namespace axTLS
{
    public class axtls
    {
END
}
elsif ($binding == $VBNET)
{
    # generate the VB.NET/C interface file
    print DATA_OUT << "END";
' The VB.NET to C interface definition file for the axTLS project
' Do not modify - this file is generated

Imports System
Imports System.Runtime.InteropServices

Namespace axTLSvb
    Public Class axtls
END
}

parseFile(@raw_data);

# finish up
if ($binding == $CSHARP)
{
    print DATA_OUT "    };\n";
    print DATA_OUT "};\n";
}
elsif ($binding == $VBNET)
{
    print DATA_OUT "    End Class\nEnd Namespace\n";
}

close(DATA_IN);
close(DATA_OUT);

#===============================================================

