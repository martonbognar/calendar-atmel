#ifndef lcd_h
#define lcd_h

void start();
void command(char);
void data(char);
void Send_A_String(char *StringOfCharacters);
void cut(char *str);
void Send_An_Integer(int x);
void setCursor(int row,int column);
void clearScreen();
void home();
void cursor();
void noCursor();
void blink();
void noBlink();
void display();
void noDisplay();
void scrollDisplayLeft();
void scrollDisplayRight();
void autoscroll();
void noAutoscroll();
void createChar(int num,unsigned int *charArray);

#endif