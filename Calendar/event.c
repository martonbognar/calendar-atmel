#include "event.h"
#include <stdio.h>
#include <string.h>

// visszaadja a parameterkent kapott esemeny es idobelyeg alapjan a bufferbe a hatralevo idot szoveges formaban
void getTimeRemainingString(Event event, char * buffer, uint32_t currentTime) {
	// ha mar elmult az esemeny
	if (event.startTime < currentTime) {
		strcpy(buffer, "event has passed");
		return;
	}
	uint32_t remainingMinutes = (event.startTime - currentTime) / 60;
	sprintf(buffer, "remaining: %lu", remainingMinutes);
}

// visszateresi erteke megadja, hogy a parameterkent atvett esemeny az idobelyeg alapjan kozelinek szamit-e
bool eventIsNear(Event event, uint32_t currentTime) {
	if (event.startTime < currentTime) {
		return false;
	}
	// a masodpercek kulonbseget percce alakitjuk, majd ezt hasonlitjuk
	return ((event.startTime - currentTime) / 60) < event.notifyMinutes;
}
