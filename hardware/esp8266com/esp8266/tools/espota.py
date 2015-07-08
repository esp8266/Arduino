#!/usr/bin/env python
#
# this script will push an OTA update to the ESP
# use it like: python espota.py <ESP_IP_address> <sketch.bin>
 
from __future__ import print_function
import socket
import sys
import os
 
def serve(remoteAddr, remotePort, filename):
  # Create a TCP/IP socket
  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  serverPort = 48266
  server_address = ('0.0.0.0', serverPort)
  print('Starting on %s:%s' % server_address, file=sys.stderr)
  try:
    sock.bind(server_address)
    sock.listen(1)
  except:
    print('Listen Failed', file=sys.stderr)
    return 1
 
  content_size = os.path.getsize(filename)
  print('Upload size: %d' % content_size, file=sys.stderr)
  message = '%d %d %d\n' % (0, serverPort, content_size)
 
  # Wait for a connection
  print('Sending invitation to:', remoteAddr, file=sys.stderr)
  sock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  remote_address = (remoteAddr, int(remotePort))
  sent = sock2.sendto(message, remote_address)
  sock2.close()
  
  print('Waiting for device...\n', file=sys.stderr)
  try:
    sock.settimeout(10)
    connection, client_address = sock.accept()
    sock.settimeout(None)
    connection.settimeout(None)
  except:
    print('No response from device', file=sys.stderr)
    sock.close()
    return 1

  try:
    f = open(filename, "rb")
    sys.stderr.write('Uploading')
    sys.stderr.flush()
    while True:
      chunk = f.read(1460)
      if not chunk: break
      sys.stderr.write('.')
      sys.stderr.flush()
      connection.settimeout(10)
      try:
        connection.sendall(chunk)
        res = connection.recv(4)
      except:
        print('\nError Uploading', file=sys.stderr)
        connection.close()
        f.close()
        sock.close()
        return 1

    print('\nWaiting for result...\n', file=sys.stderr)
    try:
      connection.settimeout(60)
      data = connection.recv(32)
      print('Result: %s' % data, file=sys.stderr)
      connection.close()
      f.close()
      sock.close()
      return 0
    except:
      print('Result: No Answer!', file=sys.stderr)
      connection.close()
      f.close()
      sock.close()
      return 1

  finally:
    connection.close()
    f.close()

  sock.close()
  return 1
 
def main(args):
  return serve(args[1], args[2], args[3])
 
 
 
if __name__ == '__main__':
  sys.exit(main(sys.argv))
 
