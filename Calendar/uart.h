#ifndef uart_h
#define uart_h

extern void USART0_Init(unsigned int baud_set_value,unsigned char dspeed);
// receive datas
extern unsigned char S_DAT;        //SIO bejovo adat (GET_SIO()-tol)
extern unsigned char GET_UART(unsigned char * UARTvar);

#endif
