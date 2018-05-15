#!/usr/bin/python

# This script pulls the list of Mozilla trusted certificate authorities
# from the web at the "mozurl" below, parses the file to grab the PEM
# for each cert, and then generates DER files in a new ./data directory
# Upload these to a SPIFFS filesystem and use the CertManager to parse
# and use them for your outgoing SSL connections.
#
# Script by Earle F. Philhower, III.  Released to the public domain.

import csv
from os import mkdir
from subprocess import Popen, PIPE
import urllib2
try:
    # for Python 2.x
    from StringIO import StringIO
except ImportError:
    # for Python 3.x
    from io import StringIO

# Mozilla's URL for the CSV file with included PEM certs
mozurl = "https://ccadb-public.secure.force.com/mozilla/IncludedCACertificateReportPEMCSV"

# Load the manes[] and pems[] array from the URL
names = []
pems = []
response = urllib2.urlopen(mozurl)
csvData = response.read()
csvReader = csv.reader(StringIO(csvData))
for row in csvReader:
    names.append(row[0]+":"+row[1]+":"+row[2])
    pems.append(row[28])
del names[0] # Remove headers
del pems[0] # Remove headers

# Try and make ./data, skip if present
try:
    os.mkdir("data")
except:
    pass

# Process the text PEM using openssl into DER files
for i in range(0, len(pems)):
    certName = "data/ca_%03d.der" % (i);
    thisPem = pems[i].replace("'", "")
    print names[i] + " -> " + certName
    pipe = Popen(['openssl','x509','-inform','PEM','-outform','DER','-out', certName], shell = False, stdin = PIPE).stdin
    pipe.write(thisPem)
    pipe.close

