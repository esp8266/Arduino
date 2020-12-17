/*
 *  Replacement for the ROM aes_unwrap() function. It uses the heap instead of
 *  the static DRAM address at 0x3FFFEA80, which may step on the SYS stack in
 *  special circumstances such as HWDT Stack Dump.
 *
 *  When not using WPS, the address space 0x3FFFE000 up to 0x40000000 is mostly
 *  available for the stacks. The one known exception is the ROM AES APIs. When
 *  `aes_decrypt_init` is called, it uses memory at 0x3FFFEA80 up to 0x3FFFEB30
 *  for a buffer. At the finish, `aes_decrypt_deinit` zeros out the buffer.
 *
 *  The NONOS SDK appears to have replacements for most of the ROM's AES APIs.
 *  However, the SDK still calls on the ROM's aes_unwrap function, which uses
 *  the ROM's AES APIs to operate. These calls can overwrite some of the stack
 *  space. To resolve the problem, this module replaces `aes_unwrap`.
 *
 *  Final note, so far, I have not seen a problem when using the extra 4K heap
 *  option without the "debug HWDT". It is when combined with the HWDT Stack
 *  Dump that a problem shows. This combination adds a Boot ROM stack, which
 *  pushes up the SYS and CONT stacks into the AES Buffer space. Then the
 *  problem shows.
 *
 *  While debugging with painted stack space, during WiFi Connect, Reconnect,
 *  and about every hour, a block of memory 0x3FFFEA80 - 0x3FFFEB30 (176 bytes)
 *  was zeroed by the Boot ROM function aes_decrypt_init. All other painted
 *  memory in the area was untouched after starting WiFi.
 */

#if (defined(DEBUG_ESP_HWDT_NOEXTRA4K) && !defined(DEBUG_ESP_HWDT)) || KEEP_ROM_AES_UNWRAP
// Using the ROM version of aes_unwrap should be fine for the no extra 4K case
// which is usually used in conjunction with WPS.

#else
// This is required for DEBUG_ESP_HWDT.
// The need is unconfirmed for the extra 4K heap case.
#include "umm_malloc/umm_malloc.h"

extern "C" {

// Uses this function from the Boot ROM
void rijndaelKeySetupDec(u32 rk[], const u8 cipherKey[]);

// This replaces the Boot ROM version just for this module
// Uses a malloc-ed buffer instead of the static buffer in stack address space.
static void *aes_decrypt_init(const u8 *key, size_t len) {
  if (16u != len) {
    return 0;
  }
  u32 *rk = (u32 *)malloc(16*11);
  // u32 *rk = (u32 *)0x3FFFEA80u;    // This is what the ROM would have used.
  if (rk) {
    rijndaelKeySetupDec(rk, key);
  }
  return (void *)rk;
}

// This replaces the Boot ROM version just for this module
static void aes_decrypt_deinit(void *ctx) {
  if (ctx) {
    ets_memset(ctx, 0, 16*11);
    if ((uint32_t)ctx != 0x3FFFEA80ul) {
      free(ctx);
    }
  }
  return;
}

/*
 * The NONOS SDK has an override on this function. To replace the aes_unwrap
 * without changing its behavior too much. We need access to the ROM version of
 * the AES APIs to make our aes_unwrap functionally equal to the current
 * environment except for the AES Buffer.
 */
#ifndef ROM_aes_decrypt
#define ROM_aes_decrypt         0x400092d4
#endif

typedef void  (*fp_aes_decrypt_t)(void *ctx, const u8 *crypt, u8 *plain);
#define AES_DECRYPT (reinterpret_cast<fp_aes_decrypt_t>(ROM_aes_decrypt))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
 * This aes_unwrap() function overrides/replaces the Boot ROM version.
 *
 * It was adapted from aes_unwrap() found in the ESP8266 RTOS SDK
 *   .../components/wap_supplicant/src/crypto/aes-unwrap.c
 *
 */
///////////////////////////////////////////////////////////////////////////////
/*
 * AES key unwrap (128-bit KEK, RFC3394)
 *
 * Copyright (c) 2003-2007, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

/**  based on RTOS SDK
 * aes_unwrap - Unwrap key with AES Key Wrap Algorithm (128-bit KEK) (RFC3394)
 * @kek: Key encryption key (KEK)
 * @n: Length of the plaintext key in 64-bit units; e.g., 2 = 128-bit = 16
 * bytes
 * @cipher: Wrapped key to be unwrapped, (n + 1) * 64 bits
 * @plain: Plaintext key, n * 64 bits
 * Returns: 0 on success, -1 on failure (e.g., integrity verification failed)
 */
int aes_unwrap(const u8 *kek, int n, const u8 *cipher, u8 *plain)
{
	u8 a[8], *r, b[16];
	int i, j;
	void *ctx;

	/* 1) Initialize variables. */
	ets_memcpy(a, cipher, 8);
	r = plain;
	ets_memcpy(r, cipher + 8, 8 * n);

	ctx = aes_decrypt_init(kek, 16);
	if (ctx == NULL)
		return -1;

	/* 2) Compute intermediate values.
	 * For j = 5 to 0
	 *     For i = n to 1
	 *         B = AES-1(K, (A ^ t) | R[i]) where t = n*j+i
	 *         A = MSB(64, B)
	 *         R[i] = LSB(64, B)
	 */
	for (j = 5; j >= 0; j--) {
		r = plain + (n - 1) * 8;
		for (i = n; i >= 1; i--) {
			ets_memcpy(b, a, 8);
			b[7] ^= n * j + i;

			ets_memcpy(b + 8, r, 8);
			AES_DECRYPT(ctx, b, b);
			ets_memcpy(a, b, 8);
			ets_memcpy(r, b + 8, 8);
			r -= 8;
		}
	}
	aes_decrypt_deinit(ctx);

	/* 3) Output results.
	 *
	 * These are already in @plain due to the location of temporary
	 * variables. Just verify that the IV matches with the expected value.
	 */
	for (i = 0; i < 8; i++) {
		if (a[i] != 0xa6)
			return -1;
	}

	return 0;
}
};
#endif
