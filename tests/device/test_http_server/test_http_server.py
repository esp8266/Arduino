from mock_decorators import setup, teardown
from threading import Thread
from poster.encode import MultipartParam
from poster.encode import multipart_encode
from poster.streaminghttp import register_openers
import urllib2
import urllib

def http_test(res, url, get=None, post=None):
    response = ''
    try:
        if get:
            url += '?' + urllib.urlencode(get)
        if post:
            post = urllib.urlencode(post)
        request = urllib2.urlopen(url, post, 2)
        response = request.read()
    except:
        return 1
    if response != res:
        return 1
    return 0

@setup('HTTP GET Parameters')
def setup_http_get_params(e):
    def testRun():
        return http_test('var1=val with spaces&var+=some%', 'http://etd.local/get', {'var1' : 'val with spaces', 'var+' : 'some%'})
    Thread(target=testRun).start()

@teardown('HTTP GET Parameters')
def teardown_http_get_params(e):
    return 0

@setup('HTTP POST Parameters')
def setup_http_post_params(e):
    def testRun():
        return http_test('var2=val with spaces', 'http://etd.local/post', None, {'var2' : 'val with spaces'})
    Thread(target=testRun).start()

@teardown('HTTP POST Parameters')
def teardown_http_post_params(e):
    return 0

@setup('HTTP GET+POST Parameters')
def setup_http_getpost_params(e):
    def testRun():
        return http_test('var3=val with spaces&var+=some%', 'http://etd.local/get_and_post', {'var3' : 'val with spaces'}, {'var+' : 'some%'})
    Thread(target=testRun).start()

@teardown('HTTP GET+POST Parameters')
def teardown_http_getpost_params(e):
    return 0

@setup('HTTP Upload')
def setup_http_upload(e):
    def testRun():
        response = ''
        try:
            register_openers()
            p = MultipartParam("file", "0123456789abcdef", "test.txt", "text/plain; charset=utf8")
            datagen, headers = multipart_encode( [("var4", "val with spaces"), p] )
            request = urllib2.Request('http://etd.local/upload', datagen, headers)
            response = urllib2.urlopen(request, None, 2).read()
        except:
            return 1
        if response != 'test.txt:16&var4=val with spaces':
            return 1
        return 0
    Thread(target=testRun).start()

@teardown('HTTP Upload')
def teardown_http_upload(e):
    return 0
