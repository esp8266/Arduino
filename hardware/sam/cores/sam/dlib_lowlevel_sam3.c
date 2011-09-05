/**
* \file
*
* Implementation of low level library.
*
*/

#if defined __ICCARM__ /* IAR Ewarm 5.41+ */

#include "board.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <yfuns.h>
#include <uart.h>

extern __weak size_t __write( int handle, const unsigned char *buf, size_t bufSize )
{
    size_t nChars = 0 ;

    /* Check for the command to flush all handles */
    if ( handle == -1 )
    {
        return 0 ;
    }

    /* Check for stdout and stderr (only necessary if FILE descriptors are enabled.) */
    if ( handle != 1 && handle != 2 )
    {
        /* remove warnings */
        return 0xfffffff ;
    }

    for ( /* Empty */ ; bufSize > 0 ; --bufSize )
    {
		while ( !uart_is_tx_ready(CONSOLE_UART) )
			;
		uart_write( CONSOLE_UART, *buf ) ;
        ++buf ;
        ++nChars ;
    }

    return nChars ;
}


extern __weak size_t __read( int handle, unsigned char *buf, size_t bufSize )
{
  size_t nChars = 0 ;

  /* Check for stdin (only necessary if FILE descriptors are enabled) */
  if ( handle != 0 )
  {
    /* remove warnings */
    return 0xfffffff ;
  }

  for ( /*Empty*/; bufSize > 0 ; --bufSize )
  {
    uint8_t c;
	while (uart_read( CONSOLE_UART, &c ))
	  ;

    if ( c == 0 )
    {
      break ;
    }
    *buf++ = c ;
    ++nChars ;
  }

  return nChars ;
}

/**
 * \brief Outputs a character on the UART.
 *
 * \param c  Character to output.
 *
 * \return The character that was output.
 */
extern __weak signed int putchar( signed int c )
{
  	while ( !uart_is_tx_ready(CONSOLE_UART) )
		;
    uart_write( CONSOLE_UART, c ) ;

    return c ;
}
#endif // defined __ICCARM__
