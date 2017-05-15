#include <avr/io.h>
#include "spi.h"

#define DD_MOSI 3
#define DD_MISO 4
#define DD_SCK 5
#define DD_SS 2	// mast be in case of master!

volatile unsigned char SPI_Busy;

void WaitForSPI(void)
{
	do{
	}while(SPI_Busy);
}
void SPI_MasterInit(void)
{
	/* Set MOSI, SCK and SS as output, all others input */
	DDRB |= (1<<DD_MOSI)|(1<<DD_SCK) | (1<<DD_SS);
	SPSR = (1<<SPI2X);		// clk = fosc/2 this is the highest transfer rate! (must be corresponding to connected device)
	SPCR = (1<<SPE)|(1<<MSTR);
}

void SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

unsigned char SPI_MasterRW(char cData)
{
	SPI_MasterTransmit(cData);
	return(SPDR);
}
