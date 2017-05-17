#define BAUD (long)9600
#define UBRR_VALUE  (unsigned int)((F_CPU/(16*BAUD)-1) & 0x0fff)

#define rs PB0
#define en PB1

#define MAX_EVENTS 8
#define LINE_LENGTH 17

#include <avr/io.h>
#include <avr/interrupt.h>
#include "f_cpu.h"
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "event.h"
#include "uart.h"
#include "lcd.h"

// megjelenitesi mod
typedef enum {
	DISPLAY_STARTTIME,
	DISPLAY_REMAINING,
} DisplayMode;

// periferiak pinjei
enum {
	LED_OUT = 2,
	BUTTON1_IN = 3,
	BUTTON2_IN = 4,
};

// kivalasztott elem sorszama
static uint8_t currentEvent = 0;
// osszes elemszam
static uint8_t eventCount = 0;
// esemenyek tombje
static Event events[MAX_EVENTS];
// kivalasztott megjelenitesi mod
static DisplayMode displayMode = DISPLAY_STARTTIME;
// aktualis timestamp
static uint32_t currentTime = 0;

// globalis string buffer
static char buffer[LINE_LENGTH];

// a fuggvenyek leirasa megtalalhato a dokumentacioban,
// de igyekeztem mindennek ertelmes neveket adni, hogy konnyen ertheto legyen
void updateCurrentTime(char * command);
void printCurrentTime(void);
void pushButton1(void);
void pushButton2(void);
void setLedStatus(void);
bool oneEventIsNear(void);
void printTopRow(char * text);
void printBottomRow(char * text);
void displayEvent(void);
Event processBuilderString(char * command);
bool addEvent(char * command);
bool removeEvent(char * command);
void removeAndReorder(uint8_t index);
void processCommand(char * command);

void updateCurrentTime(char * command) {
	sscanf(command, "tim %lu", &currentTime);
	setLedStatus();
	printCurrentTime();
}

void printCurrentTime(void) {
	clearScreen();
	sprintf(buffer, "tim: %lu", currentTime);
	printTopRow(buffer);
	_delay_ms(1000);
	displayEvent();
}

void pushButton1(void) {
	if (eventCount == 0) {
		displayEvent();
		return;
	}
	currentEvent = (currentEvent + 1) % eventCount;
	displayEvent();
}

void pushButton2(void) {
	if (displayMode == DISPLAY_STARTTIME) {
		displayMode = DISPLAY_REMAINING;
		} else {
		displayMode = DISPLAY_STARTTIME;
	}

	displayEvent();
}

void setLedStatus(void) {
	if (oneEventIsNear()) {
		// ha van kozeli esemeny, bekapcsoljuk a ledet
		PORTB |= 1 << LED_OUT;
	} else {
		// egyebkent ki
		PORTB &= ~(1 << LED_OUT);
	}
}

bool oneEventIsNear(void) {
	bool isNear = false;
	for (uint8_t i = 0; i < eventCount; ++i) {
		if (eventIsNear(events[i], currentTime)) {
			isNear = true;
		}
	}
	return isNear;
}

void printTopRow(char * text) {
	_delay_ms(200);
	setTopRowActive();
	displayText(text);
}

void printBottomRow(char * text) {
	_delay_ms(200);
	setBottomRowActive();
	displayText(text);
}

void displayEvent(void) {
	clearScreen();

	if (eventCount == 0) {
		printTopRow("No events");
		printBottomRow("scheduled.");
		return;
	}

	printTopRow(events[currentEvent].name);

	switch (displayMode) {
		case DISPLAY_REMAINING:
			// kiirjuk a hatralevo percek szamat
			getTimeRemainingString(events[currentEvent], buffer, currentTime);
			printBottomRow(buffer);
			break;
		case DISPLAY_STARTTIME:
			// kiirjuk az elore megadott hataridot
			strcpy(buffer, events[currentEvent].startString);
			printBottomRow(buffer);
			break;
	}
}

