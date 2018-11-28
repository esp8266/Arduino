from mock_decorators import setup, teardown
from flask import Flask, request
from threading import Thread
import urllib2
import os
import ssl
import time

@setup('HTTP GET & POST requests')
def setup_http_get(e):
    app = Flask(__name__)
    def shutdown_server():
        func = request.environ.get('werkzeug.server.shutdown')
        if func is None:
            raise RuntimeError('Not running with the Werkzeug Server')
        func()
    @app.route('/shutdown')
    def shutdown():
        shutdown_server()
        return 'Server shutting down...'
    @app.route("/", methods = ['GET', 'POST'])
    def root():
        print('Got data: ' + request.data);
        return 'hello!!!'
    @app.route("/data")
    def get_data():
        size = int(request.args['size'])
        return 'a'*size
    def flaskThread():
        app.run(host='0.0.0.0', port=8088)    
    th = Thread(target=flaskThread)
    th.start()

@teardown('HTTP GET & POST requests')
def teardown_http_get(e):
    response = urllib2.urlopen('http://localhost:8088/shutdown')
    html = response.read()
    time.sleep(30)


@setup('HTTPS GET request')
def setup_http_get(e):
    app = Flask(__name__)
    def shutdown_server():
        func = request.environ.get('werkzeug.server.shutdown')
        if func is None:
            raise RuntimeError('Not running with the Werkzeug Server')
        func()
    @app.route('/shutdown')
    def shutdown():
        shutdown_server()
        return 'Server shutting down...'
    @app.route("/")
    def root():
        return 'hello!!!'
    @app.route("/data")
    def get_data():
        size = int(request.args['size'])
        return 'a'*size
    def flaskThread():
        p = os.path.dirname(os.path.abspath(__file__))
        context = (p + '/server.crt', p + '/server.key')
        print(context)
        app.run(host='0.0.0.0', port=8088, ssl_context=context)    
    th = Thread(target=flaskThread)
    th.start()

@teardown('HTTPS GET request')
def teardown_http_get(e):
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    p = os.path.dirname(os.path.abspath(__file__))
    response = urllib2.urlopen('https://localhost:8088/shutdown', context=ctx)
    html = response.read()

