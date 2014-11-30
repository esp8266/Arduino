#ifndef CLIENTCONTEXT_H
#define CLIENTCONTEXT_H

class ClientContext;
class WiFiClient;

typedef void(*discard_cb_t)(void*, ClientContext*);

extern "C" void esp_yield();
extern "C" void esp_schedule();


class ClientContext
{
public:
    ClientContext(tcp_pcb* pcb, size_t rx_buffer_size, 
    			  discard_cb_t discard_cb, void* discard_cb_arg)
    : _pcb(pcb)
    , _rx_buf(rx_buffer_size)
    , _discard_cb(discard_cb)
    , _discard_cb_arg(discard_cb_arg)
    , _refcnt(0)
    , _next(0)
    {
        tcp_setprio(pcb, TCP_PRIO_MIN);
        tcp_arg(pcb, this);
        tcp_recv(pcb, &_s_recv);
        tcp_sent(pcb, &_s_sent);
        tcp_err(pcb, &_s_error);
        // tcp_poll(pcb, &_s_poll, 0);
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
    }

    void ref()
    {
        ++_refcnt;
    }

    void unref()
    {
        DEBUGV("WC:ur %d\r\n", _refcnt);
        if (--_refcnt == 0 && _pcb)
        {
        	// if (_discard_cb)
	        // 	(*_discard_cb)(_discard_cb_arg, this);
	        // else
         //    {
                tcp_arg(_pcb, NULL);
                tcp_sent(_pcb, NULL);
                tcp_recv(_pcb, NULL);
                tcp_err(_pcb, NULL);
                tcp_close(_pcb);
                _pcb = 0;
                delete this;
	        	//tcp_abort(_pcb);
            // }
        }
    }

    size_t getSize() const
    {
    	return _rx_buf.getSize();
    }

    char read()
    {
    	return _rx_buf.read();
    }

    size_t read(char* dst, size_t size)
    {
        DEBUGV("WC:rd\r\n");
    	return _rx_buf.read(dst, size);
    }

    char peek()
    {
    	return _rx_buf.peek();
    }

    void flush()
    {
    	return _rx_buf.flush();
    }

    uint8_t state() const
    {
    	if (!_pcb)
    		return CLOSED;

    	return _pcb->state;
    }

    size_t write(const char* data, size_t size)
    {
    	if (!_pcb)
    		return 0;

    	size_t room = tcp_sndbuf(_pcb);
    	size_t will_send = (room < size) ? room : size;
    	err_t err = tcp_write(_pcb, data, will_send, 0);
    	if (err != ERR_OK)
    		return 0;

    	_size_sent = will_send;
        DEBUGV("WC:wr\r\n");
    	delay(5000); // max send timeout
        DEBUGV("WC:ww\r\n");
    	return will_send - _size_sent;
    }


private:

	err_t _recv(tcp_pcb* pcb, pbuf* pb, err_t err)
	{

        if (pb == 0) // connection closed
        {
            DEBUGV("WC:rcl\r\n");
            tcp_arg(pcb, NULL);
            tcp_sent(pcb, NULL);
            tcp_recv(pcb, NULL);
            tcp_err(pcb, NULL);
            tcp_close(pcb);
            _pcb = 0;
            return ERR_OK;
        }

        DEBUGV("WC:rcr\r\n");
        size_t len = pb->len;
        _rx_buf.write(reinterpret_cast<const char*>(pb->payload), pb->len);
        pbuf_free(pb);

        tcp_recved(pcb, len);
        return ERR_OK;
	}

	void _error(err_t err)
	{
        DEBUGV("WC:er\r\n");
		_pcb = 0;
		if (_size_sent)
			esp_schedule();

	}

	err_t _poll(tcp_pcb* pcb)
	{
        return ERR_OK;
	}

	err_t _sent(tcp_pcb* pcb, uint16_t len)
	{
        DEBUGV("WC:sent %d\r\n", len);
		_size_sent -= len;
		if (_size_sent == 0)
			esp_schedule();
        return ERR_OK;
	}


	static err_t _s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err)
	{
	    return reinterpret_cast<ClientContext*>(arg)->_recv(tpcb, pb, err);
	}

	static void _s_error(void *arg, err_t err)
	{
	    reinterpret_cast<ClientContext*>(arg)->_error(err);
	}

	static err_t _s_poll(void *arg, struct tcp_pcb *tpcb)
	{
	    return reinterpret_cast<ClientContext*>(arg)->_poll(tpcb);
	}

	static err_t _s_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len)
	{
	    return reinterpret_cast<ClientContext*>(arg)->_sent(tpcb, len);
	}

private:
    ClientContext* _next;
    int _refcnt;
    tcp_pcb* _pcb;
    cbuf  _rx_buf;
    discard_cb_t _discard_cb;
    void* _discard_cb_arg;
    size_t _size_sent;
};



#endif//CLIENTCONTEXT_H
