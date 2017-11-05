from mock_decorators import setup, teardown
from threading import Thread
import socket
import time

stop_client_thread = False
client_thread = None

@setup('Simple echo server')
def setup_echo_server(e):
    global stop_client_thread
    global client_thread
    def echo_client_thread():
        server_address = socket.gethostbyname('esp8266-wfs-test.local')
        count = 0
        while count < 5 and not stop_client_thread:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((server_address, 5000))
            sock.settimeout(1.0)
            buf = 'a' * 1023 + '\n'
            sock.sendall(buf)
            data = ''
            retries = 0
            while len(data) < 1024 and retries < 3:
                data += sock.recv(1024)
                retries += 1
            print 'Received {} bytes'.format(len(data))
            if len(data) != 1024:
                raise RuntimeError('client failed to receive response')
            count += 1
        
    stop_client_thread = False
    client_thread = Thread(target=echo_client_thread)
    client_thread.start()

@teardown('Simple echo server')
def teardown_echo_server(e):
    global stop_client_thread
    stop_client_thread = True
    client_thread.join()

    
