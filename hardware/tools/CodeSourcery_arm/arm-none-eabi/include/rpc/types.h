
/*
 * Copyright (c) 2009, Sun Microsystems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Sun Microsystems, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *        from: @(#)types.h 1.18 87/07/24 SMI
 *        from: @(#)types.h        2.3 88/08/15 4.0 RPCSRC
 *        $FreeBSD: src/include/rpc/types.h,v 1.10.6.1 2003/12/18 00:59:50 peter Exp $
 *        $NetBSD: types.h,v 1.13 2000/06/13 01:02:44 thorpej Exp $
 */

/*
 * Rpc additions to <sys/types.h>
 */
#ifndef _RPC_TYPES_H
#define _RPC_TYPES_H

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(___int64_t_defined)
typedef u_int64_t u_quad_t;
typedef int64_t   quad_t;
#endif /* ___int64_t_defined */
typedef int32_t   bool_t;
typedef int32_t   enum_t;

typedef u_int32_t rpcprog_t;
typedef u_int32_t rpcvers_t;
typedef u_int32_t rpcproc_t;
typedef u_int32_t rpcprot_t;
typedef u_int32_t rpcport_t;
typedef   int32_t rpc_inline_t;

#ifndef NULL
# define NULL 0
#endif
#define __dontcare__ -1

#ifndef FALSE
# define FALSE 0
#endif
#ifndef TRUE
# define TRUE 1
#endif

#ifndef mem_alloc
#define mem_alloc(bsize)        calloc(1, bsize)
#endif
#ifndef mem_free
#define mem_free(ptr, bsize)    free(ptr)
#endif

#ifdef __cplusplus
}
#endif

#endif /* !_RPC_TYPES_H */
