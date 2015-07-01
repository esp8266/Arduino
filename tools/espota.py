#!/usr/bin/env python
#
# this script will push an OTA update to the ESP
#
# use it like: python ota_server.py <ESP_IP_address> <sketch.bin>
#
# on the ESP side you need code like this: https://gist.github.com/igrr/43d5c52328e955bb6b09 to handle the update
#
 
from __future__ import print_function
import socket
import sys
import os
 
def serve(remoteAddr, filename):
  # Create a TCP/IP socket
  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  serverPort = 48266
  server_address = ('0.0.0.0', serverPort)
  print('starting up on %s port %s' % server_address, file=sys.stderr)
  sock.bind(server_address)
  sock.listen(1)
 
  sock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  remote_address = (remoteAddr, 8266)
  content_size = os.path.getsize(filename)
  print('upload size: %d' % content_size, file=sys.stderr)
  message = '%d %d %d\n' % (0, serverPort, content_size)
 
  while True:
    # Wait for a connection
    print('sending invitation', file=sys.stderr)
    sent = sock2.sendto(message, remote_address)
    sock.settimeout(10)
    print('waiting...', file=sys.stderr)
    connection, client_address = sock.accept()
    sock.settimeout(None)
    connection.settimeout(None)
    try:
      print('connection from', client_address, file=sys.stderr)
 
      print('sending file %s\n' % filename, file=sys.stderr)
      f = open(filename, "rb")
 
      while True:
        chunk = f.read(4096)
        if not chunk:
          break
          
        sys.stderr.write('.')
        sys.stderr.flush()
        #print('sending %d' % len(chunk), file=sys.stderr)
        connection.sendall(chunk)
 
      print('\ndone!', file=sys.stderr)
      return 0
        
    finally:
      connection.close()
      f.close()
    return 1
 
def main(args):
  return serve(args[1], args[2])
 
 
 
if __name__ == '__main__':
  sys.exit(main(sys.argv))
 
