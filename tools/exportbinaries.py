#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import os
import sys
import shutil

def parse_args():
  parser = argparse.ArgumentParser( description='export generated binaries')
  parser.add_argument( '-src', action = 'store' )
  parser.add_argument( '-dst', action = 'store' )
  parser.add_argument( '-bin', action = 'store' )
  parser.add_argument( '-binSigned', action = 'store' )
  return parser.parse_args()

def main():
  args = parse_args()

  srcBin = args.src + args.bin
  srcBinSigned = args.src + args.binSigned
  dstBin = args.dst + args.bin
  dstBinSigned = args.dst + args.binSigned
  
  if not ( os.path.isfile( srcBin ) and os.path.isfile( srcBinSigned ) ):
    sys.stderr.write( "Neither '%s' nor '%s' exist in Path '%s'\n" % ( srcBin, srcBinSigned, args.src ) )
    return 1

  if not os.path.exists( args.dst ):
    os.makedirs( args.dst )
    
  res = 0
  if os.path.isfile( srcBin ):
    try:
      shutil.copyfile( srcBin, dstBin )
      sys.stderr.write( "File '%s' copied to '%s'\n" % ( args.bin, args.dst ) )
    except:
      res = 2
      sys.stderr.write( "Error while copying File '%s' to '%s'\n" % ( args.bin, args.dst ) )

  if os.path.isfile( srcBinSigned ):
    try:
      shutil.copyfile( srcBinSigned, dstBinSigned )
      sys.stderr.write( "File '%s' copied to '%s'\n" % ( args.binSigned, args.dst ) )
    except:
      res = 2
      sys.stderr.write( "Error while copying File '%s' to '%s'\n" % ( args.binSigned, args.dst ) )

  return res


if __name__ == '__main__':
  sys.exit( main() )

