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

/** \addtogroup pwm_module Working with PWM
 * The PWM driver provides the interface to configure and use the PWM
 * peripheral.
 *
 * The PWM macrocell controls square output waveforms of 4 channels.
 * Characteristics of output waveforms such as period, duty-cycle,
 * dead-time can be configured.\n
 * Some of PWM channels can be linked together as synchronous ul_channel and
 * duty-cycle of synchronous channels can be updated by PDC automaticly.
 *
 * Before enabling the channels, they must have been configured first.
 * The main settings include:
 * <ul>
 * <li>Configuration of the clock generator.</li>
 * <li>Selection of the clock for each ul_channel.</li>
 * <li>Configuration of output waveform characteristics, such as period, duty-cycle etc.</li>
 * <li>Configuration for synchronous channels if needed.</li>
 *    - Selection of the synchronous channels.
 *    - Selection of the moment when the WRDY flag and the corresponding PDC
 *      transfer request are set (PTRM and PTRCS in the PWM_SCM register).
 *    - Configuration of the update mode (UPDM in the PWM_SCM register).
 *    - Configuration of the update period (UPR in the PWM_SCUP register).
 * </ul>
 *
 * After the channels is enabled, the user must use respective update registers
 * to change the wave characteristics to prevent unexpected output waveform.
 * i.e. PWM_CDTYUPDx register should be used if user want to change duty-cycle
 * when the ul_channel is enabled.
 *
 * For more accurate information, please look at the PWM section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref pwmc.c\n
 * \ref pwmc.h.\n
 */
/*@{*/
/*@}*/

/**
 * \file
 *
 * Implementation of the Pulse Width Modulation Controller (PWM) peripheral.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

#include <stdint.h>
#include <assert.h>

/*----------------------------------------------------------------------------
 *         Local functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Finds a prescaler/divisor couple to generate the desired frequency
 * from MCK.
 *
 * Returns the value to enter in PWM_CLK or 0 if the configuration cannot be
 * met.
 *
 * \param frequency  Desired frequency in Hz.
 * \param mck  Master clock frequency in Hz.
 */
