/*
 * Compatibility for AxTLS with LWIP raw tcp mode (http://lwip.wikia.com/wiki/Raw/TCP)
 *
 *  Created on: Jan 15, 2016
 *      Author: Slavey Karadzhov
 */
#include "lwipr_compat.h"

AxlTcpDataArray axlFdArray;

#include <stdlib.h>

/* High Level "public" functions */

/**
 * Function that should be called once we are ready to use the axTLS - LWIP raw compatibility
 */
void axl_init(int capacity) {
	ax_fd_init(&axlFdArray, capacity);
}

/**
 * Appends a tcp to the internal array. Returns client file descriptor
 */
int axl_append(struct tcp_pcb *tcp) {
	return ax_fd_append(&axlFdArray, tcp);
}

/**
 * Frees  the internal mapping from this tcp. Returns the number of occurrences of the tcp
 */
int axl_free(struct tcp_pcb *tcp) {
	int i;
	int occurances = 0;

	if(tcp == NULL) {
		return 0;
	}

	AxlTcpDataArray *vector = &axlFdArray;
	AXL_DEBUG("AXL: Freeing %d tcp item", vector->size);
	for (i = 0; i < vector->size; i++) {
		if (vector->data[i].tcp == tcp) {
			if(vector->data[i].tcp_pbuf != NULL) {
				pbuf_free(vector->data[i].tcp_pbuf);
				vector->data[i].tcp_pbuf = NULL;
			}
			vector->data[i].tcp = NULL;
			vector->data[i].pbuf_offset = 0;
			occurances++;
		}
	}

	return occurances;
}

/**
 * Reads data from the SSL over TCP stream. Returns decrypted data.
 * @param SSL *sslObj
 * @param uint8_t **in_data - pointer to the decrypted incoming data, or NULL if nothing was read
 * @param void *arg - possible arguments passed to the tcp raw layer during initialization
 * @param tcp_pcb *tcp - pointer to the raw tcp object
 * @param pbuf *p - pointer to the buffer with the TCP packet data
 *
 * @return int
 * 			0 - when everything is fine but there are no symbols to process yet
 * 			< 0 - when there is an error
 * 			> 0 - the length of the clear text characters that were read
 */
int axl_ssl_read(SSL *ssl, uint8_t **in_data, struct tcp_pcb *tcp, struct pbuf *p) {
	int read_bytes = 0;
	int total_bytes = 0;
	int clientfd = -1;

	AxlTcpData* data = NULL;

	if (ssl == NULL) {
		return ERR_AXL_INVALID_SSL;
	}

	clientfd = ax_fd_getfd(&axlFdArray, tcp);
	if(clientfd == -1) {
		return ERR_AXL_INVALID_CLIENTFD;
	}

	data = ax_fd_get(&axlFdArray, clientfd);
	if(data == NULL) {
		return ERR_AXL_INVALID_CLIENTFD_DATA;
	}

	if (p != NULL) {
		data->tcp_pbuf = p;
		data->pbuf_offset = 0;
	}

	AXL_DEBUG("READY TO READ SOME DATA\n");

	tcp_recved(tcp, p->tot_len);
	do {
		WATCHDOG_FEED();
		read_bytes = ssl_read(ssl, in_data);
		AXL_DEBUG("axl_ssl_read: Read bytes: %d\n", read_bytes);
		if(read_bytes < SSL_OK) {
			/* An error has occurred. Give it back for further processing */
			total_bytes = read_bytes;
			break;
		}
		total_bytes+= read_bytes;
	} while (p->tot_len - data->pbuf_offset > 0);

	pbuf_free(p);

	return total_bytes;
}

/*
 * Lower Level LWIP RAW functions
 */

/*
 * The LWIP tcp raw version of the SOCKET_WRITE(A, B, C)
 */
