#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "spi.h"
#include "lcd.h"

//PORTC
#define SS_LCD 3
#define SET_SS_LCD PORTC |= (1 << SS_LCD)
#define CLR_SS_LCD PORTC &= ~(1 << SS_LCD)

volatile int LCD_WR_RDY;
volatile int StrIndex;
volatile char * StrPtr;
volatile char ITstr [20];
volatile unsigned char LCDline;
volatile unsigned char LCDpos;

static char tmpstr1[20];
static char tmpstr2[20];

void LCD_DATA(char data);
void SetLineAndPos(unsigned char line, unsigned char pos);

// t[msec]
void Delaysw(long t)
{
	t = (long)(160*(float)t);
	for(long i = 0; i<t; i++)
	{
		char tmp =	PORTD;
	}
}


//D7 D6 D5 D4 RS E X X
#define RS 0x08
#define E 0x04

#define CLR_DISP 0x01
#define CR 0x02
#define CGRAM_ADDR 0x40
#define DDRAM_ADDR 0x80
#define LINE1 DDRAM_ADDR
#define LINE2 (DDRAM_ADDR | 0x40)

//write to shiftregister + register connected to LCD interface
void LCD_REG_WR(unsigned char data)
{
	CLR_SS_LCD;
	SPI_MasterTransmit(data);	//write to shiftregister
	SET_SS_LCD;					//write content of shr to reg
}

// write one half of LCD command
void LCD_COMM_HALF(unsigned char com4bits)
{
	unsigned char com;
	com = (com4bits << 4);
	LCD_REG_WR(com);
	com = (com4bits << 4) | E;
	LCD_REG_WR(com);
	com = (com4bits << 4);
	LCD_REG_WR(com);
}

// write LCD command
void LCD_COMM(unsigned char com)
{
	unsigned char com_tmp;
	com_tmp = com >> 4;
	LCD_COMM_HALF(com_tmp);
	com_tmp = com & 0x0f;
	LCD_COMM_HALF(com_tmp);
}

// write one half of LCD data
void LCD_DATA_HALF(char data4bits)
{
	unsigned char data;
	data = (data4bits << 4) | RS;
	LCD_REG_WR(data);
	data = (data4bits << 4) | RS | E;
	LCD_REG_WR(data);
	data = (data4bits << 4) | RS;
	LCD_REG_WR(data);
}

// write LCD data
void LCD_DATA(char data)
{
	char data_tmp;
	data_tmp = data >> 4;		// MSB first
	LCD_DATA_HALF(data_tmp);
	data_tmp = data & 0x0f;
	LCD_DATA_HALF(data_tmp);
}


void LCD_init(void)
{
	DDRC |= (1 << SS_LCD);		//SS_LCD direction: out
	SET_SS_LCD;					//SS_LCD inactive
	SPI_MasterInit();
	LCD_WR_RDY = 1;
	// set 4 bits mode
	Delaysw(150);
	LCD_COMM_HALF(0x03);
	Delaysw(50);
	LCD_COMM_HALF(0x03);
	Delaysw(50);
	LCD_COMM_HALF(0x03);
	Delaysw(50);
	LCD_COMM_HALF(0x02);
	Delaysw(50);
	// 4 bits mode ready
	LCD_COMM(0x28);		//function set 0 0 1 DL N F x x   2 lines
	Delaysw(50);
	LCD_COMM(0x08);		//display off  0 0 0 0 1 D C B   disp. off, curs. off, blink off
	Delaysw(50);
	LCD_COMM(0x01);		//clear and home 0 0 0 0 0 0 0 1
	Delaysw(50);
	LCD_COMM(0x0c);		// dp. on, curs. on, blink on
	Delaysw(50);
	LCD_COMM(0x06);		// Entry mode 0 0 0 0 0 1 I/D S dp. on, curs. on, blink on
	Delaysw(50);
}

//Set LCD line (1 or 2) and pos (0-15) to write

void SetLineAndPos(unsigned char line, unsigned char pos)
{
	if(line == 1)
	{
		LCD_COMM(LINE1+pos);
	}
	else
	{
		LCD_COMM(LINE2+pos);
	}
}


void CLR_DISPLAY(void)
{
	LCD_COMM(CLR_DISP);
}

//copy l pieces of characters
void cpy_str(char * sd, char * ss, int l)  // sd[l-1:0] = ss[l-1:0]
{
	int i;
	int len;
	int sl;

	sl = strlen(ss);

	if(l > sl)
	{
		len = sl;
	}
	else
	{
		len = l;
	}

	for(i=0; i<len; i++)
	{
		sd[i] = ss[i];
	}
	sd[i]=0;
}

//complete string shorter then 16 char with space
void complete_str_n(char * s, int n)
{
	int l = strlen(s);
	if(l < n)
	{
		for(int i=0; i<=(n-l); i++ )
		{
			strcat(s," ");
		}
	}
}

//write string to LCD line
void LCD_write_str(char * s, unsigned char line)
{
	//char tmpstr[20];

	cpy_str(tmpstr1,s,16);
	complete_str_n(tmpstr1,16);

	do{} while(!LCD_WR_RDY);

	strcpy(ITstr,tmpstr1);
	//StrPtr = ITstr;
	StrIndex = -1;
	LCDline = line;
	LCDpos = 0;
	LCD_WR_RDY = 0;
}
