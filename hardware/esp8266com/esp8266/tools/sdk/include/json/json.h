/*
 * Copyright (c) 2011-2012, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *         A few JSON defines used for parsing and generating JSON.
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 */

#ifndef __JSON_H__
#define __JSON_H__

#define JSON_TYPE_ARRAY '['
#define JSON_TYPE_OBJECT '{'
#define JSON_TYPE_PAIR ':'
#define JSON_TYPE_PAIR_NAME 'N' /* for N:V pairs */
#define JSON_TYPE_STRING '"'
#define JSON_TYPE_INT 'I'
#define JSON_TYPE_NUMBER '0'
#define JSON_TYPE_ERROR 0

/* how should we handle null vs false - both can be 0? */
#define JSON_TYPE_NULL 'n'
#define JSON_TYPE_TRUE 't'
#define JSON_TYPE_FALSE 'f'

#define JSON_TYPE_CALLBACK 'C'

enum {
    JSON_ERROR_OK,
    JSON_ERROR_SYNTAX,
    JSON_ERROR_UNEXPECTED_ARRAY,
    JSON_ERROR_UNEXPECTED_END_OF_ARRAY,
    JSON_ERROR_UNEXPECTED_OBJECT,
    JSON_ERROR_UNEXPECTED_STRING
};

#define JSON_CONTENT_TYPE "application/json"

#endif /* __JSON_H__ */
