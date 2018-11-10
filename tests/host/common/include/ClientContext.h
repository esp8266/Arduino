/*
 */
#ifndef CLIENTCONTEXT_H
#define CLIENTCONTEXT_H

class ClientContext;
class WiFiClient;

extern "C" void esp_yield();
extern "C" void esp_schedule();

#include <include/DataSource.h>

bool getDefaultPrivateGlobalSyncValue ();

typedef void (*discard_cb_t)(void*, ClientContext*);

class ClientContext
{
public:
    ClientContext(tcp_pcb* pcb, discard_cb_t discard_cb, void* discard_cb_arg) :
        _discard_cb(discard_cb), _discard_cb_arg(discard_cb_arg), _refcnt(0), _next(0),
        _sync(::getDefaultPrivateGlobalSyncValue())
    {
        (void)pcb;
        _sock = -1;
    }

    err_t abort()
    {
        if (_sock >= 0)
            ::close(_sock);
        return ERR_ABRT;
    }

    err_t close()
    {
        abort();
        return ERR_OK;
    }

    ~ClientContext()
    {
        abort();
    }

    ClientContext* next() const
    {
        return _next;
    }

    ClientContext* next(ClientContext* new_next)
    {
        _next = new_next;
        return _next;
    }

    void ref()
    {
        ++_refcnt;
        DEBUGV(":ref %d\r\n", _refcnt);
    }

    void unref()
    {
        DEBUGV(":ur %d\r\n", _refcnt);
        if(--_refcnt == 0) {
            discard_received();
            close();
            if(_discard_cb) {
                _discard_cb(_discard_cb_arg, this);
            }
            DEBUGV(":del\r\n");
            delete this;
        }
    }

    int connect(ip_addr_t* addr, uint16_t port)
    {
        return mockConnect(addr->addr, _sock, port);
    }

    size_t availableForWrite()
    {
        // XXXFIXME be smarter
        return 512;
    }

    void setNoDelay(bool nodelay)
    {
        fprintf(stderr, MOCK "TODO setNoDelay(%d)\n", (int)nodelay);
    }

    bool getNoDelay() const
    {
        fprintf(stderr, MOCK "TODO getNoDelay()\n");
        return false;
    }

    void setTimeout(int timeout_ms)
    {
        _timeout_ms = timeout_ms;
    }

    int getTimeout() const
    {
        return _timeout_ms;
    }

    uint32_t getRemoteAddress() const
    {
        fprintf(stderr, MOCK "TODO getRemoteAddress()\n");
        return 0;
    }

    uint16_t getRemotePort() const
    {
        fprintf(stderr, MOCK "TODO getRemotePort()\n");
        return 0;
    }

    uint32_t getLocalAddress() const
    {
        fprintf(stderr, MOCK "TODO getLocalAddress()\n");
        return 0;
    }

    uint16_t getLocalPort() const
    {
        fprintf(stderr, MOCK "TODO getLocalPort()\n");
        return 0;
    }

    size_t getSize() const
    {
        return mockReadReady(_sock)? 16: 0; //XXXFIXMEDIRTYBUFFERIZE!
    }

    char read()
    {
        char c;
        if (read(&c, 1))
            return c;
        return 0;
    }

    size_t read (char* dst, size_t size)
    {
        return mockRead(_sock, dst, size, _timeout_ms);
    }

    char peek() const
    {
        // mock implementation will be more complex because of this
        fprintf(stderr, MOCK "ClientContext::peek !\n");
        ::abort();
        return -1;
    }

    size_t peekBytes(char *dst, size_t size) const
    {
        // mock implementation will be more complex because of this
        fprintf(stderr, MOCK "ClientContext::peek !\n");
        ::abort();
        return 0;
    }

    void discard_received()
    {
    }

    bool wait_until_sent(int max_wait_ms = WIFICLIENT_MAX_FLUSH_WAIT_MS)
    {
        return true;
    }

    uint8_t state() const
    {
        return 0;
    }

    size_t write(const uint8_t* data, size_t size)
    {
        return mockWrite(_sock, data, size, _timeout_ms);
    }

    size_t write(Stream& stream)
    {
        fprintf(stderr, MOCK "TODO ClientContext::write(Stream)\n");
        return 0;
    }

    size_t write_P(PGM_P buf, size_t size)
    {
        return write((const uint8_t*)buf, size);
    }

    void keepAlive (uint16_t idle_sec = TCP_DEFAULT_KEEPALIVE_IDLE_SEC, uint16_t intv_sec = TCP_DEFAULT_KEEPALIVE_INTERVAL_SEC, uint8_t count = TCP_DEFAULT_KEEPALIVE_COUNT)
    {
        fprintf(stderr, MOCK "TODO ClientContext::keepAlive()\n");
    }

    bool isKeepAliveEnabled () const
    {
        fprintf(stderr, MOCK "TODO ClientContext::isKeepAliveEnabled()\n");
        return false;
    }

    uint16_t getKeepAliveIdle () const
    {
        fprintf(stderr, MOCK "TODO ClientContext::getKeepAliveIdle()\n");
        return 0;
    }

    uint16_t getKeepAliveInterval () const
    {
        fprintf(stderr, MOCK "TODO ClientContext::getKeepAliveInternal()\n");
        return 0;
    }

    uint8_t getKeepAliveCount () const
    {
        fprintf(stderr, MOCK "TODO ClientContext::getKeepAliveCount()\n");
        return 0;
    }

    bool getSync () const
    {
        fprintf(stderr, MOCK "TODO ClientContext::getSync()\n");
        return _sync;
    }

    void setSync (bool sync)
    {
        fprintf(stderr, MOCK "TODO ClientContext::setSync()\n");
        _sync = sync;
    }

private:

    discard_cb_t _discard_cb;
    void* _discard_cb_arg;

    DataSource* _datasource = nullptr;

    int8_t _refcnt;
    ClientContext* _next;
    
    bool _sync;
    
    // MOCK
    
    int _sock = -1;
    int _timeout_ms = 5000;
};

#endif//CLIENTCONTEXT_H
