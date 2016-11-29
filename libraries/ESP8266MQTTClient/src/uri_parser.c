/*
URI Parser
Copyright (c) 2016 Tuan PM (tuanpm@live.com)
License (MIT license):
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#include "uri_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static __inline__ int
_is_scheme_char(int c)
{
	return (!isalpha(c) && '+' != c && '-' != c && '.' != c) ? 0 : 1;
}

#define JUMP_NEXT_STATE(var, state) { *curr_ptr = 0; curr_ptr ++; var = curr_ptr; parse_state = state; break;}
parsed_uri_t *parse_uri(const char *url)
{
	parsed_uri_t *puri;
	char *curr_ptr;
	int bracket_flag;
	enum parse_state_t {
		PARSE_SCHEME = 0,
		PARSE_USERNAME_OR_HOST,
		PARSE_PASSWORD_OR_PORT,
		PARSE_HOST,
		PARSE_PORT,
		PARSE_PATH,
		PARSE_QUERY,
		PARSE_FRAGMENT
	} parse_state = 0;
	puri = (parsed_uri_t *)malloc(sizeof(parsed_uri_t));
	if ( NULL == puri ) {
		return NULL;
	}
	puri->_uri_len = strlen(url);
	puri->_uri = (char*) malloc(puri->_uri_len + 1);
	if(puri->_uri == NULL) {
		free_parsed_uri(puri);
		return NULL;
	}
	strcpy(puri->_uri, url);
	puri->_uri[puri->_uri_len] = 0;
	puri->scheme = NULL;
	puri->host = NULL;
	puri->port = NULL;
	puri->path = NULL;
	puri->query = NULL;
	puri->fragment = NULL;
	puri->username = NULL;
	puri->password = NULL;

	curr_ptr = puri->_uri;
	puri->scheme = curr_ptr;
	parse_state = PARSE_SCHEME;
	bracket_flag = 0;
	while(*curr_ptr) {
		*curr_ptr = tolower(*curr_ptr);
		switch(parse_state) {
			case PARSE_SCHEME: /* parse scheme */
				if(curr_ptr + 3 < (puri->_uri + puri->_uri_len) && memcmp(curr_ptr, "://", 3) == 0) {
					*curr_ptr++ = 0;
					*curr_ptr++ = 0;
					*curr_ptr++ = 0;
					puri->host = curr_ptr;
					puri->username = curr_ptr;
					parse_state = PARSE_USERNAME_OR_HOST; //next is username or host
					break;
				}
				if(!_is_scheme_char(*curr_ptr)) {
					free_parsed_uri(puri);
					return NULL;
				}
				curr_ptr ++;
				break;
			case PARSE_USERNAME_OR_HOST: /* username or host*/
				if ( '[' == *curr_ptr && bracket_flag == 0) {
					bracket_flag = 1;
				} else if(']' == *curr_ptr && bracket_flag == 1) {
					bracket_flag = 0;
				}
				if(bracket_flag == 0 && *curr_ptr == ':') {
					puri->port = curr_ptr;
					JUMP_NEXT_STATE(puri->password, PARSE_PASSWORD_OR_PORT);
				}
				curr_ptr ++;
				break;
			case PARSE_PASSWORD_OR_PORT: /* password or port */
				if(*curr_ptr == '@') {  
					puri->port = NULL;
					JUMP_NEXT_STATE(puri->host, PARSE_HOST);
					break;
				} else if(*curr_ptr == '/') {
					puri->username = NULL;
					puri->password = NULL;
					JUMP_NEXT_STATE(puri->path, PARSE_PATH);
					break;
				}
				curr_ptr ++;
				break;
			case PARSE_HOST: /* host */
				if ( '[' == *curr_ptr && bracket_flag == 0) {
					bracket_flag = 1;
				} else if(']' == *curr_ptr && bracket_flag == 1) {
					bracket_flag = 0;
				}
				if(bracket_flag == 0 && *curr_ptr == ':') {
					JUMP_NEXT_STATE(puri->port, PARSE_PORT);
				} else if(bracket_flag == 0 && *curr_ptr == '/') {
					puri->port = NULL;
					JUMP_NEXT_STATE(puri->path, PARSE_PATH);
				}
				curr_ptr ++;
				break;
			case PARSE_PORT: /* port */
				if(*curr_ptr == '/') {
					JUMP_NEXT_STATE(puri->path, PARSE_PATH);
				} else if(*curr_ptr == '?') {
					JUMP_NEXT_STATE(puri->query, PARSE_QUERY);
				} else if(*curr_ptr == '#') {
					JUMP_NEXT_STATE(puri->fragment, PARSE_FRAGMENT);
				}
				curr_ptr ++;
				break;
			case PARSE_PATH: /* path */
				if(*curr_ptr == '?') {
					JUMP_NEXT_STATE(puri->query, PARSE_QUERY);
				} else if(*curr_ptr == '#') {
					JUMP_NEXT_STATE(puri->fragment, PARSE_FRAGMENT);
				}
				curr_ptr ++;
			case PARSE_QUERY: /* query */
				if(*curr_ptr == '#') {
					JUMP_NEXT_STATE(puri->fragment, PARSE_FRAGMENT);
				}
			case PARSE_FRAGMENT: /* fragment*/
				curr_ptr ++;
				break;
		}

	}
	if(parse_state < PARSE_HOST) {
		puri->host = puri->username;
		puri->port = puri->password;
		puri->username = NULL;
		puri->password = NULL;
	}

	return puri;
}
void free_parsed_uri(parsed_uri_t *puri)
{
	if ( NULL != puri ) {
		if ( NULL != puri->_uri ) {
			free(puri->_uri);
		}
		free(puri);
	}
}
