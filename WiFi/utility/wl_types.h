/*
 * wl_types.h
 *
 *  Created on: Jul 30, 2010
 *      Author: dlafauci
 */


#ifndef	_WL_TYPES_H_
#define	_WL_TYPES_H_

#include <inttypes.h>

typedef enum {
        WL_FAILURE = -1,
        WL_SUCCESS = 1,
} wl_error_code_t;

/* Authentication modes */
enum wl_auth_mode {
        AUTH_MODE_INVALID,
        AUTH_MODE_AUTO,
        AUTH_MODE_OPEN_SYSTEM,
        AUTH_MODE_SHARED_KEY,
        AUTH_MODE_WPA,
        AUTH_MODE_WPA2,
        AUTH_MODE_WPA_PSK,
        AUTH_MODE_WPA2_PSK
};

#endif //_WL_TYPES_H_
