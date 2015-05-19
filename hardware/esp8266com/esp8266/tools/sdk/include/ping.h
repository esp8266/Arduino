#ifndef __PING_H__
#define __PING_H__


typedef void (* ping_recv_function)(void* arg, void *pdata);
typedef void (* ping_sent_function)(void* arg, void *pdata);

struct ping_option{
	uint32 count;
	uint32 ip;
	uint32 coarse_time;
	ping_recv_function recv_function;
	ping_sent_function sent_function;
	void* reverse;
};

struct ping_resp{
	uint32 total_count;
	uint32 resp_time;
	uint32 seqno;
	uint32 timeout_count;
	uint32 bytes;
	uint32 total_bytes;
	uint32 total_time;
	sint8  ping_err;
};

bool ping_start(struct ping_option *ping_opt);
bool ping_regist_recv(struct ping_option *ping_opt, ping_recv_function ping_recv);
bool ping_regist_sent(struct ping_option *ping_opt, ping_sent_function ping_sent);

#endif /* __PING_H__ */
