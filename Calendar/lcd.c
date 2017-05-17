#include <avr/io.h>
#include "f_cpu.h"
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lcd.h"

#define rs PB0
#define en PB1

static void sendTopHalf(uint8_t byte, bool isCommand) {
	uint8_t upperBits = byte & 0xF0;	// felso negy bit kivalasztasa
	PORTD &= 0x0F;
	PORTD |= upperBits;	// D4-D7-re feltoltes
	if (isCommand) { // ha parancs, akkor alacsony, ha adat, akkor magas az rs
		PORTB &= ~(1 << rs);
	} else {
		PORTB |= (1 << rs);
	}
	PORTB |= (1 << en);	// engedelyezes
	_delay_ms(10);
	PORTB &= ~(1 << en);
}

static void sendBottomHalf(uint8_t byte, bool isCommand) {
	uint8_t lowerBits = ((byte << 4) & 0xF0); // also negy bit kivalasztasa
	PORTD &= 0x0F;
	PORTD |= lowerBits;
	if (isCommand) {
		PORTB &= ~(1 << rs);
	} else {
		PORTB |= (1 << rs);
	}
	PORTB |= (1 << en);
	_delay_ms(10);
	PORTB &= ~(1 << en);
}

void sendLcdCommand(uint8_t command) {
	sendTopHalf(command, true);
	_delay_ms(10);
	sendBottomHalf(command, true);
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
	sendTopHalf(character, false);
	_delay_us(400);
	sendBottomHalf(character, false);
	_delay_us(400);
}

void displayText(char * string) {
	while(*string != '\0') {
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
