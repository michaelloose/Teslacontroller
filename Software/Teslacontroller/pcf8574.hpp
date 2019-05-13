#include <Arduino.h>
#include <Wire.h>
#define pcf8575adress 0x20

void initialiseButtons(void);
void pcf8575ISR();
void resetUserInput(void);
bool getUserInput(void);
