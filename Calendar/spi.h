#ifndef spi_h
#define spi_h

void SPI_MasterInit(void);
void SPI_MasterTransmit(char cData);
unsigned char SPI_MasterRW(char cData);
void WaitForSPI(void);
extern volatile unsigned char SPI_Busy;

#endif
