from mock_decorators import setup, teardown
from flask import Flask, request
from threading import Thread
import socket
import select
import sys
import os

@setup('WiFi release ClientContext')
def setup_tcpsrv(e):

    global thread

    app = Flask(__name__)

    def run():

        global running

        running = False
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        for port in range(8266, 8285 + 1):
            try:
                print >>sys.stderr, 'trying port', port
                server_address = ("0.0.0.0", port)
                sock.bind(server_address)
                sock.listen(1)
                running = True
                break
            except Exception:
                print >>sys.stderr, 'busy'
        if not running:
            return
        print >>sys.stderr, 'starting up on %s port %s' % server_address
        print >>sys.stderr, 'waiting for connections'
        while running:
            print >>sys.stderr, 'loop'
            readable, writable, errored = select.select([sock], [], [], 1.0)
            if readable:
                connection, client_address = sock.accept()
                try:
                    print >>sys.stderr, 'client connected:', client_address
                finally:
                    print >>sys.stderr, 'close'
                    connection.shutdown(socket.SHUT_RDWR)
                    connection.close()

    thread = Thread(target=run)
    thread.start()

@teardown('WiFi release ClientContext')
def teardown_tcpsrv(e):
    
    global thread
    global running
    
    print >>sys.stderr, 'closing'
    running = False
    thread.join()
    return 0
