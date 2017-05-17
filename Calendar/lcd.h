#ifndef lcd_h
#define lcd_h

void lcdInit();
void setTopRowActive();
void setBottomRowActive();
void displayText(char *StringOfCharacters);
void clearScreen();

#endif