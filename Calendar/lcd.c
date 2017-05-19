#include <avr/io.h>
#include "f_cpu.h"
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lcd.h"

#define rs PB0
#define en PB1

// elkuldi egy parancs (adat vagy vezerlo) felso 4 bitjet
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

// elkuldi egy parancs (adat vagy vezerlo) also 4 bitjet
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

// a parameterkent atvett kodu utasitast meghivja a kijelzon
void sendLcdCommand(uint8_t command) {
	sendTopHalf(command, true);
	_delay_ms(10);
	sendBottomHalf(command, true);
	_delay_ms(10);
}

// inicializalja a kijelzot es a hozza tartozo portokat
void lcdInit() {
	DDRB |= 0x03;
	DDRD |= 0xF0;
	_delay_ms(150); // 4 bites kapcsolat inicializalasa
	sendBottomHalf(0x03, true);
	_delay_ms(50);
	sendBottomHalf(0x03, true);
	_delay_ms(50);
	sendBottomHalf(0x03, true);
	_delay_ms(50);
	sendBottomHalf(0x02, true);
	_delay_ms(50);
	sendLcdCommand(0x28);	// a kijelzot inicializalo parancsok, letorlese, kurzor kezdopozicioba helyezese
	sendLcdCommand(0x0C);
	sendLcdCommand(0x06);
	sendLcdCommand(0x01);
	sendLcdCommand(0x80);
}

// elkuldi a parameterben kapott karaktert a kijelzore
void sendLcdData(char character) {
	sendTopHalf(character, false);
	_delay_us(400);
	sendBottomHalf(character, false);
	_delay_us(400);
}

// elkuldi a parameterben kapott karaktertombot a kijelzore
void displayText(char * string) {
	while(*string != '\0') {
		sendLcdData(*string++);
	}
}

// a kijelzo felso soraba helyezi a kurzort
void setTopRowActive() {
	sendLcdCommand(0x80);
}

// a kijelzo also soraba helyezi a kurzort
void setBottomRowActive() {
	sendLcdCommand(0xC0);
}

// letorli a kijelzot
void clearScreen() {
	sendLcdCommand(0x01);
}
