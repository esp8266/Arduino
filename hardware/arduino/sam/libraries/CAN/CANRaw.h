#ifndef _CAN_LIBRARY_
#define _CAN_LIBRARY_

#include "sn65hvd234.h"

class CANRawClass
{
  protected:
    /* CAN peripheral, set by constructor */
    Can* m_pCan ;

    /* CAN Transceiver */
    SSN65HVD234_Data m_Transceiver ;

    /** CAN Transfer */
    can_mb_conf_t m_Mailbox ;

  private:

  public:
    // Constructor
    CANRawClass( Can* pCan ) ;

    uint32_t begin( uint32_t dwPinEN, uint32_t dwPinRs, uint32_t dwBaudrate ) ;
    uint32_t end( void ) ;

} ;

extern CANRawClass CANRaw0 ;
extern CANRawClass CANRaw1 ;

#endif // _CAN_LIBRARY_
