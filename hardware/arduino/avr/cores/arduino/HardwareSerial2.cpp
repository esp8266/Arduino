#include "Arduino.h"
#include "HardwareSerial.h"

// Each HardwareSerial is defined in its own file, sine the linker pulls
// in the entire file when any element inside is used. --gc-sections can
// additionally cause unused symbols to be dropped, but ISRs have the
// "used" attribute so are never dropped and they keep the
// HardwareSerial instance in as well. Putting each instance in its own
// file prevents the linker from pulling in any unused instances in the
// first place.

#if defined(HAVE_HWSERIAL2)

#if defined(USART_RX_vect)
  ISR(USART_RX_vect)
#elif defined(USART2_RX_vect)
  ISR(USART2_RX_vect)
#elif defined(USART_RXC_vect)
  ISR(USART_RXC_vect) // ATmega8
#else
  #error "Don't know what the Data Received vector is called for the first UART"
#endif
  {
    Serial2._rx_complete_irq();
  }

#if defined(UART2_UDRE_vect)
ISR(UART2_UDRE_vect)
#elif defined(UART_UDRE_vect)
ISR(UART_UDRE_vect)
#elif defined(USART2_UDRE_vect)
ISR(USART2_UDRE_vect)
#elif defined(USART_UDRE_vect)
ISR(USART_UDRE_vect)
#else
  #error "Don't know what the Data Register Empty vector is called for the first UART"
#endif
{
  Serial2._tx_udr_empty_irq();
}

#if defined(UBRRH) && defined(UBRRL)
  HardwareSerial Serial2(&UBRRH, &UBRRL, &UCSRA, &UCSRB, &UCSRC, &UDR);
#else
  HardwareSerial Serial2(&UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UCSR2C, &UDR2);
#endif

// Function that can be weakly referenced by serialEventRun to prevent
// pulling in this file if it's not otherwise used.
bool Serial2_available() {
  return Serial2.available();
}

#endif // HAVE_HWSERIAL2
