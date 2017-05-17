#include <avr/io.h>
#include "f_cpu.h"
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h"

#define rs PB0    //pin8
#define en PB1    //pin9

int base_address[8]={64,72,80,88,96,104,112,120};

void command(char LcdCommand)  // Basic function used in giving command
{                              // to LCD
	char UpperHalf, LowerHalf;

	UpperHalf = LcdCommand & 0xF0;	// upper 4 bits of command
	PORTD &= 0x0F;                  // flushes upper half of PortD to 0, but retains lower half of PortD
	PORTD |= UpperHalf;
	PORTB &= ~(1<<rs);
	PORTB |= (1<<en);
	_delay_ms(10);
	PORTB &= ~(1<<en);
	_delay_ms(10);

	LowerHalf = ((LcdCommand<<4) & 0xF0); //lower 4 bits of command
	PORTD &= 0x0F;                  // flushes upper half of PortD to 0, but retains lower half of PortD
	PORTD |= LowerHalf;
	PORTB &= ~(1<<rs);
	PORTB |= (1<<en);
	_delay_ms(10);
	PORTB &= ~(1<<en);
	_delay_ms(10);
}

void lcd_init()
{
	DDRB |= 0x03;    // PB0 and PB1 declared as output
	DDRD |= 0xF0;    // PD4,PD5,PD6,PD7 declared as output
	command(0x28);	// To initialize LCD in 2 lines, 5X8 dots and 4bit mode.
	command(0x0C);	// Display ON cursor OFF. E for cursor ON and C for cursor OFF
	command(0x06);	// Entry mode-increment cursor by 1
	command(0x01);	// Clear screen
	command(0x80);	// Sets cursor to (0,0)
}






void data(char AsciiChar)    // Basic function used in giving data
{                            // to LCD
	char UpperHalf, LowerHalf;

	UpperHalf = AsciiChar & 0xF0;	// upper 4 bits of data
	PORTD &= 0x0F;       // flushes upper half of PortD to 0, but retains lower half of PortD
	PORTD |= UpperHalf;
	PORTB |= (1<<rs);
	PORTB |= (1<<en);
	_delay_ms(10);
	PORTB &= ~(1<<en);
	_delay_us(400);

	LowerHalf = ((AsciiChar<<4) & 0xF0); //lower 4 bits of data
	PORTD &= 0x0F;       // flushes upper half of PortD to 0, but retains lower half of PortD
	PORTD |= LowerHalf;
	PORTB |= (1<<rs);
	PORTB |= (1<<en);
	_delay_ms(10);
	PORTB &= ~(1<<en);
	_delay_us(400);
}




void Send_A_String(char *StringOfCharacters)     // Takes input a string and displays it
{                                                // max character in a string should be
	while(*StringOfCharacters > 0)           // less than 16, after 16th character
	{                                        // everything will be ignored.
		data(*StringOfCharacters++);
	}
}

void setCursor(int row,int column)             // Indexing starts from 0.
{                                              // Therefore,
	switch(row)                            // 0<=row<=1 and 0<=column<=15.
	{                                      //
		case 0:                        // If arguments are outside the
		switch(column)         // the specified range, then function
		{                      // will not work and ignore the values
			case 0:
			command(0x80);break;
			case 1:
			command(0x81);break;
			case 2:
			command(0x82);break;
			case 3:
			command(0x83);break;
			case 4:
			command(0x84);break;
			case 5:
			command(0x85);break;
			case 6:
			command(0x86);break;
			case 7:
			command(0x87);break;
			case 8:
			command(0x88);break;
			case 9:
			command(0x89);break;
			case 10:
			command(0x8A);break;
			case 11:
			command(0x8B);break;
			case 12:
			command(0x8C);break;
			case 13:
			command(0x8D);break;
			case 14:
			command(0x8E);break;
			case 15:
			command(0x8F);break;
			default:
			break;
		}
		break;
		case 1:
		switch(column)
		{
			case 0:
			command(0xC0);break;
			case 1:
			command(0xC1);break;
			case 2:
			command(0xC2);break;
			case 3:
			command(0xC3);break;
			case 4:
			command(0xC4);break;
			case 5:
			command(0xC5);break;
			case 6:
			command(0xC6);break;
			case 7:
			command(0xC7);break;
			case 8:
			command(0xC8);break;
			case 9:
			command(0xC9);break;
			case 10:
			command(0xCA);break;
			case 11:
			command(0xCB);break;
			case 12:
			command(0xCC);break;
			case 13:
			command(0xCD);break;
			case 14:
			command(0xCE);break;
			case 15:
			command(0xCF);break;
			default:
			break;
		}
		break;
		default:
		break;
	}

}




void clearScreen()         // Clears the screen and
{                          // returns cursor to (0,0) position
	command(0x01);
}




