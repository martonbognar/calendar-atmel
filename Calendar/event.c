#include "event.h"
#include <stdio.h>
#include <string.h>

void getTimeRemainingString(Event event, char * buffer, uint32_t currentTime) {
	uint32_t remainingMinutes = (event.startTime - currentTime) / 60;
	sprintf(buffer, "remaining: %lu", remainingMinutes);
}

bool eventIsNear(Event event, uint32_t currentTime) {
	// a masodpercek kulonbseget percce alakitjuk, majd ezt hasonlitjuk
	return (event.startTime - currentTime) / 60 < event.notifyMinutes;
}
