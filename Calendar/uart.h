#ifndef uart_h
#define uart_h

void uart_init(unsigned int baud_set_value);
unsigned char get_uart_character(unsigned char * UARTvar);

#endif
