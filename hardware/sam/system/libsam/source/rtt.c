/*
 %atmel_license%
*/

/** \addtogroup rtt_module Working with RTT
 * The RTT driver provides the interface to configure and use the RTT
 * peripheral.
 *
 * The Real-time Timer is used to count elapsed seconds.\n
 * This timer is clocked by the 32kHz system clock divided by a programmable
 * 16-bit balue. To be accurate, it is better to use an
 * external 32kHz crystal instead of the internal 32kHz RC.\n
 *
 * To count elapsed seconds, the user could follow these few steps:
 * <ul>
 * <li>Programming PTPRES in RTT_MR to feeding the timer with a 1Hz signal.</li>
 * <li>Writing the bit RTTRST in RTT_MR to restart the timer with new settings.</li>
 * </ul>
 *
 * An alarm can be set to happen on second by setting alarm value in RTT_AR.
 * Alarm occurence can be detected by polling or interrupt.
 *
 * For more accurate information, please look at the RTT section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref rtt.c\n
 * \ref rtt.h.\n
 */
/*@{*/
/*@}*/

/**
 * \file
 *
 * Implementation of Real Time Timer (RTT) controller.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "chip.h"

#include <assert.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Changes the prescaler value of the given RTT and restarts it.
 *
 * \note This function disables RTT interrupt sources.
 *
 * \param rtt  Pointer to a Rtt instance.
 * \param prescaler  Prescaler value for the RTT.
 */
void RTT_SetPrescaler(Rtt *rtt, uint16_t prescaler)
{
    rtt->RTT_MR = (prescaler |  RTT_MR_RTTRST);
}

/**
 * \brief Returns the current value of the RTT timer value.
 *
 * \param rtt  Pointer to a Rtt instance.
 */
uint32_t RTT_GetTime(Rtt *rtt)
{
    return rtt->RTT_VR;
}

/**
 * \brief Enables the specified RTT interrupt sources.
 *
 * \param rtt  Pointer to a Rtt instance.
 * \param sources  Bitmask of interrupts to enable.
 */
void RTT_EnableIT(Rtt *rtt, uint32_t sources)
{
    assert( (sources & 0x0004FFFF) == 0 ) ;
    rtt->RTT_MR |= sources;
}

/**
 * \brief Returns the status register value of the given RTT.
 *
 * \param rtt  Pointer to an Rtt instance.
 */
uint32_t RTT_GetStatus(Rtt *rtt)
{
    return rtt->RTT_SR;
}

/**
 * \brief Configures the RTT to generate an alarm at the given time.
 *
 * \param pRtt  Pointer to an Rtt instance.
 * \param time  Alarm time.
 */
void RTT_SetAlarm(Rtt *pRtt, uint32_t time)
{
    assert(time > 0);

    pRtt->RTT_AR = time - 1;
}
