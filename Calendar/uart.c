#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "uart.h"

#define serin_lng (unsigned char)128

unsigned char dumy;

static unsigned char  SER_IN[serin_lng]; /* input buffer */

unsigned char  *seribuf; /* cyclic input buffer */
volatile unsigned char  wp;                      /* write pointer */
volatile unsigned char  rp;                      /* read pointer */
volatile unsigned char  sin_num;                 /* data number in input buffer */

void USART0_Init(unsigned int baud_set_value) {
	UBRR0H = (unsigned char)(baud_set_value>>8);
	UBRR0L = (unsigned char)baud_set_value;

	UCSR0A = 0x00;

	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // 8 bit adat 1 stop bit
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<< RXCIE0); // tr. & rec en, rec. IT en.

	sin_num = 0;
	wp=0;
	rp=0;
	seribuf = &SER_IN[0];
}

static volatile unsigned char uart_err;

ISR(USART_RX_vect) 		// vételi interrupt rutin
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
	*(seribuf+wp) = UDR0;   // adat a bufferbe
	//SER_IN[wp] = UDR1;
	wp =(wp+1) % serin_lng;	// pointer modulo állítása
	if (sin_num<serin_lng)	// ha nincs felülírás
	{
		sin_num+=1;
	}
	else					// ha felülírtuk a legrégebbit
	{
		//buf_ovr = 1;	// jelzés, tele a buffer, elvesz a legrégebbi adat!
		sin_num = serin_lng;
		rp = wp;		// olvasó pointer az író után mutat
		// mivel mostmár ez a legrégebbi adat
	}
}

unsigned char GET_UART(unsigned char * UARTvar)
{
	if (sin_num>0)
	{
		*UARTvar = *(seribuf+rp);
		rp = (rp+1) % serin_lng;
		sin_num-=1;
		return(1);
	}
	else
	{
		return(0);
	}
}
