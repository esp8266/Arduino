#ifndef __CORE_ESP8266_NON32XFER_H
#define __CORE_ESP8266_NON32XFER_H

#ifdef __cplusplus
extern "C" {
#endif

extern void install_non32xfer_exception_handler();
extern uint32_t mmu_non32xfer_count;

#ifdef __cplusplus
}
#endif

#endif
