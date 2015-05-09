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

#ifndef __JSONPARSE_H__
#define __JSONPARSE_H__

#include "c_types.h"
#include "json/json.h"

#ifdef JSONPARSE_CONF_MAX_DEPTH
#define JSONPARSE_MAX_DEPTH JSONPARSE_CONF_MAX_DEPTH
#else
#define JSONPARSE_MAX_DEPTH 10
#endif

struct jsonparse_state {
    const char *json;
    int pos;
    int len;
    int depth;
    /* for handling atomic values */
    int vstart;
    int vlen;
    char vtype;
    char error;
    char stack[JSONPARSE_MAX_DEPTH];
};

/**
 * \brief      Initialize a JSON parser state.
 * \param state A pointer to a JSON parser state
 * \param json The string to parse as JSON
 * \param len  The length of the string to parse
 *
 *             This function initializes a JSON parser state for
 *             parsing a string as JSON.
 */
void jsonparse_setup(struct jsonparse_state *state, const char *json,
                     int len);

/* move to next JSON element */
int jsonparse_next(struct jsonparse_state *state);

/* copy the current JSON value into the specified buffer */
int jsonparse_copy_value(struct jsonparse_state *state, char *buf,
                         int buf_size);

/* get the current JSON value parsed as an int */
int jsonparse_get_value_as_int(struct jsonparse_state *state);

/* get the current JSON value parsed as a long */
long jsonparse_get_value_as_long(struct jsonparse_state *state);

/* get the current JSON value parsed as a unsigned long */
unsigned long jsonparse_get_value_as_ulong(struct jsonparse_state *state);

/* get the length of the current JSON value */
int jsonparse_get_len(struct jsonparse_state *state);

/* get the type of the current JSON value */
int jsonparse_get_type(struct jsonparse_state *state);

/* compare the JSON value with the specified string */
int jsonparse_strcmp_value(struct jsonparse_state *state, const char *str);

#endif /* __JSONPARSE_H__ */
