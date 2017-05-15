#ifndef lcd_h
#define lcd_h

#define line1 1
#define line2 2
#define left 0
#define right 1
//functions to use
extern void Delaysw(long t);
extern void LCD_init(void);							// Initialisation must call after enabled IT
extern void LCD_write_str(char * s, unsigned char line);		//write string to lcd

#endif
