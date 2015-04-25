/*
 * copyright (c) 2008 - 2011 Espressif System
 *
 * Define user specified Event signals and Task priorities here
 *
 */

#ifndef _ETS_SYS_H
#define _ETS_SYS_H

#include "c_types.h"
#include "eagle_soc.h"

typedef uint32_t ETSSignal;
typedef uint32_t ETSParam;

typedef struct ETSEventTag ETSEvent;

struct ETSEventTag {
    ETSSignal sig;
    ETSParam  par;
};

typedef void (*ETSTask)(ETSEvent *e);

/* timer related */
typedef uint32_t ETSHandle;
typedef void ETSTimerFunc(void *timer_arg);

typedef struct _ETSTIMER_ {
    struct _ETSTIMER_    *timer_next;
    uint32_t              timer_expire;
    uint32_t              timer_period;
    ETSTimerFunc         *timer_func;
    void                 *timer_arg;
} ETSTimer;

/* interrupt related */

typedef void (*int_handler_t)(void*);

#define ETS_SPI_INUM	   2
#define ETS_GPIO_INUM       4
#define ETS_UART_INUM       5
#define ETS_UART1_INUM      5
#define ETS_FRC_TIMER1_INUM 9  /* use edge*/

#define ETS_INTR_LOCK() \
    ets_intr_lock()

#define ETS_INTR_UNLOCK() \
    ets_intr_unlock()

#define ETS_FRC_TIMER1_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_FRC_TIMER1_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_GPIO_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_GPIO_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_UART_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_UART_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_SPI_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_SPI_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_INTR_ENABLE(inum) \
    ets_isr_unmask((1<<inum))

#define ETS_INTR_DISABLE(inum) \
    ets_isr_mask((1<<inum))

#define ETS_SPI_INTR_ENABLE() \
    ETS_INTR_ENABLE(ETS_SPI_INUM)

#define ETS_UART_INTR_ENABLE() \
    ETS_INTR_ENABLE(ETS_UART_INUM)

#define ETS_UART_INTR_DISABLE() \
    ETS_INTR_DISABLE(ETS_UART_INUM)

#define ETS_FRC1_INTR_ENABLE() \
	ETS_INTR_ENABLE(ETS_FRC_TIMER1_INUM)

#define ETS_FRC1_INTR_DISABLE() \
	ETS_INTR_DISABLE(ETS_FRC_TIMER1_INUM)

#define ETS_GPIO_INTR_ENABLE() \
    ETS_INTR_ENABLE(ETS_GPIO_INUM)

#define ETS_GPIO_INTR_DISABLE() \
    ETS_INTR_DISABLE(ETS_GPIO_INUM)


void *pvPortMalloc(size_t xWantedSize);
void *pvPortRealloc(void* ptr, size_t xWantedSize);
void pvPortFree(void *ptr);
void *vPortMalloc(size_t xWantedSize);
void vPortFree(void *ptr);
void *ets_memcpy(void *dest, const void *src, size_t n);
void *ets_memset(void *s, int c, size_t n);
void ets_timer_arm_new(ETSTimer *a, int b, int c, int isMstimer);
void ets_timer_setfn(ETSTimer *t, ETSTimerFunc *fn, void *parg);
void ets_timer_disarm(ETSTimer *a);
int atoi(const char *nptr);
int ets_strncmp(const char *s1, const char *s2, int len);
int ets_strcmp(const char *s1, const char *s2);
int ets_strlen(const char *s);
char *ets_strcpy(char *dest, const char *src);
char *ets_strncpy(char *dest, const char *src, size_t n);
char *ets_strstr(const char *haystack, const char *needle);
int ets_sprintf(char *str, const char *format, ...)  __attribute__ ((format (printf, 2, 3)));
int os_snprintf(char *str, size_t size, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
int ets_printf(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));
void ets_install_putc1(void* routine);
void uart_div_modify(int no, int freq);
void ets_isr_mask(int intr);
void ets_isr_unmask(int intr);
void ets_isr_attach(int intr, int_handler_t handler, void *arg);
void ets_intr_lock();
void ets_intr_unlock();
int ets_vsnprintf(char * s, size_t n, const char * format, va_list arg);
int ets_vprintf(const char * format, va_list arg);

#endif /* _ETS_SYS_H */
