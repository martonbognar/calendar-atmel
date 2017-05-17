#ifndef lcd_h
#define lcd_h

void lcd_init();
void Send_A_String(char *StringOfCharacters);
void setCursor(int row,int column);
void clearScreen();

#endif