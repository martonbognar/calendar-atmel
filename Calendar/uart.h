#ifndef uart_h
#define uart_h

#include <stdbool.h>

void uartInit(unsigned int baud_set_value);
bool getCharacterFromUart(char * character);

#endif
