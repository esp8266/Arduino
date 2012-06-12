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

/** \file */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "chip.h"

/**
 * \brief Configures Pio pin internal pull-up.
 *
 * \param pPio            Pointer to a PIO controller.
 * \param dwMask          Bitmask of one or more pin(s) to configure.
 * \param dwPullUpEnable  Indicates if the pin(s) internal pull-up shall be configured.
 */
extern void PIO_DisableInterrupt( Pio *pPio, const uint32_t dwMask )
{
    /* Disable interrupts on the pin */
    pPio->PIO_IDR = dwMask ;
}

/**
 * \brief Configures Pio pin internal pull-up.
 *
 * \param pPio            Pointer to a PIO controller.
 * \param dwMask          Bitmask of one or more pin(s) to configure.
 * \param dwPullUpEnable  Indicates if the pin(s) internal pull-up shall be configured.
 */
extern void PIO_PullUp( Pio *pPio, const uint32_t dwMask, const uint32_t dwPullUpEnable )
{
    /* Enable the pull-up(s) if necessary */
    if ( dwPullUpEnable )
    {
        pPio->PIO_PUER = dwMask ;
    }
    else
    {
        pPio->PIO_PUDR = dwMask ;
    }
}

/**
 * \brief Configures Glitch or Debouncing filter for input.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 * \param cuttoff  Cutt off frequency for debounce filter.
 */
extern void PIO_SetDebounceFilter( Pio* pPio, const uint32_t dwMask, const uint32_t dwCuttOff )
{
#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_)
    pPio->PIO_IFSCER = dwMask ; /* set Debouncing, 0 bit field no effect */
#elif (defined _SAM3XA_) || (defined _SAM3U_)
    pPio->PIO_DIFSR = dwMask ; /* set Debouncing, 0 bit field no effect */
#else
    #error "The specified chip is not supported."
#endif
    pPio->PIO_SCDR = ((32678/(2*(dwCuttOff))) - 1) & 0x3FFF; /* the lowest 14 bits work */
}

/**
 * \brief Sets a high output level on all the PIOs defined in the given Pin instance.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will memorize the value they are changed to outputs.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 */
extern void PIO_Set( Pio* pPio, const uint32_t dwMask )
{
    pPio->PIO_SODR = dwMask ;
}

/**
 * \brief Returns 1 if one or more PIO of the given Pin instance currently have
 * a high level; otherwise returns 0. This method returns the actual value that
 * is being read on the pin. To return the supposed output value of a pin, use
 * PIO_GetOutputDataStatus() instead.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 *
 * \return 1 if the Pin instance contains at least one PIO that currently has
 * a high level; otherwise 0.
 */
extern uint32_t PIO_Get( Pio* pPio, const EPioType dwType, const uint32_t dwMask )
{
    uint32_t dwReg ;

    if ( (dwType == PIO_OUTPUT_0) || (dwType == PIO_OUTPUT_1) )
    {
        dwReg = pPio->PIO_ODSR ;
    }
    else
    {
        dwReg = pPio->PIO_PDSR ;
    }

    if ( (dwReg & dwMask) == 0 )
    {
        return 0 ;
    }
    else
    {
        return 1 ;
    }
}

/**
 * \brief Sets a low output level on all the PIOs defined in the given Pin instance.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will memorize the value they are changed to outputs.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 */
extern void PIO_Clear( Pio* pPio, const uint32_t dwMask )
{
    pPio->PIO_CODR = dwMask ;
}

/**
 * \brief Configures one pin of a PIO controller as being controlled by specific peripheral.
 *
 * \param pPio    Pointer to a PIO controller.
 * \param dwType  PIO type.
 * \param dwMask  Bitmask of one or more pin(s) to configure.
 */
