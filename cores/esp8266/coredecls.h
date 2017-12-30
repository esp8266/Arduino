
#ifndef __COREDECLS_H
#define __COREDECLS_H

#ifdef __cplusplus
extern "C" {
#endif

// TODO: put declarations here, get rid of -Wno-implicit-function-declaration

extern bool timeshift64_is_set;

void tune_timeshift64 (uint64_t now_us);
void settimeofday_cb (void (*cb)(void));

#ifdef __cplusplus
}
#endif

#endif // __COREDECLS_H
