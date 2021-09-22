
#include <LwipEthernet.h>

void SPI4EthInit()
{
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz?
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
}