extern void PIO_SetPeripheral( Pio* pPio, EPioType dwType, uint32_t dwMask )
{
    uint32_t dwSR ;

    /* Disable interrupts on the pin(s) */
    pPio->PIO_IDR = dwMask ;

    switch ( dwType )
    {
        case PIO_PERIPH_A :
#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_)
            dwSR = pPio->PIO_ABCDSR[0] ;
            pPio->PIO_ABCDSR[0] &= (~dwMask & dwSR) ;

            dwSR = pPio->PIO_ABCDSR[1];
            pPio->PIO_ABCDSR[1] &= (~dwMask & dwSR) ;
#endif /* (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_) */

#if (defined _SAM3U_) || (defined _SAM3XA_)
            dwSR = pPio->PIO_ABSR ;
            pPio->PIO_ABSR &= (~dwMask & dwSR) ;
#endif /* (defined _SAM3U_) || (defined _SAM3XA_) */
        break ;

        case PIO_PERIPH_B :
#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_)
            dwSR = pPio->PIO_ABCDSR[0] ;
            pPio->PIO_ABCDSR[0] = (dwMask | dwSR) ;

            dwSR = pPio->PIO_ABCDSR[1] ;
            pPio->PIO_ABCDSR[1] &= (~dwMask & dwSR) ;
#endif /* (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_) */

#if (defined _SAM3U_) || (defined _SAM3XA_)
            dwSR = pPio->PIO_ABSR ;
            pPio->PIO_ABSR = (dwMask | dwSR) ;
#endif /* (defined _SAM3U_) || (defined _SAM3XA_) */
        break ;

#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_)
        case PIO_PERIPH_C :
            dwSR = pPio->PIO_ABCDSR[0] ;
            pPio->PIO_ABCDSR[0] &= (~dwMask & dwSR) ;

            dwSR = pPio->PIO_ABCDSR[1] ;
            pPio->PIO_ABCDSR[1] = (dwMask | dwSR) ;
        break ;

        case PIO_PERIPH_D :
            dwSR = pPio->PIO_ABCDSR[0] ;
            pPio->PIO_ABCDSR[0] = (dwMask | dwSR) ;

            dwSR = pPio->PIO_ABCDSR[1] ;
            pPio->PIO_ABCDSR[1] = (dwMask | dwSR) ;
        break ;
#endif /* (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_) */

        // other types are invalid in this function
        case PIO_INPUT :
        case PIO_OUTPUT_0 :
        case PIO_OUTPUT_1 :
        case PIO_NOT_A_PIN :
        return ;
    }

    // Remove the pins from under the control of PIO
    pPio->PIO_PDR = dwMask ;
}

/**
 * \brief Configures one or more pin(s) or a PIO controller as inputs. Optionally,
 * the corresponding internal pull-up(s) and glitch filter(s) can be enabled.
 *
 * \param pPio        Pointer to a PIO controller.
 * \param dwMask      Bitmask indicating which pin(s) to configure as input(s).
 * \param dwAttribute .
 */
extern void PIO_SetInput( Pio* pPio, uint32_t dwMask, uint32_t dwAttribute )
{
    PIO_DisableInterrupt( pPio, dwMask ) ;
    PIO_PullUp( pPio, dwMask, dwAttribute & PIO_PULLUP ) ;

    /* Enable Input Filter if necessary */
    if ( dwAttribute & (PIO_DEGLITCH | PIO_DEBOUNCE) )
    {
        pPio->PIO_IFER = dwMask ;
    }
    else
    {
        pPio->PIO_IFDR = dwMask ;
    }

    /* Enable de-glitch or de-bounce if necessary */
#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_)
    if ( dwAttribute & PIO_DEGLITCH )
    {
        pPio->PIO_IFSCDR = dwMask ;
    }
    else
    {
        if ( dwAttribute & PIO_DEBOUNCE )
        {
            pPio->PIO_IFSCER = dwMask ;
        }
    }
#elif (defined _SAM3U_) || (defined _SAM3XA_)
    if ( dwAttribute & PIO_DEGLITCH )
    {
        pPio->PIO_SCIFSR = dwMask ;
    }
    else
    {
        if ( dwAttribute & PIO_DEBOUNCE )
        {
            pPio->PIO_SCIFSR = dwMask ;
        }
    }
#else
    #error "The specified chip is not supported."
#endif

    /* Configure pin as input */
    pPio->PIO_ODR = dwMask ;
    pPio->PIO_PER = dwMask ;
}

