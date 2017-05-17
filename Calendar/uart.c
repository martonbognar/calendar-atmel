#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdbool.h>
#include "uart.h"

#define bufferSize 128

static char inputBuffer[bufferSize];

volatile unsigned char  wp;                      /* write pointer */
volatile unsigned char  rp;                      /* read pointer */
volatile unsigned char  sin_num;                 /* data number in input buffer */

void uartInit(unsigned int baud_set_value) {
	UBRR0H = (unsigned char)(baud_set_value>>8);
	UBRR0L = (unsigned char)baud_set_value;

	UCSR0A = 0x00;

	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // 8 bit adat 1 stop bit
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<< RXCIE0); // tr. & rec en, rec. IT en.

	sin_num = 0;
	wp=0;
	rp=0;
}

static volatile unsigned char uart_err;

ISR(USART_RX_vect) 		// v�teli interrupt rutin
{
	char dumy;
	dumy = UCSR0A; 	   		// a statust ki kell olvasni az adat elott!
	if(dumy & (1<<FE0))
	{
		uart_err = 1<<FE0;
	}
	if(dumy & (1<<DOR0))
	{
		uart_err = 1<<DOR0;
	}
	*(inputBuffer+wp) = UDR0;   // adat a bufferbe
	//SER_IN[wp] = UDR1;
	wp =(wp+1) % bufferSize;	// pointer modulo �ll�t�sa
	if (sin_num<bufferSize)	// ha nincs fel�l�r�s
	{
		sin_num+=1;
	}
	else					// ha fel�l�rtuk a legr�gebbit
	{
		//buf_ovr = 1;	// jelz�s, tele a buffer, elvesz a legr�gebbi adat!
		sin_num = bufferSize;
		rp = wp;		// olvas� pointer az �r� ut�n mutat
		// mivel mostm�r ez a legr�gebbi adat
	}
}

bool getCharacterFromUart(unsigned char * UARTvar)
{
	if (sin_num>0)
	{
		*UARTvar = *(inputBuffer+rp);
		rp = (rp+1) % bufferSize;
		sin_num-=1;
		return true;
	}
	else
	{
		return false;
	}
}
