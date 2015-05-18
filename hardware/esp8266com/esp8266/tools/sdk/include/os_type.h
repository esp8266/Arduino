/*
 * copyright (c) Espressif System 2010
 *
 * mapping to ETS structures
 *
 */
#ifndef _OS_TYPES_H_
#define _OS_TYPES_H_

#include "ets_sys.h"

#define os_signal_t ETSSignal
#define os_param_t  ETSParam
#define os_event_t ETSEvent
#define os_task_t ETSTask
#define os_timer_t  ETSTimer
#define os_timer_func_t ETSTimerFunc

#endif