/**
 * \brief Configures one or more pin(s) of a PIO controller as outputs, with the
 * given default value. Optionally, the multi-drive feature can be enabled
 * on the pin(s).
 *
 * \param pPio              Pointer to a PIO controller.
 * \param dwMask            Bitmask indicating which pin(s) to configure.
 * \param defaultValue      Default level on the pin(s).
 * \param enableMultiDrive  Indicates if the pin(s) shall be configured as open-drain.
 * \param enablePullUp      Indicates if the pin shall have its pull-up activated.
 */
extern void PIO_SetOutput( Pio* pPio, uint32_t dwMask, uint32_t dwDefaultValue,
                                      uint32_t dwMultiDriveEnable, uint32_t dwPullUpEnable )
{
    PIO_DisableInterrupt( pPio, dwMask ) ;
    PIO_PullUp( pPio, dwMask, dwPullUpEnable ) ;

    /* Enable multi-drive if necessary */
    if ( dwMultiDriveEnable )
    {
        pPio->PIO_MDER = dwMask ;
    }
    else
    {
        pPio->PIO_MDDR = dwMask ;
    }

    /* Set default value */
    if ( dwDefaultValue )
    {
        pPio->PIO_SODR = dwMask ;
    }
    else
    {
        pPio->PIO_CODR = dwMask ;
    }

    /* Configure pin(s) as output(s) */
    pPio->PIO_OER = dwMask ;
    pPio->PIO_PER = dwMask ;
}

/**
 *
 * \return 1 if the pins have been configured properly; otherwise 0.
 */
extern uint32_t PIO_Configure( Pio* pPio, const EPioType dwType, const uint32_t dwMask, const uint32_t dwAttribute )
{
    /* Configure pins */
    switch ( dwType )
    {
        case PIO_PERIPH_A :
        case PIO_PERIPH_B :
#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_)
        case PIO_PERIPH_C :
        case PIO_PERIPH_D :
#endif /* (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_) */
            /* Put the pin under control of peripheral */
            PIO_SetPeripheral( pPio, dwType, dwMask ) ;
            /* Disable interrupts on the pin(s) */
            PIO_DisableInterrupt( pPio, dwMask ) ;
            /* Enable Pullup */
            PIO_PullUp( pPio, dwMask, (dwAttribute & PIO_PULLUP) ) ;
        break;

        case PIO_INPUT :
            PIO_SetInput( pPio, dwMask, dwAttribute ) ;
        break;

        case PIO_OUTPUT_0 :
        case PIO_OUTPUT_1 :
            PIO_SetOutput( pPio, dwMask, (dwType == PIO_OUTPUT_1),
                          (dwAttribute & PIO_OPENDRAIN) ? 1 : 0,
                          (dwAttribute & PIO_PULLUP) ? 1 : 0);
        break ;

        default :
        return 0 ;
    }

    return 1 ;
}

/**
 * \brief Returns 1 if one or more PIO of the given Pin are configured to output a
 * high level (even if they are not output).
 * To get the actual value of the pin, use PIO_Get() instead.
 *
 * \param pPio  Pointer to a Pin instance describing one or more pins.
 *
 * \return 1 if the Pin instance contains at least one PIO that is configured
 * to output a high level; otherwise 0.
 */
extern uint32_t PIO_GetOutputDataStatus( const Pio* pPio, const uint32_t dwMask )
{
  /* Test if pin is under control of PIO */
  if ( (pPio->PIO_PSR & dwMask) != 0 )
  {
    /* Test if pin is configured as output */
    if ( (pPio->PIO_OSR & dwMask) != 0 )
    {
      return 1 ;
    }
  }

  return 0 ;
}

