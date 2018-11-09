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
    }

    err_t abort()
    {
        return ERR_ABRT;
    }

    err_t close()
    {
        err_t err = ERR_OK;
//                err = ERR_ABRT;
        return err;
    }

    ~ClientContext()
    {
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
        return 0;
    }

    size_t availableForWrite()
    {
        return 0;
    }

    void setNoDelay(bool nodelay)
    {
    }

    bool getNoDelay() const
    {
        return false;
    }

    void setTimeout(int timeout_ms)
    {
    }

    int getTimeout() const
    {
        return 0;
    }

    uint32_t getRemoteAddress() const
    {
        return 0;
    }

    uint16_t getRemotePort() const
    {
        return 0;
    }

    uint32_t getLocalAddress() const
    {
        return 0;
    }

    uint16_t getLocalPort() const
    {
        return 0;
    }

    size_t getSize() const
    {
        return 0;
    }

    char read()
    {
        return -1;
    }

    size_t read(char* dst, size_t size)
    {
        return 0;
    }

    char peek() const
    {
        return -1;
    }

    size_t peekBytes(char *dst, size_t size) const
    {
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
        return 0;
    }

    size_t write(Stream& stream)
    {
        return 0;
    }

    size_t write_P(PGM_P buf, size_t size)
    {
        return 0;
    }

    void keepAlive (uint16_t idle_sec = TCP_DEFAULT_KEEPALIVE_IDLE_SEC, uint16_t intv_sec = TCP_DEFAULT_KEEPALIVE_INTERVAL_SEC, uint8_t count = TCP_DEFAULT_KEEPALIVE_COUNT)
    {
    }

    bool isKeepAliveEnabled () const
    {
        return false;
    }

    uint16_t getKeepAliveIdle () const
    {
        return 0;
    }

    uint16_t getKeepAliveInterval () const
    {
        return 0;
    }

    uint8_t getKeepAliveCount () const
    {
        return 0;
    }

    bool getSync () const
    {
        return _sync;
    }

    void setSync (bool sync)
    {
        _sync = sync;
    }

private:

    discard_cb_t _discard_cb;
    void* _discard_cb_arg;

    DataSource* _datasource = nullptr;

    int8_t _refcnt;
    ClientContext* _next;
    
    bool _sync;
};

#endif//CLIENTCONTEXT_H
