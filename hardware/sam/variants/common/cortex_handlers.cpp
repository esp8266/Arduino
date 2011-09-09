#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void NMI_Handler( void )
{
  for ( ;; ) ;
}

extern void HardFault_Handler( void )
{
  for ( ;; ) ;
}

extern void MemManage_Handler( void )
{
  for ( ;; ) ;
}

extern void BusFault_Handler( void )
{
  for ( ;; ) ;
}

extern void UsageFault_Handler( void )
{
  for ( ;; ) ;
}

extern void SVC_Handler( void )
{
  for ( ;; ) ;
}

extern void DebugMon_Handler( void )
{
  for ( ;; ) ;
}

extern void PendSV_Handler( void )
{
  for ( ;; ) ;
}

#ifdef __cplusplus
}
#endif

