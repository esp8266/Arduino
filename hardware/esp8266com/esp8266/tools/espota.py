#!/usr/bin/env python
#
# this script will push an OTA update to the ESP
#
# use it like: python ota_server.py <ESP_IP_address> <sketch.bin>
#
# on the ESP side you need code like this: https://gist.github.com/igrr/43d5c52328e955bb6b09 to handle the update
#
 
import socket
import sys
import os
 
def serve(remoteAddr, filename):
  # Create a TCP/IP socket
  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  serverPort = 48266
  server_address = ('0.0.0.0', serverPort)
  print >>sys.stderr, 'starting up on %s port %s' % server_address
  sock.bind(server_address)
  sock.listen(1)
 
  sock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  remote_address = (remoteAddr, 8266)
  content_size = os.path.getsize(filename)
  print >>sys.stderr, 'upload size: %d' % content_size
  message = '%d %d %d\n' % (0, serverPort, content_size)
  print >>sys.stderr, 'sending invitation %s' % message
  sent = sock2.sendto(message, remote_address)
  sent = sock2.sendto(message, remote_address)
  sent = sock2.sendto(message, remote_address)
 
 
  while True:
    # Wait for a connection
    print >>sys.stderr, 'waiting for connection'
    connection, client_address = sock.accept()
    try:
      print >>sys.stderr, 'connection from', client_address
 
      print >>sys.stderr, 'opening file %s' % filename
      f = open(filename, "rb")
 
      while True:
        chunk = f.read(4096)
        if not chunk:
          break
 
        print >>sys.stderr, 'sending %d' % len(chunk)
        connection.sendall(chunk)
 
      print >>sys.stderr, 'done!'
      return 0
        
    finally:
      connection.close()
      f.close()
    return 1
 
def main(args):
  return serve(args[1], args[2])
 
 
 
if __name__ == '__main__':
  sys.exit(main(sys.argv))
 
