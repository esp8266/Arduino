/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011-2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef UTILS_INTERRUPT_INTERRUPT_H
#define UTILS_INTERRUPT_INTERRUPT_H

#include "../chip.h"

/**
 * \weakgroup interrupt_group
 *
 * @{
 */

/**
 * \name Interrupt Service Routine definition
 *
 * @{
 */

/**
 * \brief Define service routine
 *
 * \note For NVIC devices the interrupt service routines are predefined to
 *       add to vector table in binary generation, so there is no service
 *       register at run time. The routine collections are in exceptions.h.
 *
 * Usage:
 * \code
 * ISR(foo_irq_handler)
 * {
 *      // Function definition
 *      ...
 * }
 * \endcode
 *
 * \param func Name for the function.
 */
#  define ISR(func)    \
	void func (void)

/**
 * \brief Initialize interrupt vectors
 *
 * For NVIC the interrupt vectors are put in vector table. So nothing
 * to do to initialize them, except defined the vector function with
 * right name.
 *
 * This must be called prior to \ref irq_register_handler.
 */
#  define irq_initialize_vectors()                     \
	do {                                           \
	} while(0)

/**
 * \brief Register handler for interrupt
 *
 * For NVIC the interrupt vectors are put in vector table. So nothing
 * to do to register them, except defined the vector function with
 * right name.
 *
 * Usage:
 * \code
 * irq_initialize_vectors();
 * irq_register_handler(foo_irq_handler);
 * \endcode
 *
 * \note The function \a func must be defined with the \ref ISR macro.
 * \note The functions prototypes can be found in the device exception header
 *       files (exceptions.h).
 */
#  define irq_register_handler(...)                    \
	do {                                           \
	} while(0)

//@}

#  define cpu_irq_enable()                             \
	do {                                           \
		g_interrupt_enabled = 1;            \
		__DMB();                               \
		__enable_irq();                        \
	} while (0)
#  define cpu_irq_disable()                            \
	do {                                           \
		__disable_irq();                       \
		__DMB();                               \
		g_interrupt_enabled = 0;           \
	} while (0)

typedef uint32_t irqflags_t;
extern int g_interrupt_enabled;

static inline irqflags_t cpu_irq_save(void)
{
	irqflags_t flags = g_interrupt_enabled;
	cpu_irq_disable();
	return flags;
}

static inline int cpu_irq_is_enabled_flags(irqflags_t flags)
{
	return (flags);
}

static inline void cpu_irq_restore(irqflags_t flags)
{
	if (cpu_irq_is_enabled_flags(flags))
		cpu_irq_enable();
}

#define cpu_irq_is_enabled()    g_interrupt_enabled

/**
 * \weakgroup interrupt_deprecated_group
 * @{
 */

#define Enable_global_interrupt()            cpu_irq_enable()
#define Disable_global_interrupt()           cpu_irq_disable()
#define Is_global_interrupt_enabled()        cpu_irq_is_enabled()

//@}

//@}

#endif /* UTILS_INTERRUPT_INTERRUPT_H */
