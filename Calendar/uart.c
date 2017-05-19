#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdbool.h>
#include "uart.h"

#define bufferSize 128
static char inputBuffer[bufferSize];

volatile uint8_t writeIndex;
volatile uint8_t readIndex;
volatile uint8_t characterCount;

// inicializalja az uart kapcsolatot
void uartInit(unsigned int baud_set_value) {
	UBRR0H = (unsigned char) (baud_set_value >> 8);
	UBRR0L = (unsigned char) baud_set_value;

	UCSR0A = 0x00;

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 adat bit, 1 stop bit
	UCSR0B = (1 << RXEN0) | (1 << RXCIE0); // fogadas es interrupt engedelyezes

	characterCount = 0;
	writeIndex = 0;
	readIndex = 0;
}

// ez az interrupt hivodik adat erkezesekor
ISR(USART_RX_vect) {
	inputBuffer[writeIndex] = UDR0;   // beolvasas
	writeIndex = (writeIndex + 1) % bufferSize;	// korkoros pointer eltolasa
	if (characterCount < bufferSize) {
		characterCount++;
	} else {
		readIndex = writeIndex;	// itt elveszik az adat!
	}
}

// ezzel a fuggvennyel kerhetjuk le a bufferben levo kovetkezo karaktert a parameterkent atvett pointerbe, a visszateresi ertek megadja, hogy volt-e uj karakter
bool getCharacterFromUart(char * character) {
	if (characterCount == 0) {
		return false;
	}

	*character = inputBuffer[readIndex];
	readIndex = (readIndex + 1) % bufferSize;
	characterCount--;
	return true;
}
