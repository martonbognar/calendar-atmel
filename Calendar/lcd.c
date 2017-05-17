#include <avr/io.h>
#include "f_cpu.h"
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h"

#define rs PB0
#define en PB1

void sendLcdCommand(uint8_t command) {
	uint8_t upperBits = command & 0xF0;	// felso negy bit kivalasztasa
	PORTD &= 0x0F;
	PORTD |= upperBits;
	PORTB &= ~(1 << rs);
	PORTB |= (1 << en);
	_delay_ms(10);
	PORTB &= ~(1 << en);
	_delay_ms(10);

	uint8_t lowerBits = ((command << 4) & 0xF0); // also negy bit kivalasztasa
	PORTD &= 0x0F;
	PORTD |= lowerBits;
	PORTB &= ~(1 << rs);
	PORTB |= (1 << en);
	_delay_ms(10);
	PORTB &= ~(1 << en);
	_delay_ms(10);
}

void lcdInit() {
	DDRB |= 0x03;
	DDRD |= 0xF0;
	sendLcdCommand(0x28);	// a kijelzot inicializalo parancsok
	sendLcdCommand(0x0C);
	sendLcdCommand(0x06);
	sendLcdCommand(0x01);
	sendLcdCommand(0x80);
}

void sendLcdData(char character) {
	uint8_t UpperHalf = character & 0xF0;	// upper 4 bits of data
	PORTD &= 0x0F;       // flushes upper half of PortD to 0, but retains lower half of PortD
	PORTD |= UpperHalf;
	PORTB |= (1 << rs);
	PORTB |= (1 << en);
	_delay_ms(10);
	PORTB &= ~(1 << en);
	_delay_us(400);

	uint8_t LowerHalf = ((character << 4) & 0xF0); //lower 4 bits of data
	PORTD &= 0x0F;       // flushes upper half of PortD to 0, but retains lower half of PortD
	PORTD |= LowerHalf;
	PORTB |= (1 << rs);
	PORTB |= (1 << en);
	_delay_ms(10);
	PORTB &= ~(1 << en);
	_delay_us(400);
}

void displayText(char * string) {
	while(*string > 0) {
		sendLcdData(*string++);
	}
}

void setTopRowActive() {
	sendLcdCommand(0x80);
}

void setBottomRowActive() {
	sendLcdCommand(0xC0);
}

void clearScreen() {
	sendLcdCommand(0x01);
}
