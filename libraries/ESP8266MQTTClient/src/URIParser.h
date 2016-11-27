#ifndef URIParser_h_
#define URIParser_h_
typedef struct{
	char *scheme;               /* mandatory */
	char *host;                 /* mandatory */
	char *port;                 /* optional */
	char *path;                 /* optional */
	char *query;                /* optional */
	char *fragment;             /* optional */
	char *username;             /* optional */
	char *password;             /* optional */
} parsed_url;

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Declaration of function prototypes
     */
    parsed_url *parse_uri(const char *);
    void parsed_url_free(parsed_url *);

#ifdef __cplusplus
}
#endif

#endif /* URIParser_h_ */