int ax_port_write(int clientfd, uint8_t *buf, uint16_t bytes_needed) {
	AxlTcpData *data = NULL;
	int tcp_len = 0;
	err_t err = ERR_OK;

	data = ax_fd_get(&axlFdArray, clientfd);
	if(data == NULL) {
		return ERR_AXL_INVALID_CLIENTFD;
	}

	if (data == NULL || data->tcp == NULL || buf == NULL || bytes_needed == 0) {
		AXL_DEBUG("Return Zero.\n");
		return 0;
	}

	if (tcp_sndbuf(data->tcp) < bytes_needed) {
		tcp_len = tcp_sndbuf(data->tcp);
		if(tcp_len == 0) {
			AXL_DEBUG("The send buffer is full! We have problem.\n");
			return 0;
		}

	} else {
		tcp_len = bytes_needed;
	}

	if (tcp_len > 2 * data->tcp->mss) {
		tcp_len = 2 * data->tcp->mss;
	}

	do {
		err = tcp_write(data->tcp, buf, tcp_len, TCP_WRITE_FLAG_COPY);
		if(err < SSL_OK) {
			AXL_DEBUG("Got error: %d\n", err);
		}

		if (err == ERR_MEM) {
			AXL_DEBUG("Not enough memory to write data with length: %d (%d)\n", tcp_len, bytes_needed);
			tcp_len /= 2;
		}
	} while (err == ERR_MEM && tcp_len > 1);
	AXL_DEBUG("send_raw_packet length %d(%d)\n", tcp_len, bytes_needed);
	if (err == ERR_OK) {
		err = tcp_output(data->tcp);
		if(err != ERR_OK) {
			AXL_DEBUG("tcp_output got err: %d\n", err);
		}
	}

	return tcp_len;
}

/*
 * The LWIP tcp raw version of the SOCKET_READ(A, B, C)
 */
int ax_port_read(int clientfd, uint8_t *buf, int bytes_needed) {
	AxlTcpData *data = NULL;
	uint8_t *read_buf = NULL;
	uint8_t *pread_buf = NULL;
	u16_t recv_len = 0;

	data = ax_fd_get(&axlFdArray, clientfd);
	if (data == NULL) {
		return ERR_AXL_INVALID_CLIENTFD_DATA;
	}

	if(data->tcp_pbuf == NULL || data->tcp_pbuf->tot_len == 0) {
		AXL_DEBUG("Nothing to read?! May be the connection needs resetting?\n");
		return 0;
	}

	read_buf =(uint8_t*)calloc(data->tcp_pbuf->len + 1, sizeof(uint8_t));
	pread_buf = read_buf;
	if (pread_buf != NULL){
		recv_len = pbuf_copy_partial(data->tcp_pbuf, read_buf, bytes_needed, data->pbuf_offset);
		data->pbuf_offset += recv_len;
	}

	if (recv_len != 0) {
		memcpy(buf, read_buf, recv_len);
	}

	if(bytes_needed < recv_len) {
		AXL_DEBUG("Bytes needed: %d, Bytes read: %d\n", bytes_needed, recv_len);
	}

	free(pread_buf);
	pread_buf = NULL;

	return recv_len;
}

int ax_get_file(const char *filename, uint8_t **buf) {
    *buf = 0;
    return 0;
}

void ax_wdt_feed() {
}

/*
 * Utility functions
 */
void ax_fd_init(AxlTcpDataArray *vector, int capacity) {
	vector->size = 0;
	vector->capacity = capacity;
	vector->data = (AxlTcpData*) malloc(sizeof(AxlTcpData) * vector->capacity);
}

int ax_fd_append(AxlTcpDataArray *vector, struct tcp_pcb *tcp) {
	int index;

	ax_fd_double_capacity_if_full(vector);
	index = vector->size++;
	vector->data[index].tcp = tcp;
	vector->data[index].tcp_pbuf = NULL;
	vector->data[index].pbuf_offset = 0;

	return index;
}

AxlTcpData* ax_fd_get(AxlTcpDataArray *vector, int index) {
	if (index >= vector->size || index < 0) {
		AXL_DEBUG("Index %d out of bounds for vector of size %d\n", index,
				vector->size);
		return NULL;
	}
	return &(vector->data[index]);
}

int ax_fd_getfd(AxlTcpDataArray *vector, struct tcp_pcb *tcp) {
	int i;
	for (i = 0; i < vector->size; i++) {
		if (vector->data[i].tcp == tcp) {
			return i;
		}
	}

	return -1;
}

void ax_fd_set(AxlTcpDataArray *vector, int index, struct tcp_pcb *tcp) {
	AxlTcpData value;
	while (index >= vector->size) {
		ax_fd_append(vector, 0);
	}

	value.tcp = tcp;
	value.tcp_pbuf = NULL;
	value.pbuf_offset = 0;
	vector->data[index] = value;
}

void ax_fd_double_capacity_if_full(AxlTcpDataArray *vector) {
	if (vector->size >= vector->capacity) {
		vector->capacity *= 2;
		vector->data = (AxlTcpData*)realloc(vector->data, sizeof(AxlTcpData) * vector->capacity);
	}
}

void ax_fd_free(AxlTcpDataArray *vector) {
	free(vector->data);
}
