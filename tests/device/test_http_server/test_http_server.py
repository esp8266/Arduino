from collections import OrderedDict
from mock_decorators import setup, teardown
from threading import Thread
from poster3.encode import MultipartParam
from poster3.encode import multipart_encode
from poster3.streaminghttp import register_openers
import sys
import urllib

def http_test(res, url, get=None, post=None):
    try:
        print('a', file=sys.stderr)
        if get:
            url += '?' + urllib.parse.urlencode(get)
        print('b', file=sys.stderr)
        if post:
            print('post1=', post, file=sys.stderr)
            post = bytes(urllib.parse.quote(bytes(str(post).encode('utf-8'))).encode('utf-8'))
            print('post2=', post, file=sys.stderr)
        print('c url=', url, '  post=', post, file=sys.stderr)
        request = urllib.request.urlopen(url, post, 2)
        print('d', file=sys.stderr)
        response = request.read()
        print('e', file=sys.stderr)
        print('response=', response, ' ?res=', res, file=sys.stderr)
    except Exception as e:
        print('http_test: Exception: ', e, file=sys.stderr)
        return 1
    if response != res:
        return 1
    return 0

@setup('HTTP GET Parameters')
def setup_http_get_params(e):
    def testRun():
        return http_test('var1 = val with spaces\nva=r+ = so&me%', 'http://etd.local/get', OrderedDict([('var1', 'val with spaces'), ('va=r+', 'so&me%')]))
    Thread(target=testRun).start()

@teardown('HTTP GET Parameters')
def teardown_http_get_params(e):
    return 0

@setup('HTTP POST Parameters')
def setup_http_post_params(e):
    def testRun():
        return http_test('var2 = val with spaces', 'http://etd.local/post', None, {'var2' : 'val with spaces'})
    Thread(target=testRun).start()

@teardown('HTTP POST Parameters')
def teardown_http_post_params(e):
    return 0

@setup('HTTP GET+POST Parameters')
def setup_http_getpost_params(e):
    def testRun():
        return http_test('var3 = val with spaces\nva&r+ = so=me%', 'http://etd.local/get_and_post', {'var3' : 'val with spaces'}, {'va&r+' : 'so=me%'})
    Thread(target=testRun).start()

@teardown('HTTP GET+POST Parameters')
def teardown_http_getpost_params(e):
    return 0

@setup('HTTP Upload')
def setup_http_upload(e):
    def testRun():
        response = ''
        try:
            print('f', file=sys.stderr)
            register_openers()
            print('g', file=sys.stderr)
            p = MultipartParam("file", "0123456789abcdef", "test.txt", "text/plain; charset=utf8")
            print('h', file=sys.stderr)
            datagen, headers = multipart_encode( [("var4", "val with spaces"), p] )
            print('i', file=sys.stderr)
            request = urllib.request('http://etd.local/upload', datagen, headers)
            print('j', file=sys.stderr)
            response = urllib.request.urlopen(request, None, 2).read()
            print('k', file=sys.stderr)
        except Exception as e:
            print('testRun: Exception: ', e, file=sys.stderr)
            return 1
        if response != 'test.txt:16\nvar4 = val with spaces':
            return 1
        return 0
    Thread(target=testRun).start()

@teardown('HTTP Upload')
def teardown_http_upload(e):
    return 0
