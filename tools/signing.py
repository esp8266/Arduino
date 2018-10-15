#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
import sys
import argparse
import subprocess
import hashlib

def parse_args():
    parser = argparse.ArgumentParser(description='Binary signing tool')
    parser.add_argument('-m', '--mode', help='Mode (header, sign)')
    parser.add_argument('-b', '--bin', help='Unsigned binary')
    parser.add_argument('-o', '--out', help='Output file');
    parser.add_argument('-p', '--publickey', help='Public key file');
    parser.add_argument('-s', '--privatekey', help='Private(secret) key file');
    return parser.parse_args()


def main():
    args = parse_args()
    if args.mode == "header":
        val = ""
        try:
            with open(args.publickey, "rb") as f:
                pub = f.read()
                val += "#include <pgmspace.h>\n"
                val += "#define ARDUINO_SIGNING 1\n"
                val += "static const char signing_pubkey[] PROGMEM = {\n"
                for i in pub:
                    val += "0x%02x, \n" % ord(i)
                val = val[:-3]
                val +="\n};\n"
                sys.stderr.write("Enabling binary signing\n")
        except:
            sys.stderr.write("Not enabling binary signing\n")
            val += "#define ARDUINO_SIGNING 0\n"
        with open(args.out, "w") as f:
            f.write(val)
        return 0
    elif args.mode == "sign":
        val = ""
        try:
            with open(args.bin, "rb") as b:
                bin = b.read()
                sha256 = hashlib.sha256(bin)
                sys.stderr.write("Signing SHA256 = " + sha256.hexdigest() + "\n");
                signcmd = [ 'openssl', 'rsautl', '-sign', '-inkey', args.privatekey ]
                proc = subprocess.Popen(signcmd, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
                signout = proc.communicate(input=sha256.digest())[0]
                with open(args.out, "wb") as out:
                    out.write(bin)
                    out.write(signout)
                    out.write(b'\x00\x01\x00\x00')
                    sys.stderr.write("Signed binary: " + args.out + "\n")
        except:
            sys.stderr.write("Not signing the generated binary\n")
        return 0
    else:
        sys.stderr.write("ERROR: Mode not specified as header or sign\n")

if __name__ == '__main__':
    sys.exit(main())

