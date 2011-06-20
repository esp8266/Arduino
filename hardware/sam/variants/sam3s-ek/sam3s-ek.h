
#include "Arduino.h"

extern UARTClass Serial ;
extern UARTClass UART2 ;

extern USARTClass USART1 ;
extern USARTClass USART2 ;

#if defined(USBCON)
  #include "usb_api.h"
#endif

