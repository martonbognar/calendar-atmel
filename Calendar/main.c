#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "event.h"

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
	sscanf(command, "tim %u", &currentTime);
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
	printf("%s\n", text);
}

void printBottomRow(char text[17]) {
	printf("%s\n", text);
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

		printf("%s", temp);
		printf("\n");
		strcpy(data[i++], temp);
		temp = strtok(NULL, ";");

	}

	Event event;
	strcpy(event.name, data[0]);
	strcpy(event.startString, data[1]);
	uint32_t startTime;
	sscanf(data[2], "%u", &startTime);
	event.startTime = startTime;
	uint32_t notifyMinutes;
	sscanf(data[3], "%u", &notifyMinutes);
	event.notifyMinutes = notifyMinutes;
	return event;
}

// add example cd event;2017-02-03 23:53;1494151200;30

bool addEvent(char command[256]) {
	if (eventCount == 16)
	return false;

	printf("COmmand: %s\n", command);

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
}

int main() {
	DDRB |= 1 << LED_OUT;
	DDRB &= ~(1 << BUTTON1_IN);
	DDRB &= ~(1 << BUTTON2_IN);
	PORTB &= ~(1 << LED_OUT);
	
	bool button1;
	bool button2;
	
	displayEvent();
	while (1) {
		button1 = PINB & (1 << BUTTON1_IN) && !button1;
		button2 = PINB & (1 << BUTTON2_IN) && !button2;
		
		if (button1) {
			pushButton1();
		}
		
		if (button2) {
			pushButton2();
		}
	}
	return 0;
}






int main(void)
{

	
    while (1) 
    {
		bool button1 = PINB & (1 << BUTTON1_IN);
		bool button2 = PINB & (1 << BUTTON2_IN);
		
		bool led = PINB & (1 << LED_OUT);
		
		if ((button1 && !led) || (button2 && led)) {
			PORTB ^= 1 << LED_OUT;
			led = ~led;
		}
			
    }
}