Event processBuilderString(char * command) {
	char* temp;
	char data[4][LINE_LENGTH];
	uint8_t i = 0;

	// feldolgozzuk a pontosvesszokkel elvalasztott adatokat
	temp = strtok(command, ";");

	while (temp != NULL) {
		strcpy(data[i++], temp);
		temp = strtok(NULL, ";");
	}

	Event event;
	strcpy(event.name, data[0]);
	strcpy(event.startString, data[1]);
	sscanf(data[2], "%lu", &(event.startTime));
	sscanf(data[3], "%lu", &(event.notifyMinutes));
	return event;
}

bool addEvent(char * command) {
	// nem adjuk hozza, ha mar nincs hely
	if (eventCount == MAX_EVENTS)
		return false;

	events[eventCount++] = processBuilderString(command + 4);
	displayEvent();
	return true;
}

bool removeEvent(char * command) {
	bool removed = false;
	for (uint8_t i = 0; i < eventCount; ++i) {
		// vizsgaljuk a nevvel valo egyezest
		if (strcmp(events[i].name, command + 4) == 0) {
			removeAndReorder(i);
			removed = true;
		}
	}

	if (removed) {
		displayEvent();
	}

	return removed;
}

void removeAndReorder(uint8_t index) {
	for (uint8_t i = index; i < eventCount - 1; ++i) {
		events[i] = events[i + 1];
	}
	eventCount--;
	// ha az utolsot toroltuk ki, ne ervenytelen helyre mutasson az index
	if (currentEvent >= eventCount) {
		currentEvent = 0;
	}
}

void processCommand(char * command) {
	char type[4];
	// minden parancs 3 karakteres azonositoval rendelkezik, azt vizsgaljuk
	strncpy(type, command, 3);
	type[3] = '\0';
	if (strcmp(type, "add") == 0) {
		addEvent(command);
	}
	if (strcmp(type, "rmv") == 0) {
		removeEvent(command);
	}
	if (strcmp(type, "tim") == 0) {
		updateCurrentTime(command);
	}
	if (strcmp(type, "led") == 0) {
		PORTB ^= 1 << LED_OUT;
	}
	if (strcmp(type, "pct") == 0) {
		printCurrentTime();
	}
}

int main() {
	DDRB |= 1 << LED_OUT;
	DDRB &= ~(1 << BUTTON1_IN);
	DDRB &= ~(1 << BUTTON2_IN);
	PORTB &= ~(1 << LED_OUT);

	char UART_data;
	uint8_t index = 0;
	char str[256];
	bool receivedCommand = false;

	uartInit(UBRR_VALUE);
	_delay_ms(200);
	lcdInit();
	sei();

	bool button1 = false;
	bool button2 = false;

	_delay_ms(200);
	clearScreen();
	displayEvent();

	// az idozito megirasahoz ezt az oldalt hasznaltam:
	// https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
	OCR1A = 0x3D08;
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS12) | (1 << CS10);

	while (1) {
		receivedCommand = false;
		// ha erkezett adat, azt feldolgozzuk
		if (getCharacterFromUart(&UART_data)) {
			if (UART_data == '\r') {
				// ha megerkezett a lezaro karakter
				receivedCommand = true;
				str[index] = '\0';
				index = 0;
			} else {
				// egyebkent belefuzzuk
				str[index++] = UART_data;
			}
		}

		// itt vizsgaljuk a gombok allapotat
		button1 = (PINB & (1 << BUTTON1_IN)) && !button1;
		button2 = (PINB & (1 << BUTTON2_IN)) && !button2;

		if (receivedCommand) {
			processCommand(str);
		}

		if (button1) {
			pushButton1();
		}

		if (button2) {
			pushButton2();
		}
	}
}

// a masodpercek itt porognek
ISR (TIMER1_COMPA_vect) {
	currentTime++;
	setLedStatus();
}
