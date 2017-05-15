#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "event.h"
#include "uart.h"
#include "lcd.h"

typedef enum {
	DISPLAY_STARTTIME,
	DISPLAY_REMAINING,
} DisplayMode;

typedef enum {
	COMMAND_ADD,
	COMMAND_REMOVE,
} CommandType;

enum {
	LED_OUT = 0,
	BUTTON1_IN = 1,
	BUTTON2_IN = 2,
};

static uint8_t currentEvent = 0;
static uint8_t eventCount = 0;
static Event events[16];
static DisplayMode displayMode;
static uint32_t currentTime = 0;

void updateCurrentTime(char command[256]);
void pushButton1(void);
void pushButton2(void);
void setLedStatus(void);
bool oneEventIsNear(void);
void printTopRow(char text[17]);
void printBottomRow(char text[17]);
void displayEvent(void);
Event processBuilderString(char command[256]);
bool addEvent(char command[256]);
bool removeEvent(char command[256]);
void removeAndReorder(uint8_t index);
void processCommand(char command[256]);

void updateCurrentTime(char command[256]) {
	sscanf(command, "tim %lu", &currentTime);
	setLedStatus();
	displayEvent();
}

void pushButton1(void) {
	if (eventCount == 0)
	return;
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
		PORTB |= 1 << LED_OUT;
		} else {
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

void printTopRow(char text[17]) {
	//printf("%s\n", text);
}

void printBottomRow(char text[17]) {
	//printf("%s\n", text);
}

void displayEvent(void) {
	char buffer[17];

	if (eventCount == 0) {
		printTopRow("No events");
		printBottomRow("scheduled.");
		return;
	}

	switch (displayMode) {
		case DISPLAY_REMAINING:
		getTimeRemainingString(events[currentEvent], buffer, currentTime);
		break;
		case DISPLAY_STARTTIME:
		strcpy(buffer, events[currentEvent].startString);
		break;
	}

	printTopRow(events[currentEvent].name);
	printBottomRow(buffer);
}

Event processBuilderString(char command[256]) {
	char* temp;
	char data[4][17];
	uint8_t i = 0;

	temp = strtok(command, ";");

	while (temp != NULL) {

		strcpy(data[i++], temp);
		temp = strtok(NULL, ";");

	}

	Event event;
	strcpy(event.name, data[0]);
	strcpy(event.startString, data[1]);
	uint32_t startTime;
	sscanf(data[2], "%lu", &startTime);
	event.startTime = startTime;
	uint32_t notifyMinutes;
	sscanf(data[3], "%lu", &notifyMinutes);
	event.notifyMinutes = notifyMinutes;
	return event;
}

// add example cd event;2017-02-03 23:53;1494151200;30

bool addEvent(char command[256]) {
	if (eventCount == 16)
	return false;

	events[eventCount++] = processBuilderString(command + 4);
	return true;
}

bool removeEvent(char command[256]) {
	bool removed = false;
	for (uint8_t i = 0; i < eventCount; ++i) {
		if (strcmp(events[i].name, command)) {
			removeAndReorder(i);
			removed = true;
		}
	}
	return removed;
}

void removeAndReorder(uint8_t index) {
	for (uint8_t i = index; i < eventCount - 1; ++i) {
		events[i] = events[i + 1];
	}
	eventCount--;
}

void processCommand(char command[256]) {
	char type[4];
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
}

#define BAUD (long)9600
#define Fcpu 16000000
#define UBRR_VALUE  (unsigned int)((Fcpu/(16*BAUD)-1) & 0x0fff)

int main() {
	DDRB |= 1 << LED_OUT;
	DDRB &= ~(1 << BUTTON1_IN);
	DDRB &= ~(1 << BUTTON2_IN);
	PORTB &= ~(1 << LED_OUT);

		unsigned char UART_data;
		uint8_t index = 0;
		char str[16 + 1];
		bool receivedCommand = false;

		USART0_Init(UBRR_VALUE);
		LCD_init();
		sei(); // IT enable

	bool button1 = false;
	bool button2 = false;

	displayEvent();
	LCD_write_str("  UART sample   ",line1);
	while (1) {
		receivedCommand = false;
		if (GET_UART(&UART_data)) {
			if (UART_data == '\r') {
				receivedCommand = true;
				str[index] = '\0';
				index = 0;
			} else {
				str[index++] = UART_data;
			}
		}

		button1 = (PINB & (1 << BUTTON1_IN)) && !button1;
		button2 = (PINB & (1 << BUTTON2_IN)) && !button2;

		if (receivedCommand) {
			processCommand(str);
		}

		if (button1) {
			pushButton1();
			PORTB ^= 1 << LED_OUT;
		}

		if (button2) {
			pushButton2();
			PORTB ^= 1 << LED_OUT;
		}
	}
}
