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
 *         JSON output generation
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 */

#ifndef __JSONTREE_H__
#define __JSONTREE_H__

#include "c_types.h"
#include "json/json.h"

#ifdef JSONTREE_CONF_MAX_DEPTH
#define JSONTREE_MAX_DEPTH JSONTREE_CONF_MAX_DEPTH
#else
#define JSONTREE_MAX_DEPTH 10
#endif /* JSONTREE_CONF_MAX_DEPTH */

struct jsontree_context {
    struct jsontree_value *values[JSONTREE_MAX_DEPTH];
    uint16_t index[JSONTREE_MAX_DEPTH];
    int (* putchar)(int);
    uint8_t depth;
    uint8_t path;
    int callback_state;
};

struct jsontree_value {
    uint8_t type;
    /* followed by a value */
};

struct jsontree_string {
    uint8_t type;
    const char *value;
};

struct jsontree_int {
    uint8_t type;
    int value;
};

/* NOTE: the jsontree_callback set will receive a jsonparse state */
struct jsonparse_state;
struct jsontree_callback {
    uint8_t type;
    int (* output)(struct jsontree_context *js_ctx);
    int (* set)(struct jsontree_context *js_ctx, struct jsonparse_state *parser);
};

struct jsontree_pair {
    const char *name;
    struct jsontree_value *value;
};

struct jsontree_object {
    uint8_t type;
    uint8_t count;
    struct jsontree_pair *pairs;
};

struct jsontree_array {
    uint8_t type;
    uint8_t count;
    struct jsontree_value **values;
};

#define JSONTREE_STRING(text) {JSON_TYPE_STRING, (text)}
#define JSONTREE_PAIR(name, value) {(name), (struct jsontree_value *)(value)}
#define JSONTREE_CALLBACK(output, set) {JSON_TYPE_CALLBACK, (output), (set)}

#define JSONTREE_OBJECT(name, ...)                                      \
    static struct jsontree_pair jsontree_pair_##name[] = {__VA_ARGS__};   \
    static struct jsontree_object name = {                                \
        JSON_TYPE_OBJECT,							\
        sizeof(jsontree_pair_##name)/sizeof(struct jsontree_pair),          \
        jsontree_pair_##name }

#define JSONTREE_PAIR_ARRAY(value) (struct jsontree_value *)(value)
#define JSONTREE_ARRAY(name, ...)                                      \
    static struct jsontree_value* jsontree_value_##name[] = {__VA_ARGS__};   \
    static struct jsontree_array name = {                                \
        JSON_TYPE_ARRAY,							\
        sizeof(jsontree_value_##name)/sizeof(struct jsontree_value*),          \
        jsontree_value_##name }

#define JSONTREE_OBJECT_EXT(name, ...)                                  \
    static struct jsontree_pair jsontree_pair_##name[] = {__VA_ARGS__};   \
    struct jsontree_object name = {                                       \
        JSON_TYPE_OBJECT,							\
        sizeof(jsontree_pair_##name)/sizeof(struct jsontree_pair),          \
        jsontree_pair_##name }

void jsontree_setup(struct jsontree_context *js_ctx,
                    struct jsontree_value *root, int (* putchar)(int));
void jsontree_reset(struct jsontree_context *js_ctx);

const char *jsontree_path_name(const struct jsontree_context *js_ctx,
                               int depth);

void jsontree_write_int(const struct jsontree_context *js_ctx, int value);
void jsontree_write_int_array(const struct jsontree_context *js_ctx, const int *text, uint32 length);

void jsontree_write_atom(const struct jsontree_context *js_ctx,
                         const char *text);
void jsontree_write_string(const struct jsontree_context *js_ctx,
                           const char *text);
int jsontree_print_next(struct jsontree_context *js_ctx);
struct jsontree_value *jsontree_find_next(struct jsontree_context *js_ctx,
        int type);

#endif /* __JSONTREE_H__ */
