
#include <LwipEthernet.h>

#ifndef ETHERNET_SPI_CLOCK_DIV
#define ETHERNET_SPI_CLOCK_DIV SPI_CLOCK_DIV4 // 4MHz (SPI.h)
#endif

void SPI4EthInit()
{
    SPI.begin();
    SPI.setClockDivider(ETHERNET_SPI_CLOCK_DIV);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
}
