#include <avr/io.h>
#include <stdbool.h>

enum {
	LED_OUT = 0,
	BUTTON1_IN = 1,
	BUTTON2_IN = 2,
	};

int main(void)
{
	DDRB |= 1 << LED_OUT;
	DDRB &= ~(1 << BUTTON1_IN);
	DDRB &= ~(1 << BUTTON2_IN);
	PORTB &= ~(1 << LED_OUT);
	
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


