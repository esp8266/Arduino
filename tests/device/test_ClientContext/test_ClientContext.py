#!/usr/bin/env python3

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
                print ('trying port %d' %port, file=sys.stderr)
                server_address = ("0.0.0.0", port)
                sock.bind(server_address)
                sock.listen(1)
                running = True
                break
            except Exception:
                print >>sys.stderr, 'busy'
        if not running:
            return
        print ('starting up on %s port %s' % server_address, file=sys.stderr)
        print ( 'waiting for connections', file=sys.stderr)
        while running:
            print ('loop', file=sys.stderr)
            readable, writable, errored = select.select([sock], [], [], 1.0)
            if readable:
                connection, client_address = sock.accept()
                try:
                    print('client connected: %s' % str(client_address), file=sys.stderr)
                finally:
                    print ('close', file=sys.stderr)
                    connection.shutdown(socket.SHUT_RDWR)
                    connection.close()

    thread = Thread(target=run)
    thread.start()

@teardown('WiFi release ClientContext')
def teardown_tcpsrv(e):
    
    global thread
    global running
    
    print ('closing', file=sys.stderr)
    running = False
    thread.join()
    return 0
