#ifndef CALENDAR_EVENT_H
#define CALENDAR_EVENT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	char name[16 + 1];
	char startString[16 + 1];
	uint32_t startTime;
	uint32_t notifyMinutes;
} Event;

void getTimeRemainingString(Event event, char buffer[17], uint32_t currentTime);
bool eventIsNear(Event event, uint32_t currentTime);

#endif
