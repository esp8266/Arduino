#ifndef __ESP8266_WEB_SERVER_HELPER_H__
#define __ESP8266_WEB_SERVER_HELPER_H__


namespace WebServerDarkain {

////////////////////////////////////////////////////////////////////////////////
// HTTP UPLOAD STATUS
// https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
////////////////////////////////////////////////////////////////////////////////
enum HTTPStatus {
	HTTP_CONTINUE				= 100,
	HTTP_SWITCH_PROTO			= 101,
	HTTP_OK						= 200,
	HTTP_CREATED				= 201,
	HTTP_ACCEPTED				= 202,
	HTTP_NON_AUTH_INFO			= 203,
	HTTP_NO_CONTENT				= 204,
	HTTP_RESET_CONTENT			= 205,
	HTTP_PARTIAL_CONTENT		= 206,
	HTTP_IM_USED				= 226,
	HTTP_MULTIPLE_CHOICE		= 300,
	HTTP_MOVED_PERM				= 301,
	HTTP_FOUND					= 302,
	HTTP_SEE_OTHER				= 303,
	HTTP_NOT_MODIFIED			= 304,
	HTTP_USE_PROXY				= 305,
	HTTP_SWITCH_PROXY			= 306,
	HTTP_TEMP_REDIRECT			= 307,
	HTTP_PERM_REDIRECT			= 308,
	HTTP_BAD_REQUEST			= 400,
	HTTP_UNAUTHORIZED			= 401,
	HTTP_PAYMENT_REQ			= 402,
	HTTP_FORBIDDEN				= 403,
	HTTP_NOT_FOUND				= 404,
	HTTP_NOT_ALLOWED			= 405,
	HTTP_NOT_ACCEPTABLE			= 406,
	HTTP_PROXY_AUTH_REQ			= 407,
	HTTP_TIMEOUT				= 408,
	HTTP_CONFLICT				= 409,
	HTTP_GONE					= 410,
	HTTP_LENGTH_REQ				= 411,
	HTTP_PRECON_FAIL			= 412,
	HTTP_PAYLOAD_LARGE			= 413,
	HTTP_URI_LARGE				= 414,
	HTTP_UNSUPPORT_MEDIA		= 415,
	HTTP_RANGE_ERROR			= 416,
	HTTP_EXPECATION_FAIL		= 417,
	HTTP_TEAPOT					= 418,
	HTTP_MISDIRECT_REQUEST		= 421,
	HTTP_TOO_MANY_REQUEST		= 429,
	HTTP_REQUEST_LARGE			= 431,
	HTTP_SERVER_ERROR			= 500,
	HTTP_NOT_IMPLEMENTED		= 501,
	HTTP_BAD_GATEWAY			= 502,
	HTTP_UNAVAILABLE			= 503,
	HTTP_GATEWAY_TIMEOUT		= 504,
	HTTP_VERSION_UNSUPPORTED	= 505,
};




////////////////////////////////////////////////////////////////////////////////
// HTTP REQUEST METHODS
// https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods
////////////////////////////////////////////////////////////////////////////////
enum HTTPMethod {
	HTTP_ANY,
	HTTP_GET,
	HTTP_HEAD,
	HTTP_POST,
	HTTP_PUT,
	HTTP_DELETE,
	HTTP_CONNECT,
	HTTP_OPTIONS,
	HTTP_TRACE,
	HTTP_PATCH,
};




////////////////////////////////////////////////////////////////////////////////
// HTTP UPLOAD STATUS
////////////////////////////////////////////////////////////////////////////////
enum HTTPUploadStatus {
	UPLOAD_FILE_START,
	UPLOAD_FILE_WRITE,
	UPLOAD_FILE_END,
	UPLOAD_FILE_ABORTED,
};




////////////////////////////////////////////////////////////////////////////////
// HTTP CLIENT STATUS
////////////////////////////////////////////////////////////////////////////////
enum HTTPClientStatus {
	HC_NONE,
	HC_WAIT_READ,
	HC_WAIT_CLOSE,
};




////////////////////////////////////////////////////////////////////////////////
// HTTP AUTHENTICATION
////////////////////////////////////////////////////////////////////////////////
enum HTTPAuthMethod {
	BASIC_AUTH,
	DIGEST_AUTH,
};




////////////////////////////////////////////////////////////////////////////////
// DOWNLOAD MTU
////////////////////////////////////////////////////////////////////////////////
#define HTTP_DOWNLOAD_UNIT_SIZE 1460



#ifndef HTTP_UPLOAD_BUFLEN
#define HTTP_UPLOAD_BUFLEN 2048
#endif



#define HTTP_MAX_DATA_WAIT	5000 //ms to wait for the client to send the request
#define HTTP_MAX_POST_WAIT	5000 //ms to wait for POST data to arrive
#define HTTP_MAX_SEND_WAIT	5000 //ms to wait for data chunk to be ACKed
#define HTTP_MAX_CLOSE_WAIT	2000 //ms to wait for the client to close the connection



#define CONTENT_LENGTH_UNKNOWN ((size_t) -1)
#define CONTENT_LENGTH_NOT_SET ((size_t) -2)



class ESP8266WebServer;



typedef struct {
	HTTPUploadStatus status;
	String	filename;
	String	name;
	String	type;
	size_t	totalSize;		// file size
	size_t	currentSize;	// size of data currently in buf
	uint8_t	buf[HTTP_UPLOAD_BUFLEN];
} HTTPUpload;



class RequestArgument {
public:
	RequestArgument()	{ key = value = nullptr; }
	~RequestArgument()	{ key = value = nullptr; }
	const char *key;
	const char *value;
};

//#include "detail/RequestHandler.h"

}; // namespace WebServerDarkain

namespace fs { class FS; }

#endif //__ESP8266_WEB_SERVER_HELPER_H__
