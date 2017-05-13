#include "event.h"
#include <stdio.h>

void getTimeRemainingString(Event event, char buffer[17], uint32_t currentTime) {
	uint32_t remainingMinutes = (event.startTime - currentTime) / 60;
	sprintf(buffer, "%u", remainingMinutes);
}

bool eventIsNear(Event event, uint32_t currentTime) {
	return (event.startTime - currentTime) / 60 < event.notifyMinutes;
}