static uint16_t FindClockConfiguration(
    uint32_t frequency,
    uint32_t mck)
{
    uint32_t divisors[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    uint8_t divisor = 0;
    uint32_t prescaler;

    assert(frequency < mck);

    /* Find prescaler and divisor values */
    prescaler = (mck / divisors[divisor]) / frequency;
    while ((prescaler > 255) && (divisor < 11)) {

        divisor++;
        prescaler = (mck / divisors[divisor]) / frequency;
    }

    /* Return result */
    if ( divisor < 11 )
    {
//        TRACE_DEBUG( "Found divisor=%u and prescaler=%u for freq=%uHz\n\r", divisors[divisor], prescaler, frequency ) ;

        return prescaler | (divisor << 8) ;
    }
    else
    {
        return 0 ;
    }
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Configures PWM a ul_channel with the given parameters, basic configure function.
 *
 * The PWM controller must have been clocked in the PMC prior to calling this
 * function.
 * Beware: this function disables the ul_channel. It waits until disable is effective.
 *
 * \param ul_channel  Channel number.
 * \param prescaler  Channel prescaler.
 * \param alignment  Channel alignment.
 * \param polarity  Channel polarity.
 */
void PWMC_ConfigureChannel( Pwm* pPwm, uint32_t ul_channel, uint32_t prescaler, uint32_t alignment, uint32_t polarity )
{
    pPwm->PWM_CH_NUM[0].PWM_CMR = 1;

//    assert(prescaler < PWM_CMR0_CPRE_MCKB);
    assert((alignment & (uint32_t)~PWM_CMR_CALG) == 0);
    assert((polarity & (uint32_t)~PWM_CMR_CPOL) == 0);

    /* Disable ul_channel (effective at the end of the current period) */
    if ((pPwm->PWM_SR & (1 << ul_channel)) != 0) {
        pPwm->PWM_DIS = 1 << ul_channel;
        while ((pPwm->PWM_SR & (1 << ul_channel)) != 0);
    }

    /* Configure ul_channel */
    pPwm->PWM_CH_NUM[ul_channel].PWM_CMR = prescaler | alignment | polarity;
}

/**
 * \brief Configures PWM a ul_channel with the given parameters, extend configure function.
 *
 * The PWM controller must have been clocked in the PMC prior to calling this
 * function.
 * Beware: this function disables the ul_channel. It waits until disable is effective.
 *
 * \param ul_channel            Channel number.
 * \param prescaler          Channel prescaler.
 * \param alignment          Channel alignment.
 * \param polarity           Channel polarity.
 * \param countEventSelect   Channel counter event selection.
 * \param DTEnable           Channel dead time generator enable.
 * \param DTHInverte         Channel Dead-Time PWMHx output Inverted.
 * \param DTLInverte         Channel Dead-Time PWMHx output Inverted.
 */
void PWMC_ConfigureChannelExt( Pwm* pPwm, uint32_t ul_channel, uint32_t prescaler, uint32_t alignment, uint32_t polarity,
                               uint32_t countEventSelect, uint32_t DTEnable, uint32_t DTHInverte, uint32_t DTLInverte )
{
//    assert(prescaler < PWM_CMR0_CPRE_MCKB);
    assert((alignment & (uint32_t)~PWM_CMR_CALG) == 0);
    assert((polarity & (uint32_t)~PWM_CMR_CPOL) == 0);
    assert((countEventSelect & (uint32_t)~PWM_CMR_CES) == 0);
    assert((DTEnable & (uint32_t)~PWM_CMR_DTE) == 0);
    assert((DTHInverte & (uint32_t)~PWM_CMR_DTHI) == 0);
    assert((DTLInverte & (uint32_t)~PWM_CMR_DTLI) == 0);

    /* Disable ul_channel (effective at the end of the current period) */
    if ((pPwm->PWM_SR & (1 << ul_channel)) != 0) {
        pPwm->PWM_DIS = 1 << ul_channel;
        while ((pPwm->PWM_SR & (1 << ul_channel)) != 0);
    }

    /* Configure ul_channel */
    pPwm->PWM_CH_NUM[ul_channel].PWM_CMR = prescaler | alignment | polarity |
        countEventSelect | DTEnable | DTHInverte | DTLInverte;
}

/**
 * \brief Configures PWM clocks A & B to run at the given frequencies.
 *
 * This function finds the best MCK divisor and prescaler values automatically.
 *
 * \param clka  Desired clock A frequency (0 if not used).
 * \param clkb  Desired clock B frequency (0 if not used).
 * \param mck  Master clock frequency.
 */
void PWMC_ConfigureClocks(uint32_t clka, uint32_t clkb, uint32_t mck)
{
    uint32_t mode = 0;
    uint32_t result;

    /* Clock A */
    if (clka != 0) {

        result = FindClockConfiguration(clka, mck);
        assert( result != 0 ) ;
        mode |= result;
    }

    /* Clock B */
    if (clkb != 0) {

        result = FindClockConfiguration(clkb, mck);
        assert( result != 0 ) ;
        mode |= (result << 16);
    }

    /* Configure clocks */
//    TRACE_DEBUG( "Setting PWM_CLK = 0x%08X\n\r", mode ) ;
    PWM->PWM_CLK = mode;
}

/**
 * \brief Sets the period value used by a PWM ul_channel.
 *
 * This function writes directly to the CPRD register if the ul_channel is disabled;
 * otherwise, it uses the update register CPRDUPD.
 *
 * \param ul_channel Channel number.
 * \param period  Period value.
 */
void PWMC_SetPeriod( Pwm* pPwm, uint32_t ul_channel, uint16_t period)
{
    /* If ul_channel is disabled, write to CPRD */
    if ((pPwm->PWM_SR & (1 << ul_channel)) == 0) {

        pPwm->PWM_CH_NUM[ul_channel].PWM_CPRD = period;
    }
    /* Otherwise use update register */
    else {

        pPwm->PWM_CH_NUM[ul_channel].PWM_CPRDUPD = period;
    }
}

/**
 * \brief Sets the duty cycle used by a PWM ul_channel.
 * This function writes directly to the CDTY register if the ul_channel is disabled;
 * otherwise it uses the update register CDTYUPD.
 * Note that the duty cycle must always be inferior or equal to the ul_channel
 * period.
 *
 * \param ul_channel  Channel number.
 * \param duty     Duty cycle value.
 */
void PWMC_SetDutyCycle( Pwm* pPwm, uint32_t ul_channel, uint16_t duty)
{
    assert(duty <= pPwm->PWM_CH_NUM[ul_channel].PWM_CPRD);

    /* If ul_channel is disabled, write to CDTY */
    if ((pPwm->PWM_SR & (1 << ul_channel)) == 0) {

        pPwm->PWM_CH_NUM[ul_channel].PWM_CDTY = duty;
    }
    /* Otherwise use update register */
    else {

        pPwm->PWM_CH_NUM[ul_channel].PWM_CDTYUPD = duty;
    }
}

/**
 * \brief Sets the dead time used by a PWM ul_channel.
 * This function writes directly to the DT register if the ul_channel is disabled;
 * otherwise it uses the update register DTUPD.
 * Note that the dead time must always be inferior or equal to the ul_channel
 * period.
 *
 * \param ul_channel  Channel number.
 * \param timeH    Dead time value for PWMHx output.
 * \param timeL    Dead time value for PWMLx output.
 */
void PWMC_SetDeadTime( Pwm* pPwm, uint32_t ul_channel, uint16_t timeH, uint16_t timeL)
{
    assert(timeH <= pPwm->PWM_CH_NUM[ul_channel].PWM_CPRD);
    assert(timeL <= pPwm->PWM_CH_NUM[ul_channel].PWM_CPRD);

    /* If ul_channel is disabled, write to DT */
    if ((pPwm->PWM_SR & (1 << ul_channel)) == 0) {

        pPwm->PWM_CH_NUM[ul_channel].PWM_DT = timeH | (timeL << 16);
    }
    /* Otherwise use update register */
    else {
        pPwm->PWM_CH_NUM[ul_channel].PWM_DTUPD = timeH | (timeL << 16);
    }
}

/**
 * \brief Configures Syncronous ul_channel with the given parameters.
 * Beware: At this time, the channels should be disabled.
 *
 * \param channels                 Bitwise OR of Syncronous channels.
 * \param updateMode               Syncronous ul_channel update mode.
 * \param requestMode              PDC transfer request mode.
 * \param requestComparisonSelect  PDC transfer request comparison selection.
 */
void PWMC_ConfigureSyncChannel( Pwm* pPwm, uint32_t ul_channels, uint32_t updateMode, uint32_t requestMode, uint32_t requestComparisonSelect )
{
    pPwm->PWM_SCM = ul_channels | updateMode | requestMode | requestComparisonSelect;
}

/**
 * \brief Sets the update period of the synchronous channels.
 * This function writes directly to the SCUP register if the ul_channel #0 is disabled;
 * otherwise it uses the update register SCUPUPD.
 *
 * \param period   update period.
 */
void PWMC_SetSyncChannelUpdatePeriod( Pwm* pPwm, uint8_t period )
{
    /* If ul_channel is disabled, write to SCUP */
    if ((pPwm->PWM_SR & (1 << 0)) == 0) {

        pPwm->PWM_SCUP = period;
    }
    /* Otherwise use update register */
    else {

        pPwm->PWM_SCUPUPD = period;
    }
}

/**
 * \brief Sets synchronous channels update unlock.
 *
 * Note: If the UPDM field is set to 0, writing the UPDULOCK bit to 1
 * triggers the update of the period value, the duty-cycle and
 * the dead-time values of synchronous channels at the beginning
 * of the next PWM period. If the field UPDM is set to 1 or 2,
 * writing the UPDULOCK bit to 1 triggers only the update of
 * the period value and of the dead-time values of synchronous channels.
 * This bit is automatically reset when the update is done.
 */
void PWMC_SetSyncChannelUpdateUnlock( Pwm* pPwm )
{
    pPwm->PWM_SCUC = PWM_SCUC_UPDULOCK;
}

/**
 * \brief Enables the given PWM ul_channel.
 *
 * This does NOT enable the corresponding pin;this must be done in the user code.
 *
 * \param ul_channel  Channel number.
 */
void PWMC_EnableChannel( Pwm* pPwm, uint32_t ul_channel)
{
    pPwm->PWM_ENA = 1 << ul_channel;
}

/**
 * \brief Disables the given PWM ul_channel.
 *
 * Beware, ul_channel will be effectively disabled at the end of the current period.
 * Application can check ul_channel is disabled using the following wait loop:
 * while ((PWM->PWM_SR & (1 << ul_channel)) != 0);
 *
 * \param ul_channel  Channel number.
 */
void PWMC_DisableChannel( Pwm* pPwm, uint32_t ul_channel)
{
    pPwm->PWM_DIS = 1 << ul_channel;
}

/**
 * \brief Enables the period interrupt for the given PWM ul_channel.
 *
 * \param ul_channel  Channel number.
 */
void PWMC_EnableChannelIt( Pwm* pPwm, uint32_t ul_channel)
{
    pPwm->PWM_IER1 = 1 << ul_channel;
}

/**
 * \brief Disables the period interrupt for the given PWM ul_channel.
 *
 * \param ul_channel  Channel number.
 */
void PWMC_DisableChannelIt( Pwm* pPwm, uint32_t ul_channel)
{
    pPwm->PWM_IDR1 = 1 << ul_channel;
}

/**
 * \brief Enables the selected interrupts sources on a PWMC peripheral.
 *
 * \param sources1  Bitwise OR of selected interrupt sources of PWM_IER1.
 * \param sources2  Bitwise OR of selected interrupt sources of PWM_IER2.
 */
void PWMC_EnableIt( Pwm* pPwm, uint32_t sources1, uint32_t sources2)
{
    pPwm->PWM_IER1 = sources1;
    pPwm->PWM_IER2 = sources2;
}

/**
 * \brief Disables the selected interrupts sources on a PWMC peripheral.
 *
 * \param sources1  Bitwise OR of selected interrupt sources of PWM_IDR1.
 * \param sources2  Bitwise OR of selected interrupt sources of PWM_IDR2.
 */
void PWMC_DisableIt( Pwm* pPwm, uint32_t sources1, uint32_t sources2)
{
    pPwm->PWM_IDR1 = sources1;
    pPwm->PWM_IDR2 = sources2;
}

/**
 * \brief Sends the contents of buffer through a PWMC peripheral, using the PDC to
 * take care of the transfer.
 *
 * Note: Duty cycle of syncronous channels can update by PDC
 *       when the field UPDM (Update Mode) in the PWM_SCM register is set to 2.
 *
 * \param pwmc      Pointer to an Pwm instance.
 * \param pvBuffer  Data buffer to send.
 * \param length    Length of the data buffer.
 */
uint8_t PWMC_WriteBuffer( Pwm *pwmc, void* pvBuffer, uint32_t length)
{
    /* Check if first bank is free */
    if (pwmc->PWM_TCR == 0) {

        pwmc->PWM_TPR = (uint32_t) pvBuffer;
        pwmc->PWM_TCR = length;
        pwmc->PWM_PTCR = PERIPH_PTCR_TXTEN;
        return 1;
    }
    /* Check if second bank is free */
    else if (pwmc->PWM_TNCR == 0) {

        pwmc->PWM_TNPR = (uint32_t) pvBuffer;
        pwmc->PWM_TNCR = length;
        return 1;
    }

    /* No free banks */
    return 0;
}

/**
 * \brief Set PWM output override value.
 *
 * \param value  Bitwise OR of output override value.
 */
void PWMC_SetOverrideValue( Pwm* pPwm, uint32_t value)
{
    pPwm->PWM_OOV = value;
}

/**
 * \brief Enalbe override output.
 *
 * \param value  Bitwise OR of output selection.
 * \param sync   0: enable the output asyncronously, 1: enable it syncronously
 */
void PWMC_EnableOverrideOutput( Pwm* pPwm, uint32_t value, uint32_t sync)
{
    if (sync) {

        pPwm->PWM_OSSUPD = value;
    } else {

        pPwm->PWM_OSS = value;
    }
}

/**
 * \brief Disalbe override output.
 *
 * \param value  Bitwise OR of output selection.
 * \param sync   0: enable the output asyncronously, 1: enable it syncronously
 */
void PWMC_DisableOverrideOutput( Pwm* pPwm, uint32_t value, uint32_t sync)
{
    if (sync) {

        pPwm->PWM_OSCUPD = value;
    } else {

        pPwm->PWM_OSC = value;
    }
}

/**
 * \brief Set PWM fault mode.
 *
 * \param mode  Bitwise OR of fault mode.
 */
void PWMC_SetFaultMode( Pwm* pPwm, uint32_t mode)
{
    pPwm->PWM_FMR = mode;
}

/**
 * \brief PWM fault clear.
 *
 * \param fault  Bitwise OR of fault to clear.
 */
void PWMC_FaultClear( Pwm* pPwm, uint32_t fault)
{
    pPwm->PWM_FCR = fault;
}

/**
 * \brief Set PWM fault protection value.
 *
 * \param value  Bitwise OR of fault protection value.
 */
void PWMC_SetFaultProtectionValue( Pwm* pPwm, uint32_t value)
{
    pPwm->PWM_FPV = value;
}

/**
 * \brief Enable PWM fault protection.
 *
 * \param value  Bitwise OR of FPEx[y].
 */
void PWMC_EnableFaultProtection( Pwm* pPwm, uint32_t ul_channel, uint32_t ul_value)
{
	/* Fault Protection Enable Register */
	uint32_t ul_fault_enable_reg = 0;

#if (SAM3XA_SERIES)
	if (ul_channel < 4)
  {
		ul_channel *= 8;
		ul_fault_enable_reg = pPwm->PWM_FPE1;
		ul_fault_enable_reg &= ~(0xFF << ul_channel);
		ul_fault_enable_reg |= (ul_value << ul_channel);
		pPwm->PWM_FPE1 = ul_fault_enable_reg;
	}
  else
  {
		ul_channel -= 4;
		ul_channel *= 8;
		ul_fault_enable_reg = pPwm->PWM_FPE2;
		ul_fault_enable_reg &= ~(0xFF << ul_channel);
		ul_fault_enable_reg |= (ul_value << ul_channel);
		pPwm->PWM_FPE2 = ul_fault_enable_reg;
	}
#endif

#if (SAM3U_SERIES || SAM3S_SERIES || SAM3SD8_SERIES || SAM4S_SERIES)
	ul_channel *= 8;
	ul_fault_enable_reg = pPwm->PWM_FPE;
	ul_fault_enable_reg &= ~(0xFF << ul_channel);
	ul_fault_enable_reg |= (ul_value << ul_channel);
	pPwm->PWM_FPE = ul_fault_enable_reg;
#endif
}

/**
 * \brief Configure comparison unit.
 *
 * \param x     comparison x index
 * \param value comparison x value.
 * \param mode  comparison x mode
 */
void PWMC_ConfigureComparisonUnit( Pwm* pPwm, uint32_t x, uint32_t value, uint32_t mode)
{
    assert(x < 8);

    /* If ul_channel is disabled, write to CMPxM & CMPxV */
    if ((pPwm->PWM_SR & (1 << 0)) == 0) {
        pPwm->PWM_CMP[x].PWM_CMPM = mode;
        pPwm->PWM_CMP[x].PWM_CMPV = value;
    }
    /* Otherwise use update register */
    else {
        pPwm->PWM_CMP[x].PWM_CMPMUPD = mode;
        pPwm->PWM_CMP[x].PWM_CMPVUPD = value;
    }
}

/**
 * \brief Configure event line mode.
 *
 * \param x    Line x
 * \param mode Bitwise OR of line mode selection
 */
void PWMC_ConfigureEventLineMode( Pwm* pPwm, uint32_t x, uint32_t mode)
{
    assert(x < 2);

    if (x == 0)
    {
        pPwm->PWM_ELMR[0] = mode;
    }
    else
    {
        if (x == 1)
        {
            pPwm->PWM_ELMR[1] = mode;
        }
    }
}
