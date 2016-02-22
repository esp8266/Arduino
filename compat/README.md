If you are using [LWIP raw tcp mode](http://lwip.wikia.com/wiki/Raw/TCP) and want to add SSL support below are the steps that can help you to achieve this with the help of [axTLS]( http://axtls.sourceforge.net/ ).
		
First you have to include the `lwipr_compat.h` header.

```C
#include "compat/lwipr_compat.h"
```

Then in the code block where you initialize the tcp raw connection you should call `axl_init`.
Take a look at the example below:

```C
lwip_init();

/* 
 * The line below should be added AFTER the lwip_init code
 * AND BEFORE the call to tcp_new()
 * The parameter value 10 specifies how many SSL connections are expected
 */ 
axl_init(10);

// .. some more code
tcp = tcp_new();
tcp_sent(tcp, staticOnSent);
tcp_recv(tcp, staticOnReceive);
tcp_err(tcp, staticOnError);
tcp_poll(tcp, staticOnPoll, 4);
// ... and even more code 
res = tcp_connect(tcp, &addr, port, staticOnConnected);


```

Now we should add in our `staticOnConnected` funciton code to create new ssl context and ssl object. 
In the example below the `sslObj` and `sslContext` are defined as global

```C
// global definitions
SSL *sslObj = NULL;
SSLCTX* sslContext = NULL;

// and some more code...

err_t staticOnConnected(void *arg, struct tcp_pcb *tcp, err_t err)
{
	int clientfd = -1;
	uint32_t options = 0;

	if (tcp == NULL) {
		/* @TODO: Take care to handle error conditions */
		return -1;
	}

	clientfd = axl_append(tcp);
	if(clientfd == -1) {
		printf("Unable to add LWIP tcp -> clientfd mapping\n");
		return ERR_OK;
	}
	
	printf("Connected: ClientId: %d\n", clientfd);
#ifdef SSL_DEBUG
	options |= SSL_DISPLAY_STATES | SSL_DISPLAY_BYTES;
#endif	
	
	// if you want to verify the server certificate later you can also add the following option
	options |= SSL_SERVER_VERIFY_LATER
	
	sslContext = ssl_ctx_new(SSL_CONNECT_IN_PARTS | options, 1); // !!! SSL_CONNECT_IN_PARTS must be in the flags !!!
	sslObj = ssl_client_new(sslContext, clientfd, NULL, 0);

	return ERR_OK;
}
```


Once we are connected we can send and receive information. For the receiving part we can do the following

```C
err_t staticOnReceive(void *arg, struct tcp_pcb *tcp, struct pbuf *p, err_t err)
{
	uint8_t *read_buf = NULL;
	int read_bytes = 0;

	printf("Err: %d\n", err);

	if(tcp == NULL || p == NULL) {
		/* @TODO: Take care to handle error conditions */
		return -1;
	}

	read_bytes = axl_ssl_read(sslObj, &read_buf, tcp, p);
	if(read_bytes > 0) {
		printf("Got data: %s", read_buf);
		// @TODO: Do something useful with the read_buf 
	}

	return ERR_OK;
}
```

In the receiving part you can also add debug code to display more information about the SSL handshake, once it was successul.


```C
err_t staticOnReceive(void *arg, struct tcp_pcb *tcp, struct pbuf *p, err_t err)
{
	static int show_info = 0;
	const char *common_name = NULL;
	
	// ..
	read_bytes = axl_ssl_read(sslObj, &read_buf, tcp, p);
	if(read_bytes > 0) {
		printf("Got data: %s", read_buf);
		// @TODO: Do something useful with the read_buf 
	}
	
	if(!show_info && ssl_handshake_status(sslObj) == SSL_OK) {
		common_name = ssl_get_cert_dn(sslObj, SSL_X509_CERT_COMMON_NAME);
		if (common_name) {
			printf("Common Name:\t\t\t%s\n", common_name);
		}

		// These two funcitons below can be found in the axtls examples
		display_session_id(sslObj); 
		display_cipher(sslObj);
		show_info = 1;
	}
	
	return ERR_OK;
}

```


And for the sending part we can use the following code sample as a start

```C
void someSendingfunction() {
	uint8_t *out_buf;
	int out_bytes = 0;
	
	// ... take care to store something in the out_buf
	
	axl_ssl_write(sslObj, out_buf, out_bytes);
}

```

Good luck and send your success stories at slaff@attachix.com.
